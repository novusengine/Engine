#pragma once
#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct Draw
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            u32 vertexCount = 0;
            u32 instanceCount = 0;
            u32 vertexOffset = 0;
            u32 instanceOffset = 0;
        };
    }
}