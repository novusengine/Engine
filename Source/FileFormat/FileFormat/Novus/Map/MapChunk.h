#pragma once
#include "FileFormat/Novus/FileHeader.h"
#include "FileFormat/Warcraft/Shared.h"

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

		VertexData vertexData[Terrain::CELL_TOTAL_GRID_SIZE] = { };
		//f32 heightData[Terrain::CELL_TOTAL_GRID_SIZE] = { 0 };
		//u8 normalData[Terrain::CELL_TOTAL_GRID_SIZE][3] = { {127}, {255}, {127} }; // This is ugly but lets us pack this data into 25% of the original size
		//u8 colorData[Terrain::CELL_TOTAL_GRID_SIZE][3] = { {127}, {127}, {127} }; // This is ugly but lets us pack this data into 25% of the original size

		u64 hole = 0;

		LayerData layers[LAYER_COUNT] = { };
	};

	struct Chunk
	{
	public:
		static const u32 CURRENT_VERSION = 2;

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

		Cell cells[Terrain::CHUNK_NUM_CELLS] = { };

		u32 chunkAlphaMapTextureHash = Terrain::TEXTURE_ID_INVALID;
		std::vector<Terrain::Placement> mapObjectPlacements = { };
		std::vector<Terrain::Placement> complexModelPlacements = { };

	public:
		bool Save(const std::string& path);

		static bool Read(std::shared_ptr<Bytebuffer>& buffer, Chunk& out);
		static bool FromADT(Adt::Layout& layout, Chunk& out);
	};
	PRAGMA_NO_PADDING_END;
}