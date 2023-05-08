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

            BufferID bufferID = BufferID::Invalid();
            BufferPassUsage from = BufferPassUsage::NONE;
        };
    }
}