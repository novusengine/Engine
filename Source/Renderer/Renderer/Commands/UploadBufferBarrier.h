#pragma once
#include "Renderer/BackendDispatch.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct UploadBufferBarrier
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;
        };
    }
}