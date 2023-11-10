#pragma once
#include "RenderStates.h"

#include <Base/Types.h>

namespace Renderer
{
    struct Settings
    {
    public:
        static const u32 SCREEN_WIDTH = 2560;
        static const u32 SCREEN_HEIGHT = 1320;
        static const size_t STAGING_BUFFER_SIZE = 32 * 1024 * 1024; // 32 MB

        static const u32 MAX_SHADOW_CASCADES = 8;
        static const u32 MAX_VIEWS = 1 + MAX_SHADOW_CASCADES; // Main view + shadow cascades

        static const FrontFaceState FRONT_FACE_STATE = FrontFaceState::COUNTERCLOCKWISE;

        static const u32 MAX_TEXTURES_NORMAL = 4096; // Don't increase this or we get issues with certain graphics cards not supporting enough textures
        static const u32 MAX_TEXTURES_EXTENDED = 8192; // Keep this synced with common.inc.hlsl
        static u32 MAX_TEXTURES; // Set in RenderDevice for your API
    };
}
