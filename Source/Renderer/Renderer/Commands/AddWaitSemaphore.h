#pragma once
#include "Renderer/BackendDispatchFn.h"
#include "Renderer/Descriptors/SemaphoreDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct AddWaitSemaphore
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            SemaphoreID semaphore;
        };
    }
}