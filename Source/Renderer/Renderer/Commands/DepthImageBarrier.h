#pragma once
#include "Renderer/BackendDispatch.h"
#include "Renderer/Descriptors/DepthImageDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct DepthImageBarrier
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            DepthImageID image = DepthImageID::Invalid();
        };
    }
}