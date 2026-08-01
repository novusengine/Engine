#pragma once

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct DrawMeshTasks
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            u32 groupCountX = 0;
            u32 groupCountY = 0;
            u32 groupCountZ = 0;
        };
    }
}
