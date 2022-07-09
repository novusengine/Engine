#include "UploadBufferHandlerVK.h"
#include "BufferHandlerVK.h"
#include "TextureHandlerVK.h"
#include "CommandListHandlerVK.h"
#include "SemaphoreHandlerVK.h"
#include "RenderDeviceVK.h"
#include "Renderer/Renderers/Vulkan/RendererVK.h"
#include "Renderer/RenderSettings.h"

#include <Base/Container/ConcurrentQueue.h>
#include <Base/Container/SafeVector.h>

#include <tracy/Tracy.hpp>
#include <tracy/TracyVulkan.hpp>

namespace Renderer
{
    namespace Backend
    {
        enum BufferStatus : u8
        {
            READY, // Ready to be used
            CLOSED, // Closed and waiting for execution
            SUBMITTED // Has been executed, but hasn't finished executing
        };

        enum class UploadTaskType : u8
        {
            Invalid,
            UploadToBuffer,
            UploadToTexture,
            CopyBufferToBuffer,
            QueueDestroyBuffer
        };

        struct UploadTask
        {
            UploadTask(UploadTaskType inType) : type(inType) { }

            UploadTaskType type;
        };

        struct UploadToBufferTask : UploadTask
        {
            UploadToBufferTask() : UploadTask(UploadTaskType::UploadToBuffer) { }

            BufferID targetBuffer;
            size_t targetOffset;
            size_t stagingBufferOffset;
            size_t copySize;
        };

        struct UploadToTextureTask : UploadTask
        {
            UploadToTextureTask() : UploadTask(UploadTaskType::UploadToTexture) { }

            TextureID targetTexture = TextureID::Invalid();
            size_t targetOffset;
            size_t stagingBufferOffset;
        };

        struct CopyBufferToBufferTask : UploadTask
        {
            CopyBufferToBufferTask() : UploadTask(UploadTaskType::CopyBufferToBuffer) { }

            BufferID targetBuffer;
            size_t targetOffset;
            BufferID sourceBuffer;
            size_t sourceOffset;
            size_t copySize;
        };

        struct QueueDestroyBufferTask : UploadTask
        {
            QueueDestroyBufferTask() : UploadTask(UploadTaskType::QueueDestroyBuffer) { }

            BufferID buffer;
        };

        struct SubmitTask
        {
            u32 stagingBufferID;
        };

        struct StagingBuffer
        {
            BufferID buffer = BufferID::Invalid();
            void* mappedMemory = nullptr;
            Memory::StackAllocator allocator;

            moodycamel::ConcurrentQueue<UploadTask*> uploadTasks;

            std::atomic<BufferStatus> bufferStatus = BufferStatus::READY;

            std::mutex handleMutex;
            i32 activeHandles = 0;
            i32 totalHandles = 0;

            VkFence fence;
        };

        struct UploadBufferHandlerVKData : IUploadBufferHandlerVKData
        {
            FrameResource<StagingBuffer, 3> stagingBuffers;
            std::atomic<u32> selectedStagingBuffer = 0;

            moodycamel::ConcurrentQueue<SubmitTask> submitTasks;
            std::thread submitThread;

            bool isDirty = true;
            bool needsWait = false;
            std::mutex submitMutex;
            SemaphoreID uploadFinishedSemaphore;
        };

        void UploadBufferHandlerVK::Init(RendererVK* renderer, RenderDeviceVK* device, BufferHandlerVK* bufferHandler, TextureHandlerVK* textureHandler, SemaphoreHandlerVK* semaphoreHandler, CommandListHandlerVK* commandListHandler)
        {
            _renderer = renderer;
            _device = device;
            _bufferHandler = bufferHandler;
            _textureHandler = textureHandler;
            _semaphoreHandler = semaphoreHandler;
            _commandListHandler = commandListHandler;
            _data = new UploadBufferHandlerVKData();

            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);
            for (u32 i = 0; i < data->stagingBuffers.Num; i++)
            {
                StagingBuffer& stagingBuffer = data->stagingBuffers.Get(i);

                BufferDesc bufferDesc;
                bufferDesc.name = "StagingBuffer" + std::to_string(i);
                bufferDesc.size = Settings::STAGING_BUFFER_SIZE;
                bufferDesc.usage = BufferUsage::TRANSFER_SOURCE;
                bufferDesc.cpuAccess = BufferCPUAccess::WriteOnly;

                stagingBuffer.buffer = _bufferHandler->CreateBuffer(bufferDesc);
                stagingBuffer.allocator.Init(Settings::STAGING_BUFFER_SIZE, "StagingBuffer", true, false);

                // Map the buffer
                void* mappedStagingMemory;
                VkResult result = vmaMapMemory(_device->_allocator, _bufferHandler->GetBufferAllocation(stagingBuffer.buffer), &mappedStagingMemory);
                if (result != VK_SUCCESS)
                {
                    DebugHandler::PrintFatal("UploadBufferHandlerVK : vmaMapMemory failed!\n");
                }
                stagingBuffer.mappedMemory = mappedStagingMemory;

                // Create fence
                VkFenceCreateInfo fenceInfo = {};
                fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

                vkCreateFence(_device->_device, &fenceInfo, nullptr, &stagingBuffer.fence);
            }

            data->uploadFinishedSemaphore = _semaphoreHandler->CreateNSemaphore();

            data->submitThread = std::thread(&UploadBufferHandlerVK::RunSubmitThread, this);
        }

        void UploadBufferHandlerVK::ExecuteUploadTasks()
        {
            // Debug if this is uploading the non-filled buffers correctly

            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);

            if (!data->isDirty)
                return;

            ZoneScoped;
            std::scoped_lock lock(data->submitMutex);

            CommandListID commandListID = _commandListHandler->BeginCommandList(QueueType::Graphics);

            VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

#if TRACY_ENABLE
            TracySourceLocation(ExecuteUpload, "ExecuteUpload", tracy::Color::Yellow2);
            tracy::VkCtxManualScope tracyScope(_device->_tracyContext, &ExecuteUpload, true);
            tracyScope.Start(commandBuffer);
#endif

            for (u32 i = 0; i < data->stagingBuffers.Num; i++)
            {
                StagingBuffer& stagingBuffer = data->stagingBuffers.Get(i);

                if (stagingBuffer.bufferStatus == BufferStatus::READY && stagingBuffer.totalHandles > 0)
                {
                    ExecuteStagingBuffer(commandBuffer, stagingBuffer);
                    stagingBuffer.totalHandles = 0; // Not sure about this
                    data->selectedStagingBuffer = (data->selectedStagingBuffer + 1) % data->stagingBuffers.Num;
                }
            }

#if TRACY_ENABLE
            tracyScope.End();
#endif

            VkSemaphore semaphore = _semaphoreHandler->GetVkSemaphore(data->uploadFinishedSemaphore);
            _commandListHandler->AddSignalSemaphore(commandListID, semaphore);
            data->needsWait = true;

            _commandListHandler->EndCommandList(commandListID, VK_NULL_HANDLE);

            // Reset staging buffer allocators and uploadToBufferTasks
            for (u32 i = 0; i < data->stagingBuffers.Num; i++)
            {
                StagingBuffer& stagingBuffer = data->stagingBuffers.Get(i);
                stagingBuffer.allocator.Reset();
            }

            data->isDirty = false;
        }

        void UploadBufferHandlerVK::Clear()
        {
            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);

            // Reset staging buffer allocators and uploadToBufferTasks
            for (u32 i = 0; i < data->stagingBuffers.Num; i++)
            {
                StagingBuffer& stagingBuffer = data->stagingBuffers.Get(i);
                stagingBuffer.allocator.Reset();

                {
                    UploadTask* task;
                    while (stagingBuffer.uploadTasks.try_dequeue(task))
                    {
                        delete task;
                    }
                }

                /*{
                    QueueDestroyBufferTask task;
                    while (stagingBuffer.queueDestroyBufferTasks.try_dequeue(task)) // Just empty the tasks
                    {

                    }
                }*/
            }

            data->isDirty = false;
        }

        std::shared_ptr<UploadBuffer> UploadBufferHandlerVK::CreateUploadBuffer(BufferID targetBuffer, size_t targetOffset, size_t size)
        {
            if (targetBuffer == BufferID::Invalid())
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Tried to create an upload buffer pointing at an invalid buffer");
            }

            if (size > Settings::STAGING_BUFFER_SIZE)
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Requested bigger staging memory than our staging buffer size!");
            }

            if (size == 0)
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Tried to upload 0 bytes of data!");
            }

            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);

            void* mappedMemory = nullptr;
            StagingBufferID stagingBufferID;

            size_t offset = Allocate(size, stagingBufferID, mappedMemory);

            UploadToBufferTask* task = new UploadToBufferTask();
            task->targetBuffer = targetBuffer;
            task->targetOffset = targetOffset;
            task->stagingBufferOffset = offset;
            task->copySize = size;

            size_t targetBufferSize = _bufferHandler->GetBufferSize(targetBuffer);
            if (targetOffset + size > targetBufferSize)
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Upload Overflowed Target Buffer");
            }

            StagingBuffer& stagingBuffer = data->stagingBuffers.Get(static_cast<StagingBufferID::type>(stagingBufferID));
            stagingBuffer.uploadTasks.enqueue(task);

            // Increment number of active handles
            {
                std::scoped_lock lock(stagingBuffer.handleMutex);
                stagingBuffer.activeHandles++;
                stagingBuffer.totalHandles++;
            }

            //u16 stagingBufferIDInt = static_cast<StagingBufferID::type>(stagingBufferID);
            std::shared_ptr<UploadBuffer> uploadBuffer(new UploadBuffer(),
                [&](UploadBuffer* buffer)
                {
                    {
                        std::scoped_lock lock(stagingBuffer.handleMutex);
                        // Decrement the number of active handles into this staging buffer
                        stagingBuffer.activeHandles--;
                    }
                    delete buffer;
                });

            uploadBuffer->size = size;
            uploadBuffer->mappedMemory = mappedMemory;

            data->isDirty = true;
            return uploadBuffer;
        }

        std::shared_ptr<UploadBuffer> UploadBufferHandlerVK::CreateUploadBuffer(TextureID targetTexture, size_t targetOffset, size_t size)
        {
            if (targetTexture == TextureID::Invalid())
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Tried to create an upload buffer pointing at an invalid texture");
            }

            if (size > Settings::STAGING_BUFFER_SIZE)
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Requested bigger staging memory than our staging buffer size!");
            }

            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);

            void* mappedMemory = nullptr;
            StagingBufferID stagingBufferID;
            size_t offset = Allocate(size, stagingBufferID, mappedMemory);

            UploadToTextureTask* task = new UploadToTextureTask();
            task->targetTexture = targetTexture;
            task->targetOffset = targetOffset;
            task->stagingBufferOffset = offset;

            size_t targetTextureSize = _textureHandler->GetTextureSize(targetTexture);
            if (targetOffset + size > targetTextureSize)
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Upload Overflowed Target Buffer");
            }

            StagingBuffer& stagingBuffer = data->stagingBuffers.Get(static_cast<StagingBufferID::type>(stagingBufferID));
            stagingBuffer.uploadTasks.enqueue(task);

            // Increment number of active handles
            {
                std::scoped_lock lock(stagingBuffer.handleMutex);
                stagingBuffer.activeHandles++;
                stagingBuffer.totalHandles++;
            }

            //u16 stagingBufferIDInt = static_cast<StagingBufferID::type>(stagingBufferID);
            std::shared_ptr<UploadBuffer> uploadBuffer(new UploadBuffer(),
                [&](UploadBuffer* buffer)
                {
                    {
                        std::scoped_lock lock(stagingBuffer.handleMutex);
                        // Decrement the number of active handles into this staging buffer
                        stagingBuffer.activeHandles--;
                    }
                    delete buffer;
                });

            uploadBuffer->size = size;
            uploadBuffer->mappedMemory = mappedMemory;

            data->isDirty = true;
            return uploadBuffer;
        }

        void UploadBufferHandlerVK::CopyBufferToBuffer(BufferID targetBuffer, size_t targetOffset, BufferID sourceBuffer, size_t sourceOffset, size_t size)
        {
            if (targetBuffer == BufferID::Invalid())
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Tried to create a CopyBufferToBuffer where the target buffer was invalid");
            }

            if (sourceBuffer == BufferID::Invalid())
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Tried to create a CopyBufferToBuffer where the source buffer was invalid");
            }

            CopyBufferToBufferTask* task = new CopyBufferToBufferTask();
            task->targetBuffer = targetBuffer;
            task->targetOffset = targetOffset;
            task->sourceBuffer = sourceBuffer;
            task->sourceOffset = sourceOffset;
            task->copySize = size;

            size_t targetBufferSize = _bufferHandler->GetBufferSize(targetBuffer);
            if (targetOffset + size > targetBufferSize)
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Upload Overflowed Target Buffer");
            }

            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);

            void* mappedMemory = nullptr;
            StagingBufferID stagingBufferID;
            Allocate(1, stagingBufferID, mappedMemory); // TODO: Figure out a way to not need unnecessary allocate to figure out which staging buffer is "current"

            StagingBuffer& stagingBuffer = data->stagingBuffers.Get(static_cast<StagingBufferID::type>(stagingBufferID));
            stagingBuffer.uploadTasks.enqueue(task);
        }

        void UploadBufferHandlerVK::QueueDestroyBuffer(BufferID buffer)
        {
            if (buffer == BufferID::Invalid())
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Tried to create a CopyBufferToBuffer where the target buffer was invalid");
            }

            QueueDestroyBufferTask* task = new QueueDestroyBufferTask();
            task->buffer = buffer;

            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);

            void* mappedMemory = nullptr;
            StagingBufferID stagingBufferID;
            Allocate(1, stagingBufferID, mappedMemory); // TODO: Figure out a way to not need unnecessary allocate to figure out which staging buffer is "current"

            StagingBuffer& stagingBuffer = data->stagingBuffers.Get(static_cast<StagingBufferID::type>(stagingBufferID));
            stagingBuffer.uploadTasks.enqueue(task);
        }

        SemaphoreID UploadBufferHandlerVK::GetUploadFinishedSemaphore()
        {
            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);
            return data->uploadFinishedSemaphore;
        }

        bool UploadBufferHandlerVK::ShouldWaitForUpload()
        {
            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);
            return data->needsWait;
        }

        void UploadBufferHandlerVK::SetHasWaitedForUpload()
        {
            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);
            data->needsWait = false;
        }

        size_t UploadBufferHandlerVK::Allocate(size_t size, StagingBufferID& stagingBufferID, void*& mappedMemory)
        {
            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);
            size_t offset = 0;

            u32 tries = 0;
            while (tries < 5)
            {
                u32 selectedStagingBuffer = data->selectedStagingBuffer;

                StagingBuffer* stagingBuffer = &data->stagingBuffers.Get(selectedStagingBuffer);
                std::scoped_lock lock(stagingBuffer->handleMutex);

                if (stagingBuffer->bufferStatus == BufferStatus::READY)
                {
                    // Try to allocate in the currently selected stagingbuffer
                    if (stagingBuffer->allocator.TryAllocateOffset(size, 16, offset))
                    {
                        stagingBufferID = StagingBufferID(static_cast<StagingBufferID::type>(selectedStagingBuffer));
                        mappedMemory = static_cast<void*>(&static_cast<u8*>(stagingBuffer->mappedMemory)[offset]);
                        return offset;
                    }

                    // If we got here, close the buffer and create a submit task
                    stagingBuffer->bufferStatus = BufferStatus::CLOSED;

                    SubmitTask submitTask;
                    submitTask.stagingBufferID = selectedStagingBuffer;

                    data->submitTasks.enqueue(submitTask);
                }

                data->selectedStagingBuffer = (selectedStagingBuffer + 1) % data->stagingBuffers.Num;
                tries++;
            }

            // If we after 5 seconds couldn't find a stagingbuffer to use, just wait for the next one to be available
            u32 selectedStagingBuffer = data->selectedStagingBuffer;
            StagingBuffer* stagingBuffer = &data->stagingBuffers.Get(selectedStagingBuffer);

            while (stagingBuffer->bufferStatus != BufferStatus::READY)
            {
                std::this_thread::yield();
            }

            std::scoped_lock lock(stagingBuffer->handleMutex);

            // Try to allocate in the currently selected stagingbuffer
            if (stagingBuffer->allocator.TryAllocateOffset(size, 16, offset))
            {
                stagingBufferID = StagingBufferID(static_cast<StagingBufferID::type>(selectedStagingBuffer));
                mappedMemory = static_cast<void*>(&static_cast<u8*>(stagingBuffer->mappedMemory)[offset]);
                return offset;
            }
            
            DebugHandler::PrintFatal("UploadBufferHandlerVK : Could not allocate in staging buffer after 5 tries and waiting");
            return offset;
        }

        void UploadBufferHandlerVK::ExecuteStagingBuffer(VkCommandBuffer commandBuffer, StagingBuffer& stagingBuffer)
        {
            //UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);

            {
                UploadTask* task;
                while (stagingBuffer.uploadTasks.try_dequeue(task))
                {
                    UploadTaskType uploadType = task->type;

                    if (uploadType == UploadTaskType::Invalid)
                    {
                        DebugHandler::PrintFatal("UploadBufferHandlerVK : UploadTask supplied 'UploadTaskType::Invalid'");
                    }
                    else if (uploadType == UploadTaskType::UploadToBuffer)
                    {
                        UploadToBufferTask* uploadToBufferTask = reinterpret_cast<UploadToBufferTask*>(task);
                        HandleUploadToBufferTask(commandBuffer, stagingBuffer, uploadToBufferTask);
                    }
                    else if (uploadType == UploadTaskType::UploadToTexture)
                    {
                        UploadToTextureTask* uploadToTextureTask = reinterpret_cast<UploadToTextureTask*>(task);
                        HandleUploadToTextureTask(commandBuffer, stagingBuffer, uploadToTextureTask);
                    }
                    else if (uploadType == UploadTaskType::CopyBufferToBuffer)
                    {
                        CopyBufferToBufferTask* copyBufferToBufferTask = reinterpret_cast<CopyBufferToBufferTask*>(task);
                        HandleCopyBufferToBufferTask(commandBuffer, copyBufferToBufferTask);

                        
                    }
                    else if (uploadType == UploadTaskType::QueueDestroyBuffer)
                    {
                        QueueDestroyBufferTask* queueDestroyBufferTask = reinterpret_cast<QueueDestroyBufferTask*>(task);
                        HandleQueueDestroyBufferTask(queueDestroyBufferTask);
                    }

                    delete task;
                }
            }
        }

        void UploadBufferHandlerVK::ExecuteStagingBuffer(StagingBuffer& stagingBuffer)
        {
            // First thread to reach here should submit it
            CommandListID commandListID = _commandListHandler->BeginCommandList(QueueType::Graphics);
            VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

#if TRACY_ENABLE
            TracySourceLocation(RingBufferUpload, "RingBufferUpload", tracy::Color::Yellow2);
            tracy::VkCtxManualScope tracyScope(_device->_tracyContext, &RingBufferUpload, true);
            tracyScope.Start(commandBuffer);
#endif

            ExecuteStagingBuffer(commandBuffer, stagingBuffer);

#if TRACY_ENABLE
            tracyScope.End();
#endif

            _commandListHandler->EndCommandList(commandListID, stagingBuffer.fence);
        }

        void UploadBufferHandlerVK::WaitForStagingBuffer(StagingBuffer& stagingBuffer)
        {
            if (stagingBuffer.bufferStatus != BufferStatus::SUBMITTED)
                return;

            u64 timeout = 5000000000; // 5 seconds in nanoseconds
            VkResult result = vkWaitForFences(_device->_device, 1, &stagingBuffer.fence, true, timeout);

            if (result == VK_TIMEOUT)
            {
                DebugHandler::PrintFatal("UploadBufferHandlerVK : Waiting for staging buffer fence took longer than 5 seconds, something is wrong!");
            }

            vkResetFences(_device->_device, 1, &stagingBuffer.fence);

            // Reset staging buffer
            stagingBuffer.allocator.Reset();
        }

        void UploadBufferHandlerVK::HandleUploadToBufferTask(VkCommandBuffer commandBuffer, StagingBuffer& stagingBuffer, UploadToBufferTask* uploadToBufferTask)
        {
            VkBuffer dstBuffer = _bufferHandler->GetBuffer(uploadToBufferTask->targetBuffer);
            VkBuffer srcBuffer = _bufferHandler->GetBuffer(stagingBuffer.buffer);

            VkBufferCopy copyRegion = {};
            copyRegion.dstOffset = uploadToBufferTask->targetOffset;
            copyRegion.srcOffset = uploadToBufferTask->stagingBufferOffset;
            copyRegion.size = uploadToBufferTask->copySize;
            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

            // TODO: Figure out a smarter way to do this so we don't need to add a barrier if it hasn't been written to before
            VkBufferMemoryBarrier bufferBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
            bufferBarrier.buffer = dstBuffer;
            bufferBarrier.size = VK_WHOLE_SIZE;
            bufferBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            size_t srcBufferSize = _bufferHandler->GetBufferSize(stagingBuffer.buffer);
            size_t dstBufferSize = _bufferHandler->GetBufferSize(uploadToBufferTask->targetBuffer);

            if (copyRegion.srcOffset + copyRegion.size > srcBufferSize)
            {
                DebugHandler::PrintFatal("[UploadBufferHandlerVK::HandleUploadToBufferTask] Source Buffer out of bounds!");
            }
            if (copyRegion.dstOffset + copyRegion.size > dstBufferSize)
            {
                DebugHandler::PrintFatal("[UploadBufferHandlerVK::HandleUploadToBufferTask] Destination Buffer out of bounds!");
            }

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &bufferBarrier, 0, nullptr);
        }

        void UploadBufferHandlerVK::HandleUploadToTextureTask(VkCommandBuffer commandBuffer, StagingBuffer& stagingBuffer, UploadToTextureTask* uploadToTextureTask)
        {
            VkBuffer srcBuffer = _bufferHandler->GetBuffer(stagingBuffer.buffer);

            size_t srcBufferSize = _bufferHandler->GetBufferSize(stagingBuffer.buffer);
            size_t textureSize = _textureHandler->GetTextureSize(uploadToTextureTask->targetTexture);

            if (uploadToTextureTask->stagingBufferOffset + textureSize > srcBufferSize)
            {
                DebugHandler::PrintFatal("[UploadBufferHandlerVK::HandleUploadToTextureTask] Source Buffer out of bounds!");
            }

            _textureHandler->CopyBufferToImage(commandBuffer, srcBuffer, uploadToTextureTask->stagingBufferOffset, uploadToTextureTask->targetTexture);
        }

        void UploadBufferHandlerVK::HandleCopyBufferToBufferTask(VkCommandBuffer commandBuffer, CopyBufferToBufferTask* copyBufferToBufferTask)
        {
            VkBuffer dstBuffer = _bufferHandler->GetBuffer(copyBufferToBufferTask->targetBuffer);
            VkBuffer srcBuffer = _bufferHandler->GetBuffer(copyBufferToBufferTask->sourceBuffer);

            {
                // TODO: Figure out a smarter way to do this so we don't need to add a barrier if it hasn't been written to before
                VkBufferMemoryBarrier bufferBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
                bufferBarrier.buffer = srcBuffer;
                bufferBarrier.size = VK_WHOLE_SIZE;
                bufferBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                bufferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &bufferBarrier, 0, nullptr);
            }

            VkBufferCopy copyRegion = {};
            copyRegion.dstOffset = copyBufferToBufferTask->targetOffset;
            copyRegion.srcOffset = copyBufferToBufferTask->sourceOffset;
            copyRegion.size = copyBufferToBufferTask->copySize;

            size_t srcBufferSize = _bufferHandler->GetBufferSize(copyBufferToBufferTask->sourceBuffer);
            size_t dstBufferSize = _bufferHandler->GetBufferSize(copyBufferToBufferTask->targetBuffer);

            if (copyRegion.srcOffset + copyRegion.size > srcBufferSize)
            {
                DebugHandler::PrintFatal("[UploadBufferHandlerVK::HandleCopyBufferToBufferTask] Source Buffer out of bounds!");
            }
            if (copyRegion.dstOffset + copyRegion.size > dstBufferSize)
            {
                DebugHandler::PrintFatal("[UploadBufferHandlerVK::HandleCopyBufferToBufferTask] Destination Buffer out of bounds!");
            }

            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

            {
                // TODO: Figure out a smarter way to do this so we don't need to add a barrier if it hasn't been written to before
                VkBufferMemoryBarrier bufferBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
                bufferBarrier.buffer = dstBuffer;
                bufferBarrier.size = VK_WHOLE_SIZE;
                bufferBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                bufferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &bufferBarrier, 0, nullptr);
            }
        }

        void UploadBufferHandlerVK::HandleQueueDestroyBufferTask(QueueDestroyBufferTask* queueDestroyBufferTask)
        {
            _renderer->DestroyBuffer(queueDestroyBufferTask->buffer);
        }

        void UploadBufferHandlerVK::RunSubmitThread()
        {
            UploadBufferHandlerVKData* data = static_cast<UploadBufferHandlerVKData*>(_data);

            while (true)
            {
                std::vector<SubmitTask> delayedSubmitTasks;

                SubmitTask submitTask;
                while (data->submitTasks.try_dequeue(submitTask))
                {
                    StagingBuffer& stagingBuffer = data->stagingBuffers.Get(submitTask.stagingBufferID);
                    std::scoped_lock lock(stagingBuffer.handleMutex);

                    // If there are still open handles to this staging buffer, delay it until the next time we check
                    if (stagingBuffer.activeHandles > 0)
                    {
                        delayedSubmitTasks.push_back(submitTask);
                        continue;
                    }

                    std::scoped_lock submitLock(data->submitMutex);
                    stagingBuffer.bufferStatus = BufferStatus::SUBMITTED;

                    ExecuteStagingBuffer(stagingBuffer);
                    WaitForStagingBuffer(stagingBuffer); // TODO: See if we can move this wait later, maybe add waitTasks?

                    stagingBuffer.totalHandles = 0;
                    stagingBuffer.bufferStatus = BufferStatus::READY;
                }

                // Push the delayed tasks back into the queue
                for (SubmitTask& submitTask : delayedSubmitTasks)
                {
                    data->submitTasks.enqueue(submitTask);
                }

                //std::this_thread::sleep_for(std::chrono::microseconds(100));
                std::this_thread::yield();
            }
        }
    }
}
