#pragma once
#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    // Lets strong-typedef an ID type with the underlying type of u8
    STRONG_TYPEDEF(CommandListID, u8);
}