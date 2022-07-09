#pragma once
#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct MarkFrameStart
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            u32 frameIndex;
        };
    }
}