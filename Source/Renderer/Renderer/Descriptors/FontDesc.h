#pragma once
#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    struct FontDesc
    {
        std::string path = "";
        f32 size = 0.0f;
        i32 padding = 5;
    };
}