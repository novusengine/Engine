#pragma once
#include "vk_mem_alloc.h"
#include "Renderer/Descriptors/ImageDesc.h"
#include "Renderer/Descriptors/DepthImageDesc.h"
#include "Renderer/Descriptors/TimeQueryDesc.h"

#include <Base/Types.h>

#include <vulkan/vulkan_core.h>

#include <vector>
#include <optional>

namespace Novus
{
    class Window;
}
struct GLFWwindow;

namespace tracy
{
    class VkCtx;
}

namespace Renderer
{
    class RendererVK;

    namespace Backend
    {
        struct SwapChainVK;
        class ImageHandlerVK;
        class SemaphoreHandlerVK;
        struct DescriptorMegaPoolVK;

        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            bool graphicsFamilySupportsTimeStamps;

            std::optional<uint32_t> transferFamily;
            bool transferFamilySupportsTimeStamps;

            std::optional<uint32_t> presentFamily;
            bool presentFamilySupportsTimeStamps;

            bool IsComplete()
            {
                return graphicsFamily.has_value() && transferFamily.has_value() && presentFamily.has_value();
            }
        };

        struct ImguiContext
        {
            VkRenderPass imguiPass;
            VkDescriptorPool imguiPool;
        };

        class RenderDeviceVK
        {
        public:
            RenderDeviceVK(Novus::Window* window);
            ~RenderDeviceVK();

            void Init();
            void InitWindow(ImageHandlerVK* imageHandler, SemaphoreHandlerVK* semaphoreHandler, Novus::Window* window);

            u32 GetFrameIndex() { return _frameIndex; }
            void EndFrame() { _frameIndex = (_frameIndex + 1) % FRAME_INDEX_COUNT; }

            void FlushGPU();

            const std::string& GetGPUName() { return _gpuName; }

        private:
            void InitOnce();

            // InitOnce helper functions
            void InitVulkan();
            void SetupDebugMessenger();
            void PickPhysicalDevice();
            void CreateLogicalDevice();
            void CreateAllocator();
            void CreateCommandPool();
            void CreateTracyContext();
            void InitializeImguiVulkan();

            // InitWindow helper functions
            void CreateSurface(GLFWwindow* window, SwapChainVK* swapChain);
            void CreateSwapChain(const ivec2& windowSize, SwapChainVK* swapChain, VkFormat& format);
            void CreateSemaphores(SwapChainVK* swapChain, SemaphoreHandlerVK* semaphoreHandler);
            void CreateImages(SwapChainVK* swapChain, ImageHandlerVK* imageHandler, VkFormat format);

            void CleanupSwapChain(SwapChainVK* swapChain);
            void RecreateSwapChain(ImageHandlerVK* imageHandler, SemaphoreHandlerVK* semaphoreHandler, SwapChainVK* swapChain);

            int RateDeviceSuitability(VkPhysicalDevice device);
            QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
            bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
            bool CheckDeviceFeatureSupport(VkPhysicalDevice device, VkPhysicalDeviceFeatures2& requestedFeatures);

            void CheckValidationLayerSupport();
            std::vector<const char*> GetRequiredExtensions();

            VkCommandBuffer BeginSingleTimeCommands();
            void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

            void CopyBuffer(VkBuffer dstBuffer, u64 dstOffset, VkBuffer srcBuffer, u64 srcOffset, u64 range);
            void CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkFormat format, u32 width, u32 height, u32 numLayers, u32 numMipLevels);
            void CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, size_t srcOffset, VkImage dstImage, VkFormat format, u32 width, u32 height, u32 numLayers, u32 numMipLevels);
            void TransitionImageLayout(VkImage image, VkImageAspectFlags aspects, VkImageLayout oldLayout, VkImageLayout newLayout, u32 numLayers, u32 numMipLevels);
            void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspects, VkImageLayout oldLayout, VkImageLayout newLayout, u32 numLayers, u32 numMipLevels);

            uvec2 GetMainWindowSize() { return _mainWindowSize; }
            vec2 GetRenderSize() { return _renderSize; }
            void SetRenderSize(vec2 renderSize) { _renderSize = renderSize; }

            ImguiContext* GetImguiContext() { return _imguiContext; }

            static PFN_vkCmdDrawIndirectCountKHR fnVkCmdDrawIndirectCountKHR;
            static PFN_vkCmdDrawIndexedIndirectCountKHR fnVkCmdDrawIndexedIndirectCountKHR;

            bool HasExtendedTextureSupport() { return _hasExtendedTextureSupport; }
        private:
            Novus::Window* _window;
            uvec2 _mainWindowSize;
            vec2 _renderSize = vec2(1, 1);

            static const u32 FRAME_INDEX_COUNT = 2;
            static bool _initialized;
            u32 _frameIndex;

            VkInstance _instance;
            VkDebugUtilsMessengerEXT _debugMessenger;
            
            std::string _gpuName;
            VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
            VkDevice _device = VK_NULL_HANDLE;
            VkCommandPool _graphicsCommandPool = VK_NULL_HANDLE;
            VkCommandPool _transferCommandPool = VK_NULL_HANDLE;

            VkQueue _graphicsQueue = VK_NULL_HANDLE;
            VkQueue _transferQueue = VK_NULL_HANDLE;
            VkQueue _presentQueue = VK_NULL_HANDLE;

            bool _graphicsQueueSupportsTimestamps = false;
            f32 _timestampNanosecondsPerIncrement = 0.0f;
            
            bool _hasExtendedTextureSupport = false;

            std::vector<SwapChainVK*> _swapChains;

            VmaAllocator _allocator;

            DescriptorMegaPoolVK* _descriptorMegaPool;

            tracy::VkCtx* _tracyContext = nullptr;
            ImguiContext* _imguiContext = nullptr;
            friend class ::Renderer::RendererVK;
            friend class BufferHandlerVK;
            friend class ImageHandlerVK;
            friend class TextureHandlerVK;
            friend class ModelHandlerVK;
            friend class ShaderHandlerVK;
            friend class PipelineHandlerVK;
            friend class CommandListHandlerVK;
            friend class SamplerHandlerVK;
            friend class SemaphoreHandlerVK;
            friend class TimeQueryHandlerVK;
            friend class UploadBufferHandlerVK;
            friend struct DescriptorAllocatorHandleVK;
            friend class DescriptorAllocatorPoolVKImpl;
            friend class DescriptorSetBuilderVK;
        };
    }
}
