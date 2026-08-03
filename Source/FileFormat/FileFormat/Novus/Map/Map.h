#pragma once
#include "FileFormat/Shared.h"
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>

#include <type_traits>

namespace Map
{
    static const std::string HEADER_FILE_EXTENSION = ".map";

    enum ModelAllocationHintFlags : u32
    {
        ModelAllocationHintFlags_None = 0,

        // Scene counts include every serialized root placement. A runtime may
        // deduplicate placements by stable identity and consequently use less.
        ModelAllocationHintFlags_SceneCountsAreUpperBounds = 1u << 0
    };

    // Aggregate element counts for the unique, renderable Model V2 assets
    // reachable from a map's placements. These are reservation hints only;
    // readers must not reject content when runtime totals differ.
    struct ModelResourceAllocationHints
    {
        u64 models = 0;
        u64 meshes = 0;
        u64 meshLODs = 0;
        u64 submeshes = 0;
        u64 meshlets = 0;
        u64 positions = 0;
        u64 vertexAttributes = 0;
        u64 skinningRecords = 0;
        u64 meshletVertexIndices = 0;
        u64 meshletTriangleRecords = 0; // PackedMeshletTriangle records (4 bytes each).
        u64 jointPaletteRemaps = 0;
        u64 materialSlots = 0;
        u64 embeddedInstanceSets = 0;
        u64 embeddedInstanceRecords = 0;

        ModelResourceAllocationHints& operator+=(const ModelResourceAllocationHints& other);
    };
    static_assert(sizeof(ModelResourceAllocationHints) == 112);

    struct ModelSceneAllocationHints
    {
        u64 rootPlacements = 0;
        u64 selectedRenderableEmbeddedInstances = 0;
        u64 totalModelInstances = 0;
        u64 geometryGroupMaskWords = 0;
        u64 meshletHistoryWords = 0;

        ModelSceneAllocationHints& operator+=(const ModelSceneAllocationHints& other);
    };
    static_assert(sizeof(ModelSceneAllocationHints) == 40);

    struct ModelAllocationHints
    {
        ModelResourceAllocationHints resources;
        ModelSceneAllocationHints scene;
        u32 flags = ModelAllocationHintFlags_None;
        u32 reserved = 0;
    };
    static_assert(sizeof(ModelAllocationHints) == 160);
    static_assert(std::is_trivially_copyable_v<ModelResourceAllocationHints>);
    static_assert(std::is_trivially_copyable_v<ModelSceneAllocationHints>);
    static_assert(std::is_trivially_copyable_v<ModelAllocationHints>);

    struct MapHeader
    {
    public:
        static const u32 CURRENT_VERSION = 4;

        struct Flags
        {
            u32 UseMapObjectAsBase : 1;
        };

    public:
        FileHeader header = FileHeader(FileHeader::Type::MapHeader, CURRENT_VERSION);

        Flags flags = { };
        Terrain::Placement placement = { };
        ModelAllocationHints modelAllocationHints;
        std::vector<u64> chunkHashes;

    public:
        bool Save(std::shared_ptr<Bytebuffer>& buffer);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, MapHeader& out);
    };
}
