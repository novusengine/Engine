#pragma once
#include <Base/Types.h>

#include <FileFormat/Novus/ShaderPack/ShaderPack.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    struct DescriptorSetDesc
    {
        FileFormat::DescriptorSetReflection* reflection;
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(DescriptorSetID, u16);
}