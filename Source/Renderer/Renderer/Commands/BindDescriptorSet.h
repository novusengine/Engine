#pragma once
#include "Renderer/DescriptorSet.h"

#include <Base/Types.h>

namespace Renderer
{
    class TrackedBufferBitSets;

    namespace Commands
    {
        struct BindDescriptorSet
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            DescriptorSet* set;

            const TrackedBufferBitSets* bufferPermissions;
        };
    }
}