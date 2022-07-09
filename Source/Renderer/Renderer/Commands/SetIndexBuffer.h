#pragma once
#include "Renderer/Descriptors/BufferDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct SetIndexBuffer
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            BufferID bufferID = BufferID::Invalid();
            IndexFormat indexFormat = IndexFormat::UInt32;
        };
    }
}