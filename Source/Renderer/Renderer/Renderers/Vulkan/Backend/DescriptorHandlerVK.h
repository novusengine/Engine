#pragma once
#include "Renderer/Descriptors/DescriptorSetDesc.h"
#include "Renderer/Descriptors/TextureDesc.h"
#include "Renderer/Descriptors/TextureArrayDesc.h"
#include "Renderer/DescriptorType.h"

#include <Base/Platform.h>
#include <Base/Types.h>

#include <vulkan/vulkan_core.h>

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class TextureHandlerVK;
        struct DescriptorSet;

        struct IDescriptorHandlerData {};

        class DescriptorHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device, TextureHandlerVK* textureHandler);

            DescriptorSetID CreateDescriptorSet(const DescriptorSetDesc& desc);

            void BindDescriptor(DescriptorSetID setID, u32 binding, VkBuffer buffer, DescriptorType type, u32 frameIndex);
            void BindDescriptor(DescriptorSetID setID, u32 binding, VkImageView image, DescriptorType type, bool isRT, u32 frameIndex);
            void BindDescriptorArray(DescriptorSetID setID, u32 binding, VkImageView image, u32 arrayOffset, DescriptorType type, bool isRT, u32 frameIndex);
            void BindDescriptorArray(DescriptorSetID setID, u32 binding, std::vector<VkImageView>& images, u32 arrayOffset, DescriptorType type, bool isRT, u32 frameIndex);
            void BindDescriptor(DescriptorSetID setID, u32 binding, VkSampler sampler, u32 frameIndex);
            void BindDescriptorArray(DescriptorSetID setID, u32 binding, VkSampler sampler, u32 arrayIndex, u32 frameIndex);
            void BindDescriptor(DescriptorSetID setID, u32 binding, TextureArrayID textureArrayID); // Texture arrays don't take a frameIndex for simplicity, we don't expect them to change per-frame

            // Updates only a range of descriptors in a texture array binding (for incremental updates)
            void UpdateTextureArrayDescriptors(DescriptorSetID setID, u32 binding, const TextureID* textureIDs, u32 startIndex, u32 count);

            VkDescriptorSet GetVkDescriptorSet(DescriptorSetID descriptorSetID, u32 frameIndex);
            VkDescriptorSetLayout GetVkDescriptorSetLayout(DescriptorSetID descriptorSetID);

        private:
            void CreateDescriptorPool();

            void CreateDescriptorSet(DescriptorSet& descriptorSet);

        private:
            RenderDeviceVK* _device;
            TextureHandlerVK* _textureHandler;

            IDescriptorHandlerData* _data;
        };
    }
}