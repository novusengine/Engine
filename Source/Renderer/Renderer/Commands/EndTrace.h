#pragma once
#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct EndTrace
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;
        };
    }
}