#pragma once
#include "Renderer/Descriptors/BufferDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct DrawIndirectCount
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            BufferID argumentBuffer = BufferID::Invalid();
            BufferID drawCountBuffer = BufferID::Invalid();
            u32 argumentBufferOffset = 0;
            u32 drawCountBufferOffset = 0;
            u32 maxDrawCount = 0;
        };
    }
}