#include "BufferHandlerVK.h"
#include "RenderDeviceVK.h"
#include "DebugMarkerUtilVK.h"

#include <Base/Math/Math.h>
#include <Base/Container/SafeVector.h>

#include <vulkan/vulkan.h>

#include <vector>
#include <queue>

namespace Renderer
{
    namespace Backend
    {
        struct Buffer
        {
            std::string name;
            VmaAllocation allocation;
            VkBuffer buffer;
            VkDeviceSize size;
        };

        struct TemporaryBuffer
        {
            BufferID bufferID;
            u32 framesLifetimeLeft;
        };

        struct BufferHandlerVKData : IBufferHandlerVKData
        {
            std::vector<Buffer> buffers;
            std::queue<BufferID> returnedBufferIDs;

            std::vector<TemporaryBuffer> temporaryBuffers;
        };

        void BufferHandlerVK::Init(RenderDeviceVK* device)
        {
            _device = device;
            _data = new BufferHandlerVKData();
        }

        void BufferHandlerVK::OnFrameStart()
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);

            i64 numBuffers = static_cast<i64>(data.temporaryBuffers.size());

            if (numBuffers > 0)
            {
                for (i64 i = numBuffers - 1; i >= 0; i--)
                {
                    TemporaryBuffer& buffer = data.temporaryBuffers[i];

                    if (--buffer.framesLifetimeLeft == 0)
                    {
                        DestroyBuffer(buffer.bufferID);
                        data.temporaryBuffers.erase(data.temporaryBuffers.begin() + i);
                    }
                }
            }
        }

        VkBuffer BufferHandlerVK::GetBuffer(BufferID bufferID) const
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);

            NC_ASSERT(bufferID != BufferID::Invalid(), "BufferHandler VK : GetBuffer tried to access Invalid BufferID");

            BufferID::type bufferIndex = static_cast<BufferID::type>(bufferID);
            NC_ASSERT(bufferIndex < data.buffers.size(), "BufferHandler VK : GetBuffer tried to access BufferID {} which doesn't exist yet (Size: {})", bufferIndex, data.buffers.size());

            return data.buffers[bufferIndex].buffer;
        }

        VkDeviceSize BufferHandlerVK::GetBufferSize(BufferID bufferID) const
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);

            NC_ASSERT(bufferID != BufferID::Invalid(), "BufferHandler VK : GetBufferSize tried to access Invalid BufferID");

            BufferID::type bufferIndex = static_cast<BufferID::type>(bufferID);
            NC_ASSERT(bufferIndex < data.buffers.size(), "BufferHandler VK : GetBufferSize tried to access BufferID {} which doesn't exist yet (Size: {})", bufferIndex, data.buffers.size());

            return data.buffers[bufferIndex].size;
        }

        VmaAllocation BufferHandlerVK::GetBufferAllocation(BufferID bufferID) const
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);

            NC_ASSERT(bufferID != BufferID::Invalid(), "BufferHandler VK : GetBufferAllocation tried to access Invalid BufferID");

            BufferID::type bufferIndex = static_cast<BufferID::type>(bufferID);
            NC_ASSERT(bufferIndex < data.buffers.size(), "BufferHandler VK : GetBufferAllocation tried to access BufferID {} which doesn't exist yet (Size: {})", bufferIndex, data.buffers.size());

            return data.buffers[bufferIndex].allocation;
        }

        std::string BufferHandlerVK::GetBufferName(BufferID bufferID) const
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);

            NC_ASSERT(bufferID != BufferID::Invalid(), "BufferHandler VK : GetBufferName tried to access Invalid BufferID");

            BufferID::type bufferIndex = static_cast<BufferID::type>(bufferID);
            NC_ASSERT(bufferIndex < data.buffers.size(), "BufferHandler VK : GetBufferName tried to access BufferID {} which doesn't exist yet (Size: {})", bufferIndex, data.buffers.size());

            return data.buffers[bufferIndex].name;
        }

        BufferID BufferHandlerVK::CreateBuffer(BufferDesc& desc)
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);

            VkBufferUsageFlags usage = 0;

            if (desc.usage & BufferUsage::VERTEX_BUFFER)
            {
                usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            }

            if (desc.usage & BufferUsage::INDEX_BUFFER)
            {
                usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            }

            if (desc.usage & BufferUsage::UNIFORM_BUFFER)
            {
                usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            }
            
            if (desc.usage & BufferUsage::STORAGE_BUFFER)
            {
                usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            }

            if (desc.usage & BufferUsage::INDIRECT_ARGUMENT_BUFFER)
            {
                usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
            }

            if (desc.usage & BufferUsage::TRANSFER_SOURCE)
            {
                usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            }

            if (desc.usage & BufferUsage::TRANSFER_DESTINATION)
            {
                usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            }

            u64 descSize = Math::Max(desc.size, 1ul);

            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = descSize;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            
            VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
            if (desc.cpuAccess == BufferCPUAccess::ReadOnly)
            {
                memoryUsage = VMA_MEMORY_USAGE_GPU_TO_CPU;
            }
            else if (desc.cpuAccess == BufferCPUAccess::WriteOnly)
            {
                memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
            }

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = memoryUsage;

            const BufferID bufferID = AcquireNewBufferID();

            if (bufferID == BufferID::Invalid())
            {
                NC_LOG_CRITICAL("BufferHandlerVK: Acquired new buffer ID that was BufferID::Invalid, are we overflowing?");
            }

            bool failed = false;

            Buffer& buffer = data.buffers[(BufferID::type)bufferID];
            buffer.size = descSize;

            if (vmaCreateBuffer(_device->_allocator, &bufferInfo, &allocInfo, &buffer.buffer, &buffer.allocation, nullptr) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create buffer!");
                return BufferID::Invalid();
            }

            buffer.name = desc.name;
            DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)buffer.buffer, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, buffer.name.c_str());

            return bufferID;
        }

        BufferID BufferHandlerVK::CreateTemporaryBuffer(BufferDesc& desc, u32 framesLifetime)
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);

            TemporaryBuffer& temporaryBuffer = data.temporaryBuffers.emplace_back();
            temporaryBuffer.bufferID = CreateBuffer(desc);
            temporaryBuffer.framesLifetimeLeft = framesLifetime;

            return temporaryBuffer.bufferID;
        }

        void BufferHandlerVK::DestroyBuffer(BufferID bufferID)
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);

            Buffer& buffer = data.buffers[(BufferID::type)bufferID];

            vmaDestroyBuffer(_device->_allocator, buffer.buffer, buffer.allocation);

            ReturnBufferID(bufferID);
        }

        u32 BufferHandlerVK::GetNumBuffers()
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);
            
            return static_cast<u32>(data.buffers.size());
        }

        BufferID BufferHandlerVK::AcquireNewBufferID()
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);

            BufferID bufferID;

            // Check if we have returned bufferIDs to use
            if (data.returnedBufferIDs.size() > 0)
            {
                bufferID = data.returnedBufferIDs.front();
                data.returnedBufferIDs.pop();
            }
            else
            {
                // Else create a new one
                bufferID = BufferID(static_cast<BufferID::type>(data.buffers.size()));
                data.buffers.emplace_back();
            }

            return bufferID;
        }

        void BufferHandlerVK::ReturnBufferID(BufferID bufferID)
        {
            BufferHandlerVKData& data = static_cast<BufferHandlerVKData&>(*_data);

            data.returnedBufferIDs.push(bufferID);
        }
    }
}
