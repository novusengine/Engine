#pragma once
#include "Renderer/Descriptors/BufferDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct PipelineBarrier
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            PipelineBarrierType barrierType;
            BufferID buffer = BufferID::Invalid();
        };
    }
}