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

    struct TextureBaseDesc
    {
        i32 width = 0;
        i32 height = 0;
        i32 layers = 1;
        i32 mipLevels = 1;

        ImageFormat format;
        std::string debugName = "";
        bool renderable = false;
    };

    struct DataTextureDesc : TextureBaseDesc
    {
        const u8* data = nullptr;
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(TextureID, u16);
}