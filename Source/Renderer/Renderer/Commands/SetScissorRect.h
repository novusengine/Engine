#pragma once
#include "Renderer/RenderStates.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct SetScissorRect
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            ScissorRect scissorRect;
        };
    }
}