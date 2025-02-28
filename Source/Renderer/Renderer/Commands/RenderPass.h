#pragma once
#include "Renderer/Descriptors/RenderPassDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct BeginRenderPass
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            RenderPassDesc desc;
        };

        struct EndRenderPass
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            RenderPassDesc desc;
        };

        struct BeginTextureRenderPass
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;
            TextureRenderPassDesc desc;
        };

        struct EndTextureRenderPass
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;
            TextureRenderPassDesc desc;
        };

        struct BeginTextureComputeWritePass
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;
            TextureRenderPassDesc desc;
        };

        struct EndTextureComputeWritePass
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;
            TextureRenderPassDesc desc;
        };
    }
}