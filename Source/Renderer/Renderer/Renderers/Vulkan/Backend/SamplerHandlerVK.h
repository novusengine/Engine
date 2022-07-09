#pragma once
#include "Renderer/Descriptors/SamplerDesc.h"
#include "Renderer/Descriptors/GraphicsPipelineDesc.h"

#include <Base/Types.h>

#include <vulkan/vulkan_core.h>

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class TextureHandlerVK;
        class PipelineHandlerVK;

        struct ISamplerHandlerVKData {};

        class SamplerHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device);

            SamplerID CreateSampler(const SamplerDesc& desc);

            VkSampler& GetSampler(const SamplerID samplerID);

            const SamplerDesc& GetSamplerDesc(const SamplerID samplerID);

        private:
            u64 CalculateSamplerHash(const SamplerDesc& desc);

            bool TryFindExistingSampler(u64 descHash, size_t& id);

        private:
            RenderDeviceVK* _device;

            ISamplerHandlerVKData* _data;
        };
    }
}