#pragma once
#include "Renderer/RenderStates.h"

#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    struct TextureDesc
    {
        std::string path = "";
    };

    struct DataTextureDesc
    {
        i32 width = 0;
        i32 height = 0;
        i32 layers = 1;

        ImageFormat format;
        
        u8* data = nullptr;
        std::string debugName = "";
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(TextureID, u16);
}