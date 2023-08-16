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
	PRAGMA_NO_PADDING_START;
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

		f32 height = 0.f;
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

	struct Cell
	{
	public:
		struct LayerData
		{
		public:
			u32 textureID = Terrain::TEXTURE_ID_INVALID;
		};

		struct VertexData
		{
		public:
			u8 normal[3] = { 0, 0, 0 };
			u8 color[3] = { 0, 0, 0 };
			f16 height = f16(0);
		};

	public:
		static constexpr u32 LAYER_COUNT = 4;

		u16 areaID = 0;

		f16 cellMinHeight = f16(10000);
		f16 cellMaxHeight = f16(-10000);

		VertexData vertexData[Terrain::CELL_TOTAL_GRID_SIZE];

		u64 hole = 0;

		LayerData layers[LAYER_COUNT] = { };
	};

	struct Chunk
	{
	public:
		static const u32 CURRENT_VERSION = 3;

		struct HeightHeader
		{
		public:
			u8 hasHeightBox = false;
			f32 gridMinHeight = 20000;
			f32 gridMaxHeight = -20000;
		};

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
		};

	public:
		FileHeader header = FileHeader(FileHeader::Type::MapChunk, CURRENT_VERSION);

		HeightHeader heightHeader = { };
		HeightBox heightBox = { };

		Cell cells[Terrain::CHUNK_NUM_CELLS];
		u32 chunkAlphaMapTextureHash = Terrain::TEXTURE_ID_INVALID;

		LiquidInfo liquidInfo = { };
		std::vector<Terrain::Placement> mapObjectPlacements = { };
		std::vector<Terrain::Placement> complexModelPlacements = { };

	public:
		bool Save(const std::string& path);

		static bool Read(std::shared_ptr<Bytebuffer>& buffer, Chunk& out);
		static bool FromADT(Adt::Layout& layout, Chunk& out);
	};
	PRAGMA_NO_PADDING_END;
}