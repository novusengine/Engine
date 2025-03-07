#pragma once
#include "FileFormat/Shared.h"
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>

#include <vector>

namespace Adt
{
    struct Layout;
}

namespace Map
{
    static const std::string CHUNK_FILE_EXTENSION = ".chunk";

    struct CellLiquidHeader
    {
    public:
        // Packed Format
        // Bit 1-7 (numInstances)
        // Bit 8 (hasAttributes)
        u8 packedData = 0;
    };

    struct CellLiquidInstance
    {
    public:
        u8 liquidTypeID = 0; // Reference into LiquidType.cdb
        u8 packedData = 0; // (Bit 1-6 LVF, Bit 7 hasBitmapForLiquid, Bit 8 hasVertexData)
        u8 packedOffset = 0; // (Bit 1-3 X, Bit 4-8 Y)
        u8 packedSize = 0; // (Bit 1-3 Width, Bit 4-8 Height)

        f32 height = 0.0f;

        u32 bitmapDataOffset = 0;
        u32 vertexDataOffset = 0;
    };

    struct CellLiquidAttributes
    {
    public:
        u64 fishableBitmap = 0;
        u64 fatigueBitmap = 0;
    };

    struct LiquidInfo
    {
    public:
        std::vector<CellLiquidHeader> headers = { };
        std::vector<CellLiquidInstance> instances = { };
        std::vector<CellLiquidAttributes> attributes = { };

        std::vector<u8> bitmapData = { };
        std::vector<u8> vertexData = { };
    };

    struct CellsData
    {
    public:
        static constexpr u32 CELL_LAYER_COUNT = 4;

    public:
        u16 areaIDs[Terrain::CHUNK_NUM_CELLS];
        vec2 heightBounds[Terrain::CHUNK_NUM_CELLS];
        f32 heightField[Terrain::CHUNK_NUM_CELLS][Terrain::CELL_TOTAL_GRID_SIZE];
        u8 normals[Terrain::CHUNK_NUM_CELLS][Terrain::CELL_TOTAL_GRID_SIZE][3];
        u8 colors[Terrain::CHUNK_NUM_CELLS][Terrain::CELL_TOTAL_GRID_SIZE][3];
        u64 holes[Terrain::CHUNK_NUM_CELLS];
        u32 layerTextureIDs[Terrain::CHUNK_NUM_CELLS][CELL_LAYER_COUNT];
    };
    static_assert(sizeof(CellsData) == 379904, "CellsData is not 454144 byte in size");

    struct Chunk
    {
    public:
        static const u32 CURRENT_VERSION = 7;

        struct HeightHeader
        {
        public:
            u8 hasHeightBox = false;
            u8 padding[3] = { 0 };
            f32 gridMinHeight = 20000;
            f32 gridMaxHeight = -20000;
            u32 padding1 = 0;
        };
        static_assert(sizeof(HeightHeader) == 16, "HeightHeader is not 16 byte in size");

        struct HeightBox
        {
        public:
            struct HeightPlane
            {
                // For future implementation: https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-bots-programs/wow-memory-editing/351404-traceline-intersection-collision-detection-height-limit.html
                i16 heightPoints[3 * 3] = { 0, 0, 0, 0, 0, 0, 0 };
            };

            HeightPlane max = { };
            HeightPlane min = { };
            u32 padding = 0;
        };
        static_assert(sizeof(HeightBox) == 40, "HeightBox is not 40 byte in size");

        struct PlacementHeader
        {
        public:
            Terrain::Placement* GetPlacement(std::shared_ptr<Bytebuffer>& buffer, u32 placementIndex)
            {
                if (placementIndex >= numPlacements)
                    return nullptr;

                u64 placementOffset = dataOffset + (placementIndex * sizeof(Terrain::Placement));
                if (placementOffset >= buffer->writtenData)
                    return nullptr;

                Terrain::Placement* placement = reinterpret_cast<Terrain::Placement*>(&buffer->GetDataPointer()[placementOffset]);
                return placement;
            }
        public:
            u64 dataOffset = 0;
            u32 numPlacements = 0;
            u32 padding = 0;
        };
        static_assert(sizeof(PlacementHeader) == 16, "PlacementHeader is not 16 byte in size");

        struct LiquidHeader
        {
        public:
            const CellLiquidHeader* GetHeader(std::shared_ptr<Bytebuffer>& buffer, u8 headerIndex) const
            {
                if (headerIndex >= numHeaders)
                    return nullptr;

                u64 headerOffset = dataOffset + (headerIndex * sizeof(CellLiquidHeader));
                if (headerOffset >= buffer->writtenData)
                    return nullptr;

                CellLiquidHeader* header = reinterpret_cast<CellLiquidHeader*>(&buffer->GetDataPointer()[headerOffset]);
                return header;
            }
            const CellLiquidInstance* GetInstance(std::shared_ptr<Bytebuffer>& buffer, u16 instanceIndex) const
            {
                if (instanceIndex >= numInstances)
                    return nullptr;

                u64 numBytesBeforeInstances = dataOffset + (numHeaders * sizeof(CellLiquidHeader));
                u64 instanceOffset = numBytesBeforeInstances + (instanceIndex * sizeof(CellLiquidInstance));
                if (instanceOffset >= buffer->writtenData)
                    return nullptr;

                CellLiquidInstance* instance = reinterpret_cast<CellLiquidInstance*>(&buffer->GetDataPointer()[instanceOffset]);
                return instance;
            }
            const CellLiquidAttributes* GetAttributes(std::shared_ptr<Bytebuffer>& buffer, u8 attributesIndex) const
            {
                if (attributesIndex >= numAttributes)
                    return nullptr;

                u64 numBytesBeforeAttributes = dataOffset + (numHeaders * sizeof(CellLiquidHeader)) + (numInstances * sizeof(CellLiquidInstance));
                u64 attributesOffset = numBytesBeforeAttributes + (attributesIndex * sizeof(CellLiquidAttributes));
                if (attributesOffset >= buffer->writtenData)
                    return nullptr;

                CellLiquidAttributes* attributes = reinterpret_cast<CellLiquidAttributes*>(&buffer->GetDataPointer()[attributesOffset]);
                return attributes;
            }

            const u8* GetBitmapBytes(std::shared_ptr<Bytebuffer>& buffer, u32 bitmapIndex) const
            {
                if (numBitmapBytes == 0)
                    return nullptr;

                u64 numBytesBeforeBitmap = dataOffset + (numHeaders * sizeof(CellLiquidHeader)) + (numInstances * sizeof(CellLiquidInstance) + (numAttributes * sizeof(CellLiquidAttributes)));
                u64 bitmapOffset = numBytesBeforeBitmap + (bitmapIndex * sizeof(u8));
                if (bitmapOffset >= buffer->writtenData)
                    return nullptr;

                u8* bitmapBytes = &buffer->GetDataPointer()[bitmapOffset];
                return bitmapBytes;
            }
            const u8* GetVertexBytes(std::shared_ptr<Bytebuffer>& buffer, u32 vertexIndex) const
            {
                if (numVertexBytes == 0)
                    return nullptr;

                u64 numBytesBeforeVertices = dataOffset + (numHeaders * sizeof(CellLiquidHeader)) + (numInstances * sizeof(CellLiquidInstance) + (numAttributes * sizeof(CellLiquidAttributes)) + numBitmapBytes);
                u64 vertexOffset = numBytesBeforeVertices + (vertexIndex * sizeof(u8));
                if (vertexOffset >= buffer->writtenData)
                    return nullptr;

                u8* vertexBytes = &buffer->GetDataPointer()[vertexOffset];
                return vertexBytes;
            }

        public:
            u64 dataOffset = 0;
            u32 numHeaders = 0;
            u32 numInstances = 0;
            u32 numAttributes = 0;
            u32 numBitmapBytes = 0;
            u32 numVertexBytes = 0;
            u32 padding = 0;
        };
        static_assert(sizeof(LiquidHeader) == 32, "LiquidHeader is not 32 byte in size");

        struct PhysicsHeader
        {
        public:
            u8* GetPhysicsData(std::shared_ptr<Bytebuffer>& buffer)
            {
                if (numBytes == 0 || dataOffset >= buffer->writtenData)
                    return nullptr;

                u8* physicsData = &buffer->GetDataPointer()[dataOffset];
                return physicsData;
            }

        public:
            u64 dataOffset = 0;
            u32 numBytes = 0;
            u32 padding = 0;
        };
        static_assert(sizeof(PhysicsHeader) == 16, "PhysicsHeader is not 16 byte in size");

    public:
        FileHeader header = FileHeader(FileHeader::Type::MapChunk, CURRENT_VERSION);

        HeightHeader heightHeader = { };
        HeightBox heightBox = { };

        CellsData cellsData;

        u64 chunkAlphaMapTextureHash = Terrain::TEXTURE_ID_INVALID;

        PlacementHeader placementHeader;
        LiquidHeader liquidHeader;
        PhysicsHeader physicsHeader;

    public:
        bool Save(const std::string& path, const std::vector<Terrain::Placement>& modelPlacements, const LiquidInfo& liquidInfo, const std::vector<u8>& physicsData);

        static bool Read(std::shared_ptr<Bytebuffer>& buffer, Chunk& out);
        static bool FromADT(Adt::Layout& layout, Chunk& out, std::vector<Terrain::Placement>& modelPlacements, LiquidInfo& liquidInfo);
    };
}
