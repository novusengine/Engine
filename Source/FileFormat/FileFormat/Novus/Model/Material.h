#pragma once
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>

#include <memory>
#include <span>
#include <type_traits>
#include <vector>

namespace FileFormat::Material
{
    inline constexpr char MATERIAL_FILE_EXTENSION[] = ".material";
    inline constexpr char MATERIAL_INSTANCE_FILE_EXTENSION[] = ".materialinstance";
    inline constexpr char MATERIAL_ANIMATION_FILE_EXTENSION[] = ".materialanimation";

    // Keep this value fixed during coordinated pre-main development. The first
    // accepted main-branch format establishes the initial shipping version.
    inline constexpr u32 DEVELOPMENT_VERSION = 0;
    inline constexpr FileHeader::Type MATERIAL_FILE_TYPE = FileHeader::Type::Material;
    inline constexpr FileHeader::Type MATERIAL_INSTANCE_FILE_TYPE = FileHeader::Type::MaterialInstance;
    inline constexpr FileHeader::Type MATERIAL_ANIMATION_FILE_TYPE = FileHeader::Type::MaterialAnimation;

    // Every non-empty root section begins on a 16-byte boundary. Root offsets are
    // byte offsets from the start of the file, and arrays contain their final
    // runtime representation so Game can bulk-copy them after Read validates the
    // externally known file size. PACT remains responsible for archive/disk I/O.

    enum class RasterClass : u8
    {
        Solid,
        AlphaTest,
        Transparent
    };

    enum MaterialFlags : u32
    {
        MaterialFlags_None = 0,
        MaterialFlags_TwoSided = 1u << 0,
        MaterialFlags_CastsShadows = 1u << 1,
        MaterialFlags_ReceivesDecals = 1u << 2,
        MaterialFlags_ReceivesFog = 1u << 3,
        MaterialFlags_HasCoverageFunction = 1u << 4
    };

    enum class ParameterType : u8
    {
        Float,
        Float2,
        Float3,
        Float4,
        UInt,
        UInt2,
        UInt3,
        UInt4,
        Texture2D,
        TextureCube,
        Sampler
    };

    struct ParameterDefinition
    {
        u64 nameHash = 0;
        u32 byteOffset = 0;
        u16 byteSize = 0;
        ParameterType type = ParameterType::Float;
        u8 arrayCount = 1;
    };

    struct MaterialData
    {
        std::vector<ParameterDefinition> parameters;
        std::vector<u8> defaultParameterData;
    };

    // Hashes the serialized parameter layout shared by a Material and all of its
    // flattened MaterialInstances. Keep the algorithm here so producers and
    // consumers cannot independently define the compatibility check.
    u64 CalculateParameterLayoutHash(std::span<const ParameterDefinition> parameters, u32 parameterBlockSize);

    // One authored Material program and its immutable routing metadata. The
    // default parameter block is already laid out exactly as its generated Slang
    // loader expects.
    struct MaterialAsset
    {
        FileHeader header = FileHeader(MATERIAL_FILE_TYPE, DEVELOPMENT_VERSION);

        u32 programID = 0;
        u16 lightingModelID = 0;
        u16 materialExecutionGroupID = 0;

        RasterClass rasterClass = RasterClass::Solid;
        u8 reserved0[3] = {0, 0, 0};
        u32 flags = MaterialFlags_None;

        u32 parameterBlockSize = 0;
        u32 parameterBlockAlignment = 16;
        u32 parametersOffset = 0;
        u32 numParameters = 0;
        u32 defaultParameterDataOffset = 0;
        u32 defaultParameterDataSize = 0;

        size_t GetSerializedSize(const MaterialData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const MaterialData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, MaterialAsset& out);
    };

    enum class ResourceType : u8
    {
        Texture2D,
        TextureCube,
        Sampler
    };

    enum ResourceBindingFlags : u8
    {
        ResourceBindingFlags_None = 0,
        ResourceBindingFlags_Optional = 1u << 0
    };

    // AssetConverter emits a complete flat parameter block. Game memcpy-copies it
    // and patches only these sparse resource words after resolving assets into the
    // runtime bindless heap. parameterByteOffset points at one u32 in the block.
    struct ResourceBinding
    {
        AssetID resourceAssetID = INVALID_ASSET_ID;
        u32 parameterByteOffset = 0;
        u16 samplerID = 0;
        ResourceType type = ResourceType::Texture2D;
        u8 flags = ResourceBindingFlags_None;
    };

    enum class AnimationTimeSource : u8
    {
        SharedClock,
        StableInstanceClock,
        AnimationController
    };

    enum MaterialAnimationBindingFlags : u8
    {
        MaterialAnimationBindingFlags_None = 0,
        MaterialAnimationBindingFlags_Looping = 1u << 0
    };

    struct MaterialAnimationBinding
    {
        AssetID materialAnimationAssetID = INVALID_ASSET_ID;
        u32 parameterByteOffset = 0;
        u16 trackIndex = 0;
        AnimationTimeSource timeSource = AnimationTimeSource::SharedClock;
        u8 flags = MaterialAnimationBindingFlags_None;
    };

    struct MaterialInstanceData
    {
        std::vector<u8> parameterData;
        std::vector<ResourceBinding> resourceBindings;
        std::vector<MaterialAnimationBinding> animationBindings;
    };

    // A MaterialInstance is a complete flattened parameter block, not a runtime
    // parent/delta chain. parameterLayoutHash must match the referenced Material.
    struct MaterialInstanceAsset
    {
        FileHeader header = FileHeader(MATERIAL_INSTANCE_FILE_TYPE, DEVELOPMENT_VERSION);
        AssetID materialAssetID = INVALID_ASSET_ID;
        u64 parameterLayoutHash = 0;
        u32 parameterDataOffset = 0;
        u32 parameterDataSize = 0;
        u32 resourceBindingsOffset = 0;
        u32 numResourceBindings = 0;
        u32 animationBindingsOffset = 0;
        u32 numAnimationBindings = 0;

        size_t GetSerializedSize(const MaterialInstanceData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const MaterialInstanceData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, MaterialInstanceAsset& out);
    };

    enum class MaterialAnimationMode : u8
    {
        Constant,
        LinearRate,
        UniformSamples
    };

    enum MaterialAnimationTrackFlags : u8
    {
        MaterialAnimationTrackFlags_None = 0,
        MaterialAnimationTrackFlags_Looping = 1u << 0
    };

    // Samples are vec4 to keep the first GPU evaluator branch-light. The
    // component count determines which components are meaningful. Constant and
    // LinearRate tracks consume baseValue/ratePerSecond and have no sample range.
    struct MaterialAnimationTrack
    {
        u64 trackNameHash = 0;
        u32 sampleOffset = 0;
        u32 numSamples = 0;
        u16 sampleRateHz = 0;
        u8 componentCount = 1;
        MaterialAnimationMode mode = MaterialAnimationMode::Constant;
        u8 flags = MaterialAnimationTrackFlags_None;
        u8 reserved0[3] = {0, 0, 0};
        vec4 baseValue = {};
        vec4 ratePerSecond = {};
    };

    struct MaterialAnimationData
    {
        std::vector<MaterialAnimationTrack> tracks;
        std::vector<vec4> samples;
    };

    struct MaterialAnimationAsset
    {
        FileHeader header = FileHeader(MATERIAL_ANIMATION_FILE_TYPE, DEVELOPMENT_VERSION);
        f32 durationSeconds = 0.0f;
        u32 reserved = 0;
        u32 tracksOffset = 0;
        u32 numTracks = 0;
        u32 samplesOffset = 0;
        u32 numSamples = 0;

        size_t GetSerializedSize(const MaterialAnimationData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const MaterialAnimationData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, MaterialAnimationAsset& out);
    };

    static_assert(sizeof(ParameterDefinition) == 16);
    static_assert(sizeof(vec4) == 16);
    static_assert(sizeof(MaterialAsset) == 48);
    static_assert(sizeof(ResourceBinding) == 16);
    static_assert(sizeof(MaterialAnimationBinding) == 16);
    static_assert(sizeof(MaterialInstanceAsset) == 48);
    static_assert(sizeof(MaterialAnimationTrack) == 56);
    static_assert(sizeof(MaterialAnimationAsset) == 32);

    static_assert(std::is_trivially_copyable_v<MaterialAsset>);
    static_assert(std::is_standard_layout_v<MaterialAsset>);
    static_assert(std::is_trivially_copyable_v<MaterialInstanceAsset>);
    static_assert(std::is_standard_layout_v<MaterialInstanceAsset>);
    static_assert(std::is_trivially_copyable_v<MaterialAnimationAsset>);
    static_assert(std::is_standard_layout_v<MaterialAnimationAsset>);
} // namespace FileFormat::Material
