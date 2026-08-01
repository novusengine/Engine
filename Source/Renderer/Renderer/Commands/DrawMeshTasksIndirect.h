#pragma once
#include "Renderer/Descriptors/BufferDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct DrawMeshTasksIndirect
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            BufferID argumentBuffer = BufferID::Invalid();
            u32 argumentBufferOffset = 0;
        };
    }
}
