#pragma once
#include "Renderer/Descriptors/TimeQueryDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct BeginTimeQuery
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            TimeQueryID timeQueryID = TimeQueryID::Invalid();
        };

        struct EndTimeQuery
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            TimeQueryID timeQueryID = TimeQueryID::Invalid();
        };
    }
}