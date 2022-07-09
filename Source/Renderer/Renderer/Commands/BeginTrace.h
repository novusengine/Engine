#pragma once
#include "Renderer/BackendDispatchFn.h"

#include <Base/Types.h>

namespace tracy
{
    struct SourceLocationData;
}

namespace Renderer
{
    namespace Commands
    {
        struct BeginTrace
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            const tracy::SourceLocationData* sourceLocation;
        };
    }
}