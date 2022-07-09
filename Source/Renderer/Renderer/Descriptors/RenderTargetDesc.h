#pragma once
#include "ImageDesc.h"
#include "Renderer/RenderStates.h"

#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    struct RenderTargetDesc
    {
        ImageID image = ImageID::Invalid();
        BlendState blendState;
    };
}