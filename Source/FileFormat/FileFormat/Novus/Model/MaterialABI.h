#pragma once

#include <Base/Types.h>

namespace FileFormat::Material::ABI
{
    // Defines the shared CPU/GPU Material contract used by offline cooks and runtime loading.
    // Versioned constants let Game reject a MaterialPack built for an incompatible layout.
    inline constexpr u32 VERSION = 1;
    inline constexpr u32 PARAMETER_ALIGNMENT = 16;

    enum class Parameter : u16
    {
        BaseColorFactor,
        EmissiveFactor,
        EmissiveIntensity,
        MetallicFactor,
        RoughnessFactor,
        NormalScale,
        OcclusionStrength,
        Opacity,
        AlphaCutoff,
        Count
    };

    namespace ParameterLayout
    {
        inline constexpr u32 BASE_COLOR_FACTOR_OFFSET = 0;
        inline constexpr u32 EMISSIVE_FACTOR_OFFSET = 16;
        inline constexpr u32 EMISSIVE_INTENSITY_OFFSET = 28;
        inline constexpr u32 METALLIC_FACTOR_OFFSET = 32;
        inline constexpr u32 ROUGHNESS_FACTOR_OFFSET = 36;
        inline constexpr u32 NORMAL_SCALE_OFFSET = 40;
        inline constexpr u32 OCCLUSION_STRENGTH_OFFSET = 44;
        inline constexpr u32 OPACITY_OFFSET = 48;
        inline constexpr u32 ALPHA_CUTOFF_OFFSET = 52;
        inline constexpr u32 BLOCK_SIZE = 64;
    }

    enum class LightingModel : u16
    {
        Standard = 0,
        Unlit = 1,
        Count
    };

    enum class ExecutionGroup : u16
    {
        OpaqueSimple = 0,
        OpaqueLayered = 1,
        AlphaTestSimple = 2,
        AlphaTestLayered = 3,
        TransparentSimple = 4,
        TransparentLayered = 5,
        Count
    };

    inline constexpr u32 EXECUTION_GROUP_COUNT = static_cast<u32>(ExecutionGroup::Count);

    namespace LegacyModel
    {
        inline constexpr u32 MAX_TEXTURES = 8;
        inline constexpr u32 MAX_UNITS = 8;
    }
}
