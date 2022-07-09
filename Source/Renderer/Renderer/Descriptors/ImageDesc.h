#pragma once
#include "Renderer/RenderStates.h"

#include <Base/Types.h>
#include <Base/Math/Color.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    struct ImageDesc
    {
        std::string debugName = "";

        vec2 dimensions = vec2(0, 0);
        ImageDimensionType dimensionType = ImageDimensionType::DIMENSION_ABSOLUTE;

        u32 depth = 1;
        u32 mipLevels = 1;
        ImageFormat format = ImageFormat::UNKNOWN;
        SampleCount sampleCount = SampleCount::SAMPLE_COUNT_1;
        Color clearColor = Color::Clear;
        uvec4 clearUInts = uvec4(0, 0, 0, 0);
        ivec4 clearInts = ivec4(0, 0, 0, 0);
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(ImageID, u16);
}