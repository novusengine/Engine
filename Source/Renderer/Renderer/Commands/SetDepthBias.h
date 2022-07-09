#pragma once
#include "Renderer/RenderStates.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct SetDepthBias
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            DepthBias depthBias;
        };
    }
}