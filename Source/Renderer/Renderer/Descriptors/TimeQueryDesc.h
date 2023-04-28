#pragma once
#include "Renderer/RenderStates.h"

#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    struct TimeQueryDesc
    {
        std::string name;
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(TimeQueryID, u16);
}