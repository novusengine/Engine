#pragma once
#include "vk_mem_alloc.h"
#include "Renderer/Descriptors/BufferDesc.h"

#include <Base/Types.h>

#include <vulkan/vulkan_core.h>

namespace Renderer
{
    class RendererVK;

    namespace Backend
    {
        class RenderDeviceVK;

        struct IBufferHandlerVKData {};

        class BufferHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device);

            void OnFrameStart();

            VkBuffer GetBuffer(BufferID bufferID) const;
            VkDeviceSize GetBufferSize(BufferID bufferID) const;
            VmaAllocation GetBufferAllocation(BufferID bufferID) const;

            BufferID CreateBuffer(BufferDesc& desc);
            BufferID CreateTemporaryBuffer(BufferDesc& desc, u32 framesLifetime);
            void DestroyBuffer(BufferID bufferID);

        private:
            BufferID AcquireNewBufferID();
            void ReturnBufferID(BufferID bufferID);

        private:
            RenderDeviceVK* _device;

            IBufferHandlerVKData* _data;

            friend class ::Renderer::RendererVK;
        };
    }
}