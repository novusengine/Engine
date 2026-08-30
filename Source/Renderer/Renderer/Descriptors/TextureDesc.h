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
        u64 hash = 0;
        const u8* data = nullptr;
        size_t size = 0;
    };

    struct TextureUploadRegion
    {
    public:
        uvec2 offset = uvec2(0);
        uvec2 extent = uvec2(0);
        u32 layer = 0;
        u32 mipLevel = 0;
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(TextureID, u16);
}
