#pragma once
#include <Base/Types.h>

#include "Renderer/Descriptors/TimeQueryDesc.h"

#include <vulkan/vulkan_core.h>

template<typename T>
class SafeVector;

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class BufferHandlerVK;
        class UploadBufferHandlerVK;
        class SamplerHandlerVK;
        struct Texture;

        struct ITimeQueryHandlerVKData {};

        class TimeQueryHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device);

            void FlipFrame(u32 frameIndex);

            TimeQueryID CreateTimeQuery(const TimeQueryDesc& desc);

            void Begin(VkCommandBuffer buffer, TimeQueryID id);
            void End(VkCommandBuffer buffer, TimeQueryID id);

            const std::string& GetName(TimeQueryID id);
            f32 GetLastTime(TimeQueryID id);

        private:


        private:
            ITimeQueryHandlerVKData* _data;

            RenderDeviceVK* _device;

            static PFN_vkResetQueryPoolEXT fnVkResetQueryPoolEXT;
        };
    }
}