#pragma once
#include "Renderer/RenderStates.h"

#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    typedef Sampler SamplerDesc;

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(SamplerID, u16);
}