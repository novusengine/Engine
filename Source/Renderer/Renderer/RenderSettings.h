#pragma once
#include "RenderStates.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Settings
    {
        constexpr u32 SCREEN_WIDTH = 1920;
        constexpr u32 SCREEN_HEIGHT = 1080;
        constexpr size_t STAGING_BUFFER_SIZE = 32 * 1024 * 1024; // 32 MB

        constexpr u32 MAX_SHADOW_CASCADES = 8;
        constexpr u32 MAX_VIEWS = 1 + MAX_SHADOW_CASCADES; // Main view + shadow cascades

        const FrontFaceState FRONT_FACE_STATE = FrontFaceState::COUNTERCLOCKWISE;
    }
}
