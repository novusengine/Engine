#pragma once
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>

#include <memory>
#include <type_traits>
#include <vector>

namespace FileFormat::Model
{
    inline constexpr char FILE_EXTENSION[] = ".model";

    // Keep this value fixed during coordinated pre-main development. The first
    // accepted main-branch format establishes the initial shipping version.
    inline constexpr u32 DEVELOPMENT_VERSION = 0;
    inline constexpr FileHeader::Type FILE_TYPE = FileHeader::Type::Model;

    inline constexpr u32 MAX_MESHLET_VERTICES = 64;
    // 124 matches meshoptimizer's recommended mesh-shader cluster size and the
    // Model V2 renderer contract. Keeping this in the shared format prevents the
    // cooker and renderer from silently choosing different workgroup limits.
    inline constexpr u32 MAX_MESHLET_TRIANGLES = 124;

    enum ModelFlags : u32
    {
        ModelFlags_None = 0,
        ModelFlags_HasEmbeddedInstances = 1u << 0
    };

    enum MeshFlags : u32
    {
        MeshFlags_None = 0,
        MeshFlags_Skinned = 1u << 0
    };

    enum MeshLODFlags : u32
    {
        MeshLODFlags_None = 0,
        MeshLODFlags_HasSkinningData = 1u << 0,
        MeshLODFlags_StaticFallbackIsBindPose = 1u << 1
    };

    enum SubmeshFlags : u32
    {
        SubmeshFlags_None = 0
    };

    struct Bounds
    {
        vec3 center = {};
        f32 sphereRadius = 0.0f;
        vec3 extents = {};
        u32 reserved = 0;
    };

    // Hot position stream. xyz are u16 UNORM values decoded against the owning
    // Mesh's positionDecodeOffset/positionDecodeExtent. The fourth component is
    // explicit padding so GPU element loads remain naturally aligned.
    struct PackedPosition
    {
        u16 x = 0;
        u16 y = 0;
        u16 z = 0;
        u16 reserved = 0;
    };

    // Cold stream. Normals are octahedral 2x snorm16. Tangents use octahedral
    // 2x snorm15 in bits [0, 29], handedness in bit 30, and zero in bit 31.
    // UV words are packed IEEE half2 values. These explicit layouts keep the
    // AssetConverter and shader unpack paths from acquiring implicit ABI rules.
    struct PackedVertexAttributes
    {
        u32 normal = 0;
        u32 tangent = 0;
        u32 uv0 = 0;
        u32 uv1 = 0;
    };

    // Four u8 mesh-local joint indices and four normalized u8 weights. The
    // MeshLOD's jointPaletteRemap translates these compact indices to Skeleton
    // joint indices. Static LODs have no corresponding records.
    struct PackedSkinningData
    {
        u32 jointIndices = 0;
        u32 jointWeights = 0;
    };

    // Three local meshlet-vertex indices occupy the low 24 bits. The high byte is
    // reserved and must be zero. One u32 per triangle avoids unaligned 3-byte GPU
    // loads while preserving the 7-bit visibility-buffer triangleID contract.
    struct PackedMeshletTriangle
    {
        u32 localVertexIndices = 0;
    };

    // GPU-facing, fixed 32-byte meshlet metadata. vertexOffset addresses the root
    // meshletVertexIndices array and triangleOffset addresses meshletTriangles.
    // Meshlet vertex indices are relative to the owning MeshLOD's vertex range.
    struct Meshlet
    {
        vec3 boundsCenter = {};
        f32 boundsRadius = 0.0f;
        u32 vertexOffset = 0;
        u32 triangleOffset = 0;
        u32 packedNormalCone = 0;
        u16 vertexCount = 0;
        u16 triangleCount = 0;
    };

    // A material-homogeneous range within exactly one MeshLOD. Meshlets never
    // cross Submesh boundaries. materialSlotIndex is local to the owning Mesh's
    // materialSlots range; the other IDs are stable semantic values. Submesh array
    // indices do not need to correspond between LODs.
    struct Submesh
    {
        u32 meshletOffset = 0;
        u32 numMeshlets = 0;
        u32 materialSlotIndex = 0;
        u32 geometryGroupID = 0;
        u32 semanticPartID = 0;
        u32 flags = SubmeshFlags_None;
    };

    // Offset/count pairs are asset-local element indices into arrays on
    // ModelAsset. Vertex and attribute counts must match. Skinning is either empty
    // or has the same count. Every LOD is independently meshletized.
    struct MeshLOD
    {
        u32 vertexOffset = 0;
        u32 numVertices = 0;
        u32 vertexAttributeOffset = 0;
        u32 numVertexAttributes = 0;
        u32 skinningDataOffset = 0;
        u32 numSkinningData = 0;
        u32 submeshOffset = 0;
        u32 numSubmeshes = 0;
        u32 meshletOffset = 0;
        u32 numMeshlets = 0;
        u32 jointPaletteRemapOffset = 0;
        u32 numJointPaletteRemaps = 0;
        Bounds bounds;
        f32 geometricError = 0.0f;
        u32 flags = MeshLODFlags_None;
    };

    // LOD selection belongs to the Mesh. All LOD positions share this Mesh-level
    // quantization grid so coincident source vertices decode identically.
    struct Mesh
    {
        u32 lodOffset = 0;
        u32 numLODs = 0;
        u32 materialSlotOffset = 0;
        u32 numMaterialSlots = 0;
        AssetID skeletonAssetID = INVALID_ASSET_ID;
        AssetID animationBoundsAssetID = INVALID_ASSET_ID;
        Bounds bounds;
        vec3 positionDecodeOffset = {};
        f32 reserved0 = 0.0f;
        vec3 positionDecodeExtent = {};
        u32 flags = MeshFlags_None;
    };

    struct MaterialSlot
    {
        AssetID defaultMaterialInstanceAssetID = INVALID_ASSET_ID;
        u64 nameHash = 0;
        u32 stableID = 0;
        u32 reserved = 0;
    };

    // Generalized embedded child-model placement used for converted WMO doodads
    // and similar authored model hierarchies. Runtime world placement remains a
    // Scene concern rather than GPU ModelAsset data.
    struct EmbeddedInstance
    {
        AssetID modelAssetID = INVALID_ASSET_ID;
        vec3 position = {};
        quat rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
        f32 uniformScale = 1.0f;
        u32 color = 0xFFFFFFFFu;
        u32 stableID = 0;
        u32 reserved = 0;
    };

    struct EmbeddedInstanceSet
    {
        u64 nameHash = 0;
        u32 instanceOffset = 0;
        u32 numInstances = 0;
        u32 stableID = 0;
        u32 reserved = 0;
    };

    // Converter-side ownership for Save. These vectors are never serialized as
    // C++ containers; Save writes only their contiguous element bytes.
    struct ModelData
    {
        std::vector<Mesh> meshes;
        std::vector<MeshLOD> meshLODs;
        std::vector<Submesh> submeshes;
        std::vector<Meshlet> meshlets;

        std::vector<PackedPosition> positions;
        std::vector<PackedVertexAttributes> vertexAttributes;
        std::vector<PackedSkinningData> skinningData;
        std::vector<u32> meshletVertexIndices;
        std::vector<PackedMeshletTriangle> meshletTriangles;
        std::vector<u16> jointPaletteRemaps;

        std::vector<MaterialSlot> materialSlots;
        std::vector<EmbeddedInstanceSet> embeddedInstanceSets;
        std::vector<EmbeddedInstance> embeddedInstances;
    };

    // Flat file root. Non-empty root sections begin on 16-byte boundaries and
    // root offsets are byte offsets from the start of the file;
    // offsets inside Mesh/MeshLOD/Submesh records are element offsets into the
    // corresponding root array. AssetConverter writes every root array in its
    // final GPU representation. Game validates the sections, bulk-copies them,
    // and records the resulting runtime base indices. No render-batch translation,
    // meshlet construction, material classification, or vertex repacking occurs
    // at load. Save/Read operate on Bytebuffer like the existing PACT-backed
    // formats; archive and disk I/O remain the caller's responsibility.
    struct ModelAsset
    {
        FileHeader header = FileHeader(FILE_TYPE, DEVELOPMENT_VERSION);
        Bounds bounds;
        AssetID collisionAssetID = INVALID_ASSET_ID;

        u32 meshesOffset = 0;
        u32 numMeshes = 0;
        u32 meshLODsOffset = 0;
        u32 numMeshLODs = 0;
        u32 submeshesOffset = 0;
        u32 numSubmeshes = 0;
        u32 meshletsOffset = 0;
        u32 numMeshlets = 0;

        u32 positionsOffset = 0;
        u32 numPositions = 0;
        u32 vertexAttributesOffset = 0;
        u32 numVertexAttributes = 0;
        u32 skinningDataOffset = 0;
        u32 numSkinningData = 0;
        u32 meshletVertexIndicesOffset = 0;
        u32 numMeshletVertexIndices = 0;
        u32 meshletTrianglesOffset = 0;
        u32 numMeshletTriangles = 0;
        u32 jointPaletteRemapsOffset = 0;
        u32 numJointPaletteRemaps = 0;

        u32 materialSlotsOffset = 0;
        u32 numMaterialSlots = 0;
        u32 embeddedInstanceSetsOffset = 0;
        u32 numEmbeddedInstanceSets = 0;
        u32 embeddedInstancesOffset = 0;
        u32 numEmbeddedInstances = 0;

        u32 flags = ModelFlags_None;
        u32 geometryGroupCount = 0;

        size_t GetSerializedSize(const ModelData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const ModelData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, ModelAsset& out);
    };

    static_assert(sizeof(PackedPosition) == 8);
    static_assert(sizeof(vec3) == 12);
    static_assert(sizeof(quat) == 16);
    static_assert(sizeof(PackedVertexAttributes) == 16);
    static_assert(sizeof(PackedSkinningData) == 8);
    static_assert(sizeof(PackedMeshletTriangle) == 4);
    static_assert(sizeof(Meshlet) == 32);
    static_assert(sizeof(Submesh) == 24);
    static_assert(sizeof(MeshLOD) == 88);
    static_assert(sizeof(Mesh) == 96);
    static_assert(sizeof(MaterialSlot) == 24);
    static_assert(sizeof(EmbeddedInstance) == 56);
    static_assert(sizeof(EmbeddedInstanceSet) == 24);
    static_assert(sizeof(Bounds) == 32);
    static_assert(sizeof(ModelAsset) == 160);

    static_assert(std::is_trivially_copyable_v<ModelAsset>);
    static_assert(std::is_standard_layout_v<ModelAsset>);
    static_assert(std::is_trivially_copyable_v<Mesh>);
    static_assert(std::is_trivially_copyable_v<MeshLOD>);
    static_assert(std::is_trivially_copyable_v<Submesh>);
    static_assert(std::is_trivially_copyable_v<Meshlet>);
} // namespace FileFormat::Model
