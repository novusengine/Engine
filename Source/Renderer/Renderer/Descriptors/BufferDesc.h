#pragma once
#include "Renderer/RenderStates.h"

#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    struct BufferDesc
    {
        std::string name = "";
        u8 usage;
        BufferCPUAccess cpuAccess = BufferCPUAccess::None;
        u64 size = 0;
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(BufferID, u16);
}