#pragma once
#include "FileFormat/Shared.h"
#include "FileFormat/Novus/FileHeader.h"

#include "Base/Types.h"
#include "Base/Platform.h"

namespace NavMesh
{
    // .nav files are raw Detour dtCreateNavMeshData tile blobs. They are
    // intentionally not wrapped in a Novus header because Detour expects the
    // data pointer to start at dtMeshHeader. The Detour blob is versioned by
    // Detour's DT_NAVMESH_MAGIC and DT_NAVMESH_VERSION values.
    static constexpr const char* TILE_FILE_EXTENSION = ".nav";
    static constexpr u32 TILE_LAYER = 0;
    static constexpr bool USE_64BIT_POLY_REFS = true;

    namespace Agent
    {
        inline constexpr f32 HEIGHT = 1.6f;
        inline constexpr f32 RADIUS = 0.5333333f;
        inline constexpr f32 MAX_CLIMB = 1.6f;
        inline constexpr f32 MAX_SLOPE = 70.0f;
    }

    namespace TerrainHeight
    {
        static constexpr const char* FILE_EXTENSION = ".height";
        static constexpr u32 CURRENT_VERSION = 1;
        static constexpr u32 HEIGHT_COUNT = Terrain::CHUNK_NUM_CELLS * Terrain::CELL_TOTAL_GRID_SIZE;
        static constexpr u32 HOLE_COUNT = Terrain::CHUNK_NUM_CELLS;
        static constexpr u32 HEIGHT_DATA_SIZE = HEIGHT_COUNT * sizeof(f32);
        static constexpr u32 HOLE_DATA_SIZE = HOLE_COUNT * sizeof(u64);

        // Payload: Header, then heightCount f32 values in native 145-value ADT
        // cell order, then holeCount u64 values. Hole bit N masks terrain
        // patch N in that cell.
        struct Header
        {
        public:
            FileHeader header = FileHeader(FileHeader::Type::NavMeshTerrainHeight, CURRENT_VERSION);
            u32 headerSize = sizeof(Header);
            u32 chunkX = 0;
            u32 chunkY = 0;
            f32 originX = 0.0f;
            f32 originZ = 0.0f;
            f32 chunkSize = 0.0f;
            u32 cellsPerChunkStride = 0;
            u32 outerVerticesPerCellStride = 0;
            u32 innerVerticesPerCellStride = 0;
            u32 verticesPerCell = 0;
            u32 heightCount = 0;
            u32 holeCount = 0;
            u32 reserved = 0;
        };
        static_assert(sizeof(Header) == 60, "NavMesh::TerrainHeight::Header is not 60 byte in size");

        inline bool IsValidHeader(const Header& header)
        {
            return header.header == FileHeader(FileHeader::Type::NavMeshTerrainHeight, CURRENT_VERSION) &&
                header.headerSize == sizeof(Header) &&
                header.chunkSize == Terrain::CHUNK_SIZE &&
                header.cellsPerChunkStride == Terrain::CHUNK_NUM_CELLS_PER_STRIDE &&
                header.outerVerticesPerCellStride == Terrain::CELL_OUTER_GRID_STRIDE &&
                header.innerVerticesPerCellStride == Terrain::CELL_INNER_GRID_STRIDE &&
                header.verticesPerCell == Terrain::CELL_TOTAL_GRID_SIZE &&
                header.heightCount == HEIGHT_COUNT &&
                header.holeCount == HOLE_COUNT;
        }
    }
}
