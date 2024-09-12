#pragma once
#include "Renderer/SwapChain.h"
#include "Renderer/FrameResource.h"
#include "Renderer/Descriptors/GraphicsPipelineDesc.h"
#include "Renderer/Descriptors/SemaphoreDesc.h"

#include <Base/Types.h>
#include <Base/Math/Math.h>

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class DescriptorSetBuilderVK;

        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        struct SwapChainVK : public SwapChain
        {
            SwapChainVK(RenderDeviceVK* inDevice) 
            : device(inDevice)
            {
            
            };
            ~SwapChainVK()
            {

            }

            SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device)
            {
                SwapChainSupportDetails details;
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

                uint32_t formatCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

                if (formatCount != 0)
                {
                    details.formats.resize(formatCount);
                    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
                }

                uint32_t presentModeCount;
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

                if (presentModeCount != 0)
                {
                    details.presentModes.resize(presentModeCount);
                    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
                }

                return details;
            }

            VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
            {
                for (const auto& availableFormat : availableFormats)
                {
                    if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    {
                        return availableFormat;
                    }
                }

                return availableFormats[0];
            }

            VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
            {
                // First check for immediate mode (not vsync)
                for (const auto& availablePresentMode : availablePresentModes)
                {
                    if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                    {
                        return availablePresentMode;
                    }
                }

                // Then check for mailbox (vsync)
                for (const auto& availablePresentMode : availablePresentModes)
                {
                    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                    {
                        return availablePresentMode;
                    }
                }

                // Then use the fallback FIFO mode which kinda sucks
                return VK_PRESENT_MODE_FIFO_KHR;
            }

            VkExtent2D ChooseSwapExtent(const ivec2& windowSize, const VkSurfaceCapabilitiesKHR& capabilities)
            {
                if (capabilities.currentExtent.width != UINT32_MAX)
                {
                    return capabilities.currentExtent;
                }
                else
                {
                    VkExtent2D actualExtent = { (uint32_t)windowSize.x, (uint32_t)windowSize.y };

                    actualExtent.width = Math::Max(capabilities.minImageExtent.width, Math::Min((int)capabilities.maxImageExtent.width, (int)actualExtent.width));
                    actualExtent.height = Math::Max(capabilities.minImageExtent.height, Math::Min((int)capabilities.maxImageExtent.height, (int)actualExtent.height));

                    return actualExtent;
                }
            }

            RenderDeviceVK* device;
            GLFWwindow* window;

            u32 frameIndex = 0;
            u32 bufferCount;

            VkSurfaceKHR surface;
            VkSwapchainKHR swapChain;

            static const u32 FRAME_BUFFER_COUNT = 2;
            FrameResource<ImageID, FRAME_BUFFER_COUNT> imageIDs;
            FrameResource<SemaphoreID, FRAME_BUFFER_COUNT> imageAvailableSemaphores;
            FrameResource<SemaphoreID, FRAME_BUFFER_COUNT> blitFinishedSemaphores;
        };
    }
}