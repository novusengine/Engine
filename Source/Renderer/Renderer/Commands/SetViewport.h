#pragma once
#include "Renderer/RenderStates.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct SetViewport
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            Viewport viewport;
        };
    }
}