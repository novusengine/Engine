#pragma once
#include "ComputeShaderDesc.h"

#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    NOVUS_NO_PADDING_START;
    struct ComputePipelineDesc
    {
        ComputeShaderID computeShader = ComputeShaderID::Invalid();
    };
    NOVUS_NO_PADDING_END;

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(ComputePipelineID, u16);
}