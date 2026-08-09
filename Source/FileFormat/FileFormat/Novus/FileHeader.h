#pragma once
#include <Base/Types.h>
#include <Base/Platform.h>

#include <limits>

struct FileHeader
{
public:
    enum class Type : i32
    {
        Invalid = -1,
        ClientDB,
        MapHeader,
        MapChunk,
        MapObject,
        MapObjectGroup,
        ComplexModel,
        ShaderPack,
        NavMeshTerrainHeight,

        // Shared AssetConverter/Game contract. Until these formats first reach
        // main, both consumers must use the same Engine commit and changes to the
        // serialized structs require discussion and coordination. Do not add
        // migration code or bump development versions for coordinated edits.
        Model,
        Material,
        MaterialInstance,
        MaterialAnimation,
        RigFamily,
        Skeleton,
        AnimationClip,
        AnimationSet,
        AnimationGraph,
        BoneMask,
        IKRig,
        RetargetProfile,
        AnimationBounds,
        MaterialPack
    };

public:
    FileHeader() { }
    FileHeader(Type inType, u32 inVersion)
    {
        type = inType;
        version = inVersion;
    }

    bool operator==(const FileHeader& other) const
    {
        return type == other.type && version == other.version;
    }

    bool operator!=(const FileHeader& other) const
    {
        return type != other.type || version != other.version;
    }

public:
    Type type = Type::Invalid;
    u32 version = 0;
};

namespace FileFormat
{
    using AssetID = u64;
    inline constexpr AssetID INVALID_ASSET_ID = std::numeric_limits<AssetID>::max();
}
