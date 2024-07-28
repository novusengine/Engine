#include "ImageHandlerVK.h"
#include "RenderDeviceVK.h"
#include "SamplerHandlerVK.h"
#include "FormatConverterVK.h"
#include "DebugMarkerUtilVK.h"

#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>

#include <vulkan/vulkan.h>
#include <imgui/backends/imgui_impl_vulkan.h>

#include <vector>
#include <mutex>

namespace Renderer
{
    namespace Backend
    {
        u32 PreviousPow2(u32 v)
        {
            u32 r = 1;

            while (r * 2 < v)
                r *= 2;

            return r;
        }

        u32 GetImageMipLevels(u32 width, u32 height)
        {
            u32 result = 1;

            while (width > 1 || height > 1)
            {
                result++;
                width /= 2;
                height /= 2;
            }

            return result;
        }

        struct Image
        {
            ImageDesc desc;

            VmaAllocation allocation;
            VkImage image;
            VkImageView colorView;
            std::vector<VkImageView> mipViews;
            std::vector<VkImageView> mipArrayViews;

            VkDescriptorSet imguiTextureHandle = 0;

            bool isSwapchain = false;
        };

        struct DepthImage
        {
            DepthImageDesc desc;

            VmaAllocation allocation;
            VkImage image;
            VkImageView depthView;
        };

        struct ImageHandlerVKData : IImageHandlerVKData
        {
            std::vector<Image> images;
            std::vector<DepthImage> depthImages;
        };

        void ImageHandlerVK::Init(RenderDeviceVK* device, SamplerHandlerVK* samplerHandler)
        {
            _device = device;
            _samplerHandler = samplerHandler;
            _data = new ImageHandlerVKData();
        }

        void ImageHandlerVK::PostInit()
        {
            // Create imgui sampler
            SamplerDesc samplerDesc;
            samplerDesc.enabled = true;
            samplerDesc.filter = SamplerFilter::MIN_MAG_MIP_LINEAR;
            samplerDesc.addressU = TextureAddressMode::WRAP;
            samplerDesc.addressV = TextureAddressMode::WRAP;
            samplerDesc.addressW = TextureAddressMode::CLAMP;
            samplerDesc.shaderVisibility = ShaderVisibility::PIXEL;
            _imguiSampler = _samplerHandler->CreateSampler(samplerDesc);
        }

        void ImageHandlerVK::OnResize(bool windowResize)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            ImageDimensionType resizeType = windowResize ? ImageDimensionType::DIMENSION_SCALE_WINDOW : ImageDimensionType::DIMENSION_SCALE_RENDERSIZE;
            ImageDimensionType resizePyramidType = windowResize ? ImageDimensionType::DIMENSION_PYRAMID_WINDOW : ImageDimensionType::DIMENSION_PYRAMID_RENDERSIZE;

            std::vector<VkDescriptorSet> descriptorsToFree;

            // Recreate color images
            for (auto& image : data.images)
            {
                if (image.isSwapchain)
                    continue;

                if (image.desc.dimensionType == resizeType || image.desc.dimensionType == resizePyramidType)
                {
                    descriptorsToFree.push_back(image.imguiTextureHandle);

                    // Destroy old image
                    vkDestroyImageView(_device->_device, image.colorView, nullptr);
                    vmaDestroyImage(_device->_allocator, image.image, image.allocation);

                    for (u32 i = 0; i < image.desc.mipLevels; i++)
                    {
                        vkDestroyImageView(_device->_device, image.mipViews[i], nullptr);
                    }
                    
                    // Create new
                    VkFormat format;
                    CreateImage(image, format);
                    CreateImageViews(image, format);

                    // Transition image from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_GENERAL
                    _device->TransitionImageLayout(image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, image.desc.depth, image.desc.mipLevels);
                }
            }

            // Recreate depth images
            for (auto& image : data.depthImages)
            {
                if (image.desc.dimensionType == resizeType || image.desc.dimensionType == resizePyramidType)
                {
                    // Destroy old image
                    vkDestroyImageView(_device->_device, image.depthView, nullptr);
                    vmaDestroyImage(_device->_allocator, image.image, image.allocation);

                    // Create new
                    CreateImage(image);
                }
            }

            if (descriptorsToFree.size() > 0)
            {
                vkFreeDescriptorSets(_device->_device, _device->_imguiContext->imguiPool, static_cast<u32>(descriptorsToFree.size()), descriptorsToFree.data());
                descriptorsToFree.clear();
            }
        }

        ImageID ImageHandlerVK::CreateImage(const ImageDesc& desc)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            size_t nextHandle = data.images.size();

            // Make sure we haven't exceeded the limit of the ImageID type, if this hits you need to change type of ImageID to something bigger
            assert(nextHandle < ImageID::MaxValue());

            Image image;
            image.desc = desc;

            assert(desc.dimensions.x > 0); // Make sure the width is valid
            assert(desc.dimensions.y > 0); // Make sure the height is valid
            assert(desc.depth > 0); // Make sure the depth is valid
            assert(desc.format != ImageFormat::UNKNOWN); // Make sure the format is valid

            VkFormat format;
            CreateImage(image, format);
            CreateImageViews(image, format);

            // Transition image from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_GENERAL
            _device->TransitionImageLayout(image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, image.desc.depth, image.desc.mipLevels);

            data.images.push_back(image);

            return ImageID(static_cast<ImageID::type>(nextHandle));
        }

        ImageID ImageHandlerVK::CreateImageFromSwapchain(const ImageDesc& desc, VkFormat format, VkSwapchainKHR swapChain, u32 imageCount, u32 index)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            size_t nextHandle = data.images.size();

            // Make sure we haven't exceeded the limit of the ImageID type, if this hits you need to change type of ImageID to something bigger
            assert(nextHandle < ImageID::MaxValue());

            Image image;
            image.desc = desc;
            image.isSwapchain = true;

            assert(desc.dimensions.x > 0); // Make sure the width is valid
            assert(desc.dimensions.y > 0); // Make sure the height is valid
            assert(desc.depth > 0); // Make sure the depth is valid
            assert(desc.format != ImageFormat::UNKNOWN); // Make sure the format is valid

            // Get VkImage from swapchain
            VkImage images[3];
            vkGetSwapchainImagesKHR(_device->_device, swapChain, &imageCount, images);
            image.image = images[index];

            for (u32 i = 0; i < 3; i++)
            {
                std::string imageName = "swapchain";
                DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)images[index], VK_OBJECT_TYPE_IMAGE, imageName.c_str());
            }

            CreateImageViews(image, format);

            // Transition image from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_GENERAL
            _device->TransitionImageLayout(image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, image.desc.depth, image.desc.mipLevels);

            data.images.push_back(image);

            return ImageID(static_cast<ImageID::type>(nextHandle));
        }

        DepthImageID ImageHandlerVK::CreateDepthImage(const DepthImageDesc& desc)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            size_t nextHandle = data.depthImages.size();

            // Make sure we haven't exceeded the limit of the DepthImageID type, if this hits you need to change type of DepthImageID to something bigger
            assert(nextHandle < DepthImageID::MaxValue());

            DepthImage image;
            image.desc = desc;

            CreateImage(image);

            data.depthImages.push_back(image);

            return DepthImageID(static_cast<DepthImageID::type>(nextHandle));
        }

        const ImageDesc& ImageHandlerVK::GetImageDesc(const ImageID id)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == ImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetImageDesc with invalid ImageID");
            }

            if (static_cast<ImageID::type>(id) >= data.images.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetImageDesc with bad ImageID (%u)", static_cast<ImageID::type>(id));
            }

            return data.images[static_cast<ImageID::type>(id)].desc;
        }

        const DepthImageDesc& ImageHandlerVK::GetImageDesc(const DepthImageID id)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);
            
            // Lets make sure this id exists
            if (id == DepthImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetDepthImageDesc with invalid DepthImageID");
            }

            if (static_cast<DepthImageID::type>(id) >= data.depthImages.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetDepthImageDesc with bad DepthImageID (%u)", static_cast<DepthImageID::type>(id));
            }

            return data.depthImages[static_cast<DepthImageID::type>(id)].desc;
        }

        uvec2 ImageHandlerVK::GetDimensions(const ImageID id, u32 mipLevel)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == ImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetDimension with invalid ImageID");
            }

            if (static_cast<ImageID::type>(id) >= data.images.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetDimension with bad ImageID (%u)", static_cast<ImageID::type>(id));
            }

            const ImageDesc& desc = GetImageDesc(id);

            f32 width = desc.dimensions.x;
            f32 height = desc.dimensions.y;
            u32 mips = desc.mipLevels;

            u32 uwidth = static_cast<u32>(width);
            u32 uheight = static_cast<u32>(height);

            // If the supplied dimensions is a % of render size
            if (desc.dimensionType == ImageDimensionType::DIMENSION_SCALE_RENDERSIZE)
            {
                vec2 renderSize = _device->GetRenderSize();
                width *= renderSize.x;
                height *= renderSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);
            }
            else if (desc.dimensionType == ImageDimensionType::DIMENSION_SCALE_WINDOW) // If the supplied dimensions is a % of window
            {
                vec2 windowSize = _device->GetMainWindowSize();
                width *= windowSize.x;
                height *= windowSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);
            }
            else if (desc.dimensionType == ImageDimensionType::DIMENSION_PYRAMID_RENDERSIZE)
            {
                vec2 renderSize = _device->GetRenderSize();
                width *= renderSize.x;
                height *= renderSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);

                uwidth = (PreviousPow2(uwidth));
                uheight = (PreviousPow2(uheight));
                mips = (GetImageMipLevels(uwidth, uheight));
            }
            else if (desc.dimensionType == ImageDimensionType::DIMENSION_PYRAMID_WINDOW)
            {
                vec2 windowSize = _device->GetMainWindowSize();
                width *= windowSize.x;
                height *= windowSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);

                uwidth = (PreviousPow2(uwidth));
                uheight = (PreviousPow2(uheight));
                mips = (GetImageMipLevels(uwidth, uheight));
            }

            u32 mip = glm::min(mips, mipLevel);

            return { uwidth >> mip, uheight >> mip };
        }

        uvec2 ImageHandlerVK::GetDimensions(const DepthImageID id)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == DepthImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetDimension with invalid DepthImageID");
            }

            if (static_cast<DepthImageID::type>(id) >= data.depthImages.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetDimension with bad DepthImageID (%u)", static_cast<DepthImageID::type>(id));
            }

            const DepthImageDesc& desc = GetImageDesc(id);

            f32 width = desc.dimensions.x;
            f32 height = desc.dimensions.y;

            u32 uwidth = static_cast<u32>(width);
            u32 uheight = static_cast<u32>(height);

            // If the supplied dimensions is a % of window size
            if (desc.dimensionType == ImageDimensionType::DIMENSION_SCALE_RENDERSIZE)
            {
                vec2 renderSize = _device->GetRenderSize();
                width *= renderSize.x;
                height *= renderSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);
            }
            else if (desc.dimensionType == ImageDimensionType::DIMENSION_SCALE_WINDOW)
            {
                vec2 windowSize = _device->GetMainWindowSize();
                width *= windowSize.x;
                height *= windowSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);
            }
            else if (desc.dimensionType == ImageDimensionType::DIMENSION_PYRAMID_RENDERSIZE)
            {
                vec2 renderSize = _device->GetRenderSize();
                width *= renderSize.x;
                height *= renderSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);

                uwidth = (PreviousPow2(uwidth));
                uheight = (PreviousPow2(uheight));
            }
            else if (desc.dimensionType == ImageDimensionType::DIMENSION_PYRAMID_WINDOW)
            {
                vec2 windowSize = _device->GetMainWindowSize();
                width *= windowSize.x;
                height *= windowSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);

                uwidth = (PreviousPow2(uwidth));
                uheight = (PreviousPow2(uheight));
            }

            return { uwidth, uheight };
        }

        VkImage ImageHandlerVK::GetImage(const ImageID id)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == ImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetImage with invalid ImageID");
            }

            if (static_cast<ImageID::type>(id) >= data.images.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetImage with bad ImageID (%u)", static_cast<ImageID::type>(id));
            }

            return data.images[static_cast<ImageID::type>(id)].image;
        }

        VkImageView ImageHandlerVK::GetColorView(const ImageID id)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == ImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetColorView with invalid ImageID");
            }

            if (static_cast<ImageID::type>(id) >= data.images.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetColorView with bad ImageID (%u)", static_cast<ImageID::type>(id));
            }

            return data.images[static_cast<ImageID::type>(id)].colorView;
        }

        VkImageView ImageHandlerVK::GetColorView(const ImageID id, u32 mipLevel)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == ImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetColorView with invalid ImageID");
            }

            if (static_cast<ImageID::type>(id) >= data.images.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetColorView with bad ImageID (%u)", static_cast<ImageID::type>(id));
            }

            ImageID::type tid = static_cast<ImageID::type>(id);
            if (mipLevel >= data.images[tid].mipViews.size())
            {
                NC_LOG_CRITICAL("ImageHandlerVK: GetColorView tried to get a mipLevel that doesn't exist");
            }

            if (mipLevel == 0)
            {
                return GetColorView(id);
            }

            return data.images[tid].mipViews[mipLevel];
        }

        VkImageView ImageHandlerVK::GetColorArrayView(const ImageID id, u32 mipLevel)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == ImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetColorView with invalid ImageID");
            }

            if (static_cast<ImageID::type>(id) >= data.images.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetColorView with bad ImageID (%u)", static_cast<ImageID::type>(id));
            }
            
            ImageID::type tid = static_cast<ImageID::type>(id);
            if (mipLevel >= data.images[tid].mipArrayViews.size())
            {
                NC_LOG_CRITICAL("ImageHandlerVK: GetColorArrayView tried to get a mipLevel that doesn't exist");
            }

            return data.images[tid].mipArrayViews[mipLevel];
        }

        VkImage ImageHandlerVK::GetImage(const DepthImageID id)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == DepthImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetImage with invalid DepthImageID");
            }

            if (static_cast<DepthImageID::type>(id) >= data.depthImages.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetImage with bad DepthImageID (%u)", static_cast<DepthImageID::type>(id));
            }

            return data.depthImages[static_cast<DepthImageID::type>(id)].image;
        }

        VkImageView ImageHandlerVK::GetDepthView(const DepthImageID id)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == DepthImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetDepthView with invalid DepthImageID");
            }

            if (static_cast<DepthImageID::type>(id) >= data.depthImages.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetDepthView with bad DepthImageID (%u)", static_cast<DepthImageID::type>(id));
            }

            return data.depthImages[static_cast<DepthImageID::type>(id)].depthView;
        }

        VkDescriptorSet ImageHandlerVK::GetImguiImageHandle(const ImageID id)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == ImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetColorView with invalid ImageID");
            }

            if (static_cast<ImageID::type>(id) >= data.images.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetColorView with bad ImageID (%u)", static_cast<ImageID::type>(id));
            }

            return data.images[static_cast<ImageID::type>(id)].imguiTextureHandle;
        }

        bool ImageHandlerVK::IsSwapChainImage(const ImageID id)
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Lets make sure this id exists
            if (id == ImageID::Invalid())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetColorView with invalid ImageID");
            }

            if (static_cast<ImageID::type>(id) >= data.images.size())
            {
                NC_LOG_CRITICAL("Tried to ImageHandlerVK::GetColorView with bad ImageID (%u)", static_cast<ImageID::type>(id));
            }

            return data.images[static_cast<ImageID::type>(id)].isSwapchain;
        }

        u32 ImageHandlerVK::GetNumImages()
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            return static_cast<u32>(data.images.size());
        }

        u32 ImageHandlerVK::GetNumDepthImages()
        {
            ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            return static_cast<u32>(data.depthImages.size());
        }

        void ImageHandlerVK::CreateImage(Image& image, VkFormat& format)
        {
            // Create image
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.pNext = nullptr;
            imageInfo.flags = 0; // TODO: VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT and VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT? https://github.com/DiligentGraphics/DiligentCore/blob/1edcafe9bd41bdde86869d4e1c0212c78ce123b7/Graphics/GraphicsEngineVulkan/src/TextureVkImpl.cpp

            if (image.desc.depth == 1)
            {
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
            }
            else
            {
                NC_LOG_CRITICAL("Non-3d images is currently unsupported");
            }

            f32 width = image.desc.dimensions.x;
            f32 height = image.desc.dimensions.y;
            u32 mips = image.desc.mipLevels;

            u32 uwidth = static_cast<u32>(width);
            u32 uheight = static_cast<u32>(height);

            // If the supplied dimensions is a % of render size
            if (image.desc.dimensionType == ImageDimensionType::DIMENSION_SCALE_RENDERSIZE)
            {
                vec2 renderSize = _device->GetRenderSize();
                width *= renderSize.x;
                height *= renderSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);
            }
            else if (image.desc.dimensionType == ImageDimensionType::DIMENSION_SCALE_WINDOW) // If the supplied dimensions is a % of window size
            {
                vec2 windowSize = _device->GetMainWindowSize();
                width *= windowSize.x;
                height *= windowSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);
            }
            else if (image.desc.dimensionType == ImageDimensionType::DIMENSION_PYRAMID_RENDERSIZE)
            {
                vec2 renderSize = _device->GetRenderSize();
                width *= renderSize.x;
                height *= renderSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);

                uwidth = (PreviousPow2(uwidth));
                uheight = (PreviousPow2(uheight));
                mips = (GetImageMipLevels(uwidth, uheight));

                image.desc.mipLevels = mips;
            }
            else if (image.desc.dimensionType == ImageDimensionType::DIMENSION_PYRAMID_WINDOW)
            {
                vec2 windowSize = _device->GetMainWindowSize();
                width *= windowSize.x;
                height *= windowSize.y;

                uwidth = static_cast<u32>(width);
                uheight = static_cast<u32>(height);

                uwidth = (PreviousPow2(uwidth));
                uheight = (PreviousPow2(uheight));
                mips = (GetImageMipLevels(uwidth, uheight));

                image.desc.mipLevels = mips;
            }

            format = FormatConverterVK::ToVkFormat(image.desc.format);

            imageInfo.format = format;
            imageInfo.extent.width = uwidth;
            imageInfo.extent.height = uheight;
            imageInfo.extent.depth = image.desc.depth;
            imageInfo.mipLevels = mips;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = FormatConverterVK::ToVkSampleCount(image.desc.sampleCount);
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            // We will sample directly from the color attachment
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; // Transfers
            imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // RTV
            imageInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT; // SRV 
            imageInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT; // UAV

            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.queueFamilyIndexCount = 0;
            imageInfo.pQueueFamilyIndices = nullptr;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            if (vmaCreateImage(_device->_allocator, &imageInfo, &allocInfo, &image.image, &image.allocation, nullptr) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create image!");
            }

            DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)image.image, VK_OBJECT_TYPE_IMAGE, image.desc.debugName.c_str());
        }

        void ImageHandlerVK::CreateImage(DepthImage& image)
        {
            // Create image
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.pNext = nullptr;
            imageInfo.flags = 0; // TODO: VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT and VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT? https://github.com/DiligentGraphics/DiligentCore/blob/1edcafe9bd41bdde86869d4e1c0212c78ce123b7/Graphics/GraphicsEngineVulkan/src/TextureVkImpl.cpp
            imageInfo.imageType = VK_IMAGE_TYPE_2D;

            f32 width = image.desc.dimensions.x;
            f32 height = image.desc.dimensions.y;

            // If the supplied dimensions is a % of window size
            if (image.desc.dimensionType == ImageDimensionType::DIMENSION_SCALE_RENDERSIZE)
            {
                vec2 renderSize = _device->GetRenderSize();
                width *= renderSize.x;
                height *= renderSize.y;
            }
            else if (image.desc.dimensionType == ImageDimensionType::DIMENSION_SCALE_WINDOW)
            {
                vec2 windowSize = _device->GetMainWindowSize();
                width *= windowSize.x;
                height *= windowSize.y;
            }

            imageInfo.format = FormatConverterVK::ToVkFormat(image.desc.format);
            imageInfo.extent.width = static_cast<u32>(width);
            imageInfo.extent.height = static_cast<u32>(height);
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = FormatConverterVK::ToVkSampleCount(image.desc.sampleCount);
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            // We will sample directly from the color attachment
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; // Transfers
            imageInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; // DSV
            imageInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT; // SRV 
            //imageInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT; // UAV TODO

            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.queueFamilyIndexCount = 0;
            imageInfo.pQueueFamilyIndices = nullptr;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            if (vmaCreateImage(_device->_allocator, &imageInfo, &allocInfo, &image.image, &image.allocation, nullptr) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create image!");
            }

            DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)image.image, VK_OBJECT_TYPE_IMAGE, image.desc.debugName.c_str());

            // Create Depth View
            VkImageViewCreateInfo depthViewInfo = {};
            depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            depthViewInfo.image = image.image;
            depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

            depthViewInfo.format = imageInfo.format;
            depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthViewInfo.subresourceRange.baseMipLevel = 0;
            depthViewInfo.subresourceRange.levelCount = 1;
            depthViewInfo.subresourceRange.baseArrayLayer = 0;
            depthViewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(_device->_device, &depthViewInfo, nullptr, &image.depthView) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create depth image view!");
            }

            DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)image.depthView, VK_OBJECT_TYPE_IMAGE_VIEW, image.desc.debugName.c_str());

            // Transition image from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            _device->TransitionImageLayout(image.image, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 1, 1);
        }

        void ImageHandlerVK::CreateImageViews(Image& image, VkFormat format)
        {
            // ImageHandlerVKData& data = static_cast<ImageHandlerVKData&>(*_data);

            // Create Color View for individual mips
            VkImageViewCreateInfo colorViewInfo = {};
            colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            colorViewInfo.image = image.image;
            if (image.desc.depth == 1)
            {
                colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            }
            else
            {
                NC_LOG_CRITICAL("Non-2d images is currently unsupported");
            }

            colorViewInfo.format = format;
            colorViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
            colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorViewInfo.subresourceRange.baseMipLevel = 0;
            colorViewInfo.subresourceRange.levelCount = image.desc.mipLevels;
            colorViewInfo.subresourceRange.baseArrayLayer = 0;
            colorViewInfo.subresourceRange.layerCount = 1;

            //we want a full mip chain of views
            image.mipViews.resize(image.desc.mipLevels);
            image.mipArrayViews.resize(image.desc.mipLevels);

            for (u32 i = 0; i < image.desc.mipLevels; ++i)
            {
                VkImageViewCreateInfo pyramidLevelInfo = colorViewInfo;
                pyramidLevelInfo.subresourceRange.baseMipLevel = i;
                pyramidLevelInfo.subresourceRange.levelCount = 1;

                if (vkCreateImageView(_device->_device, &pyramidLevelInfo, nullptr, &image.mipViews[i]) != VK_SUCCESS)
                {
                    NC_LOG_CRITICAL("Failed to create color image view!");
                }

                std::string mipName = image.desc.debugName + " mip " + std::to_string(i);
                DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)image.mipViews[i], VK_OBJECT_TYPE_IMAGE_VIEW, mipName.c_str());
            }

            if (vkCreateImageView(_device->_device, &colorViewInfo, nullptr, &image.colorView) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create color image view!");
            }

            DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)image.colorView, VK_OBJECT_TYPE_IMAGE_VIEW, image.desc.debugName.c_str());

            // Also create mipViews that are arrays of the specific view, down to the last mip
            colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;

            for (u32 i = 0; i < image.desc.mipLevels; ++i)
            {
                VkImageViewCreateInfo pyramidLevelInfo = colorViewInfo;
                pyramidLevelInfo.subresourceRange.baseMipLevel = i;
                pyramidLevelInfo.subresourceRange.levelCount = image.desc.mipLevels - i;

                if (vkCreateImageView(_device->_device, &pyramidLevelInfo, nullptr, &image.mipArrayViews[i]) != VK_SUCCESS)
                {
                    NC_LOG_CRITICAL("Failed to create color image view!");
                }

                std::string mipName = image.desc.debugName + " mip array " + std::to_string(i);
                DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)image.mipArrayViews[i], VK_OBJECT_TYPE_IMAGE_VIEW, mipName.c_str());
            }

            DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)image.colorView, VK_OBJECT_TYPE_IMAGE_VIEW, image.desc.debugName.c_str());

            if (!image.isSwapchain)
            {
                VkSampler imguiSampler = _samplerHandler->GetSampler(_imguiSampler);

                static std::mutex imguiAddTextureLock;
                {
                    std::scoped_lock lock(imguiAddTextureLock);
                    image.imguiTextureHandle = ImGui_ImplVulkan_AddTexture(imguiSampler, image.colorView, VK_IMAGE_LAYOUT_GENERAL);
                }
            }
        }
    }
}