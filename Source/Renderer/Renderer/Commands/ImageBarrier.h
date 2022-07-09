#pragma once
#include "Renderer/BackendDispatch.h"
#include "Renderer/Descriptors/ImageDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct ImageBarrier
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            ImageID image = ImageID::Invalid();
        };
    }
}