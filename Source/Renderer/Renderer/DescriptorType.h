#pragma once
#include <Base/Types.h>

namespace Renderer
{
    enum class DescriptorType : u8
    {
        UniformBuffer,
        StorageBuffer,

        SampledImage,
        StorageImage, // Writeable
    };
}