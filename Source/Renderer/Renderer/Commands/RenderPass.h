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
    }
}