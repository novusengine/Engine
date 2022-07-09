#pragma once
#include "Renderer/Descriptors/UploadBuffer.h"
#include "Renderer/Descriptors/TextureDesc.h"
#include "Renderer/Descriptors/SemaphoreDesc.h"

#include <Base/Types.h>
#include <Base/Memory/StackAllocator.h>

struct VkCommandBuffer_T;
typedef VkCommandBuffer_T* VkCommandBuffer;

namespace Renderer
{
    class RendererVK;

    namespace Backend
    {
        class RenderDeviceVK;
        class BufferHandlerVK;
        class TextureHandlerVK;
        class SemaphoreHandlerVK;
        class CommandListHandlerVK;

        struct StagingBuffer;
        struct UploadToBufferTask;
        struct UploadToTextureTask;
        struct CopyBufferToBufferTask;
        struct QueueDestroyBufferTask;

        struct IUploadBufferHandlerVKData {};

        class UploadBufferHandlerVK
        {
        public:
            void Init(RendererVK* renderer, RenderDeviceVK* device, BufferHandlerVK* bufferHandler, TextureHandlerVK* textureHandler, SemaphoreHandlerVK* semaphoreHandler, CommandListHandlerVK* commandListHandler);
            void ExecuteUploadTasks();
            void Clear();

            [[nodiscard]] std::shared_ptr<UploadBuffer> CreateUploadBuffer(BufferID targetBuffer, size_t targetOffset, size_t size);
            [[nodiscard]] std::shared_ptr<UploadBuffer> CreateUploadBuffer(TextureID targetTexture, size_t targetOffset, size_t size);
            void CopyBufferToBuffer(BufferID targetBuffer, size_t targetOffset, BufferID sourceBuffer, size_t sourceOffset, size_t size);
            void QueueDestroyBuffer(BufferID buffer);

            SemaphoreID GetUploadFinishedSemaphore();
            bool ShouldWaitForUpload();
            void SetHasWaitedForUpload();
        private:
            size_t Allocate(size_t size, StagingBufferID& stagingBufferID, void*& mappedMemory);
            void ExecuteStagingBuffer(VkCommandBuffer commandBuffer, StagingBuffer& stagingBuffer);
            void ExecuteStagingBuffer(StagingBuffer& stagingBuffer);
            void WaitForStagingBuffer(StagingBuffer& stagingBuffer);

            void HandleUploadToBufferTask(VkCommandBuffer commandBuffer, StagingBuffer& stagingBuffer, UploadToBufferTask* uploadToBufferTask);
            void HandleUploadToTextureTask(VkCommandBuffer commandBuffer, StagingBuffer& stagingBuffer, UploadToTextureTask* uploadToTextureTask);
            void HandleCopyBufferToBufferTask(VkCommandBuffer commandBuffer, CopyBufferToBufferTask* copyBufferToBufferTask);
            void HandleQueueDestroyBufferTask(QueueDestroyBufferTask* queueDestroyBufferTask);

            void RunSubmitThread();

        private:
            RendererVK* _renderer;
            RenderDeviceVK* _device;
            BufferHandlerVK* _bufferHandler;
            TextureHandlerVK* _textureHandler;
            SemaphoreHandlerVK* _semaphoreHandler;
            CommandListHandlerVK* _commandListHandler;

            IUploadBufferHandlerVKData* _data;
        };
    }
}