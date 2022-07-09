#pragma once
#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    struct FontDesc
    {
        std::string path = "";
        float size = 0.0f;
        int padding = 3;
    };
}