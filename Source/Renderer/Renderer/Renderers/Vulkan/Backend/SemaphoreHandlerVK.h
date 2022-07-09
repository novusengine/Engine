#pragma once
#include "Renderer/Descriptors/SemaphoreDesc.h"

#include <Base/Types.h>

#include <vulkan/vulkan_core.h>

namespace tracy
{
    class VkCtxManualScope;
}

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;

        struct ISemaphoreHandlerVKData {};

        class SemaphoreHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device);

            SemaphoreID CreateNSemaphore();

            VkSemaphore GetVkSemaphore(SemaphoreID id);

        private:
            

        private:

        private:
            RenderDeviceVK* _device;

            ISemaphoreHandlerVKData* _data;
        };
    }
}