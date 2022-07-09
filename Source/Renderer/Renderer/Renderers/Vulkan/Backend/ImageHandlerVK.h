#pragma once
#include "vk_mem_alloc.h"
#include "Renderer/Descriptors/ImageDesc.h"
#include "Renderer/Descriptors/DepthImageDesc.h"
#include "Renderer/Descriptors/SamplerDesc.h"

#include <Base/Types.h>

#include <vulkan/vulkan_core.h>

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class SamplerHandlerVK;
        struct Image;
        struct DepthImage;

        struct IImageHandlerVKData {};

        class ImageHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device, SamplerHandlerVK* samplerHandler);
            void PostInit();

            void OnResize(bool windowResize);

            ImageID CreateImage(const ImageDesc& desc);
            ImageID CreateImageFromSwapchain(const ImageDesc& desc, VkFormat format, VkSwapchainKHR swapChain, u32 imageCount, u32 index);

            DepthImageID CreateDepthImage(const DepthImageDesc& desc);

            const ImageDesc& GetImageDesc(const ImageID id);
            const DepthImageDesc& GetDepthImageDesc(const DepthImageID id);

            VkImage GetImage(const ImageID id);
            VkImageView GetColorView(const ImageID id);

            VkImageView GetColorView(const ImageID id, u32 mipLevel);
            VkImageView GetColorArrayView(const ImageID id, u32 mipLevel);

            uvec2 GetDimension(const ImageID id, u32 mipLevel);
            uvec2 GetDimension(const DepthImageID id);

            VkImage GetImage(const DepthImageID id);
            VkImageView GetDepthView(const DepthImageID id);

            VkDescriptorSet GetImguiImageHandle(const ImageID id);

            bool IsSwapChainImage(const ImageID id);

            u32 GetNumImages();
            u32 GetNumDepthImages();

        private:
            void CreateImage(Image& image, VkFormat& format);
            void CreateImage(DepthImage& image);

            void CreateImageViews(Image& image, VkFormat format);

        private:
            RenderDeviceVK* _device;
            SamplerHandlerVK* _samplerHandler;

            SamplerID _imguiSampler;

            IImageHandlerVKData* _data;
        };
    }
}