#pragma once
#include "FileFormat/Shared.h"

#include <Base/Types.h>
#include <Base/Platform.h>

#include <robinhood/robinhood.h>

namespace Wmo
{
    struct WMOGroupFlags
    {
        u32 HasBSPTree : 1;
        u32 : 1;
        u32 HasVertexColors : 1;
        u32 IsExterior : 1;
        u32 : 2;
        u32 IsExteriorLit : 1;
        u32 IsUnreachable : 1;
        u32 ForceSkyboxInInterior : 1;
        u32 HasLights : 1;
        u32 HasLOD : 1;
        u32 HasDoodads : 1;
        u32 HasLiquid : 1;
        u32 IsInterior : 1;
        u32 : 1;
        u32 IsMountQueryAllowed : 1;
        u32 NoPortalCulling : 1;
        u32 : 1;
        u32 ShowSkybox : 1;
        u32 LiquidIsOcean : 1;
        u32 : 1;
        u32 IsMountAllowed : 1;
        u32 : 2;
        u32 HasSecondMOCV : 1;
        u32 HasSecondMOTV : 1;
        u32 ForceCreateOccluders : 1;
        u32 : 1;
        u32 IsExteriorCulled : 1;
        u32 HasThreeMOTV : 1;
        u32 : 1;
    };

    struct MOCV
    {
    public:
        std::vector<IntColor> data;
    };

    struct MOBR
    {
    public:
        std::vector<u16> data;
    };

    struct MOBN
    {
    public:
        struct BspNode
        {
        public:
            u16 flags;
            i16 negChild;
            i16 posChild;
            u16 nFaces;
            u32 faceStart;
            f32 planeDist;
        };

        std::vector<BspNode> data;
    };

    struct MOBA
    {
    public:
        struct RenderBatch
        {
            struct Flags
            {
                u8 : 1;
                u8 UseMaterialIDLarge : 1;
            };

            u8 unused[0xA];
            u16 materialIDLarge = 0;

            u32 startIndex = 0;
            u16 indexCount = 0;
            u16 vertexStartOffset = 0;
            u16 vertexEndOffset = 0;

            Flags flags;
            u8 materialIDSmall = 0;
        };

        std::vector<RenderBatch> data;
    };

    struct MOTV
    {
    public:
        struct VertexTextureUV
        {
            vec2 uv = { };
        };

        std::vector<VertexTextureUV> data;
    };

    struct MONR
    {
    public:
        struct Normal
        {
            vec3 normal = { };
        };

        std::vector<Normal> data;
    };

    struct MOVT
    {
    public:
        struct Vertex
        {
        public:
            vec3 position = { };
        };

        std::vector<Vertex> data;
    };

    struct MOVI
    {
    public:
        std::vector<u16> data;
    };

    struct MOPY
    {
    public:
        struct TriangleMaterialInfo
        {
            struct Flags
            {
                u8 Unk0x1 : 1;
                u8 NoCameraCollision : 1;
                u8 Detail : 1;
                u8 Collision : 1;
                u8 Hint : 1;
                u8 Render : 1;
                u8 Unk0x40 : 1;
                u8 CollideHit : 1;
            };

            Flags flags = { };
            u8 materialID = 0;
        };

        std::vector<TriangleMaterialInfo> data;
    };

    struct MOGP
    {
        struct Flags2
        {
            u32 CanCutTerrain : 1;
        };

        u32 groupNameIndex = 0;
        u32 groupDescriptionIndex = 0;
        WMOGroupFlags flags = { };

        vec3 aabbMin = { };
        vec3 aabbMax = { };

        u16 portalReferenceIndex = 0;
        u16 portalReferenceCount = 0;

        u16 transitionBatchCount = 0;
        u16 interiorBatchCount = 0;
        u16 exteriorBatchCount = 0;
        u16 unused1 = 0;

        u8 fogIndices[4] = { 0 };
        u32 groupLiquid = 0; // TODO : Figure out exactly what this does

        u32 wmoAreaTableGroupID = 0;
        Flags2 flags2 = { };
        u32 unused2 = 0;
    };

    struct GFID
    {
    public:
        struct GroupFileID
        {
            u32 fileID;
        };

        std::vector<GroupFileID> data;
    };

    struct MODD
    {
    public:
        struct PlacementInfo
        {
            struct Flags
            {
                u32 MODIIndex : 24;
                u32 CanProjectTextures : 1;
                u32 ForceInteriorLighting : 1;
                u32 Unk0x4 : 1;
                u32 Unk0x8 : 1;
            };

            Flags flags = { };

            vec3 position = { };
            quat rotation = { };
            f32 scale = 0.0f;
            IntColor color = { };
        };

        std::vector<PlacementInfo> data;
    };

    struct MODI
    {
    public:
        struct DoodadFileID
        {
            u32 fileID;
        };

        std::vector<DoodadFileID> data;
    };

    struct MODS
    {
    public:
        struct DoodadSet
        {
            char name[20] = { 0 };
            u32 startIndex = 0;
            u32 count = 0;
            u8 padding[4] = { };
        };

        std::vector<DoodadSet> data;
    };

    struct MOLT
    {
    public:
        struct Light
        {
            enum class Type : i8
            {
                None = -1,
                Pointlight,
                Spotlight,
                Directional,
                Ambient
            };

            Type type = Type::None;
            u8 useAttenuation = 0;
            u8 padding[2] = { 0 };

            IntColor color = { };
            vec3 position = { };
            f32 intensity = 0.0f;

            f32 unk[4] = { 0 };

            vec2 attenuationRange = { };
        };

        std::vector<Light> data;
    };

    struct MOPR
    {
    public:
        struct PortalReference
        {
            u16 portalIndex = 0;
            u16 groupIndex = 0;

            i16 side = 0;
            u16 padding = 0;
        };

        std::vector<PortalReference> data;
    };

    struct MOPT
    {
    public:
        struct PortalInfo
        {
            u16 vertexIndex = 0;
            u16 vertexCount = 0;

            vec3 normal = { };
            f32 distance = 0.0f;
        };

        std::vector<PortalInfo> data;
    };

    struct MOPV
    {
    public:
        struct PortalVertex
        {
            vec3 position;
        };

        std::vector<PortalVertex> data;
    };

    struct MOSI
    {
    public:
        u32 skyboxFileID = 0;
    };

    struct MOGI
    {
    public:
        struct GroupInfo
        {
            WMOGroupFlags flags = { };

            vec3 aabbMin = { };
            vec3 aabbMax = { };

            i32 groupNameIndex = -1;
        };

        std::vector<GroupInfo> data;
    };

    struct MOGN
    {
    public:
        u32 numBytes = 0;
        u8* groupNames = nullptr;
    };

    struct MOMT
    {
    public:
        struct Material
        {
            struct Flags
            {
                u32 NoLighting : 1;
                u32 NoFog : 1;
                u32 TwoSided : 1;
                u32 UseExteriorLighting : 1;
                u32 EmissiveAtNight : 1;
                u32 Window : 1; // Unsure what this does
                u32 ClampTextureS : 1;
                u32 ClampTextureT : 1;
                u32 Unk0x100 : 1; // Unsure what this does
            };

            Flags flags = { };
            u32 shaderIndex = 0;
            u32 blendMode = 0;
            u32 texture1FileID = 0;

            IntColor emissiveColorAtNight = { };
            IntColor unused1 = { }; // This is set at runtime in the original client

            u32 texture2FileID = 0;
            IntColor diffuse1Color = { };

            u32 terrainGroundType = 0;

            u32 texture3FileID = 0;
            IntColor diffuse2Color = { };

            uint32_t unused2[5] = { 0 }; // This is set at runtime in the original client
        };

        std::vector<Material> data;
    };

    struct MOHD
    {
    public:
        struct Flags
        {
            u16 DoNotAttuenuateVertices : 1;
            u16 UseUnifiedRenderPath : 1;
            u16 UseLiquidTypeFromDBC : 1;
            u16 DoNotFixVertexColorAlpha : 1;
            u16 UseLOD : 1;
            u16 DefaultMaxLODCount : 1;
        };

        u32 materialCount = 0;
        u32 groupCount = 0;
        u32 portalCount = 0;
        u32 lightCount = 0;
        u32 doodadNameCount = 0;
        u32 doodadDefCount = 0;
        u32 doodadSetCount = 0;

        IntColor ambientColor = { };
        u32 wmoAreaTableID = 0;

        vec3 aabbMin = { };
        vec3 aabbMax = { };

        Flags flags = { };
        u16 lodCount = 0;
    };

    struct WMOGroup
    {
    public:
        MVER mver = { };
        MOGP mogp = { };
        MOPY mopy = { };
        MOVI movi = { };
        MOVT movt = { };
        MONR monr = { };
        std::vector<MOTV> motvs = { };
        MOBA moba = { };
        MOBN mobn = { };
        MOBR mobr = { };
        std::vector<MOCV> mocvs = { };
    };

    struct Layout
    {
    public:
        MVER mver = { };
        MOHD mohd = { };
        MOMT momt = { };
        MOGN mogn = { };
        MOGI mogi = { };
        MOSI mosi = { };
        MOPV mopv = { };
        MOPT mopt = { };
        MOPR mopr = { };
        MOLT molt = { };
        MODS mods = { };
        MODI modi = { };
        MODD modd = { };
        GFID gfid = { };

        std::vector<WMOGroup> groups = { };
    };
}
