#pragma once
#include "Renderer/Descriptors/BufferDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct BufferBarrier
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            const BufferBarrierDesc* barriers = nullptr;
            u32 count = 0;
        };
    }
}
