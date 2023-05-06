#pragma once
#include "FileFormat/Shared.h"

#include <Base/Types.h>
#include <Base/Platform.h>

#include <vector>

namespace Adt
{
PRAGMA_NO_PADDING_START;
	struct MCCV // MCCV provides us with vertex shading information for the vertices
	{
	public:
		struct Color
		{
			u8 blue = 127;
			u8 green = 127;
			u8 red = 127;
			u8 alpha = 127;
		};

		Color color[Terrain::CELL_TOTAL_GRID_SIZE] = { };
	};

	struct MCNR // MCNR provides us with normals for the vertices
	{
	public:
		struct Normal
		{
			i8 normal[3] = { 0 };
		};

		Normal normals[Terrain::CELL_TOTAL_GRID_SIZE] = { };
	};

	struct MCSH // MCSH provides us with a baked shadowmap
	{
	public:
		// These are actually 64x64 1 bit values, each bit can be on or off "black/white", the order is LSB first.
		u8 bitMask[8][8] = { };
	};

	struct MCAL // MCAL provides us with alpha maps for texture layers
	{
	public:
		struct AlphaMap
		{
			u8 alphaMap[4096] = { 0 };
		};

		std::vector<AlphaMap> data = { };
	};

	struct MCLY // MCLY provides us with a list of ?
	{
	public:
		struct Flags
		{
			u32 AnimationRotation_X : 1;
			u32 AnimationRotation_Y : 1;
			u32 AnimationRotation_Z : 1;
			u32 AnimationSpeed_X : 1;
			u32 AnimationSpeed_Y : 1;
			u32 AnimationSpeed_Z : 1;
			u32 AnimationEnabled : 1;
			u32 Overbright : 1;
			u32 UseAlphaMap : 1;
			u32 CompressedAlphaMap : 1;
			u32 UseCubeMapReflection : 1;
			u32 Unk_0x800 : 1;
			u32 Unk_0x1000 : 1;
		};

		struct LayerDefinition
		{
			u32 textureID = 0;
			Flags flags;
			u32 offsetInMCAL = 0;
			u32 effectId = 0;
		};

		std::vector<LayerDefinition> data = { };
	};

	struct MCVT // MCVT provides us with the height information for a cell.
	{
	public:
		f32 heightMap[Terrain::CELL_TOTAL_GRID_SIZE] = { 0.0f };
	};

	struct MCNK // MCNK provides us with all the information we need for a cell.
	{
	public:
		struct Flags
		{
			u32 HasMCSH : 1;
			u32 Impass : 1;
			u32 LiquidRiver : 1;
			u32 LiquidOcean : 1;
			u32 LiquidMagma : 1;
			u32 LiquidSlime : 1;
			u32 HasMMCV : 1;
			u32 Unk_0x80 : 1;
			u32 : 7;
			u32 DoNotFixAlphaMap : 1;
			u32 HighResHoles : 1;
		};

		Flags flags = { };
		uvec2 coordinates = uvec2(0, 0); // X, Y
		u32 numLayers = 0;
		u32 numDoodadRefs = 0;

		u64 holesHighRes = 0;

		u32 unused1 = 0;
		u32 unused2 = 0;
		u32 unused3 = 0;
		u32 unused4 = 0;
		u32 unused5 = 0;
		u32 unused6 = 0;
		u32 areaId = 0;
		u32 numMapObjectRefs = 0; // Number of WMO Refs
		u16 holesLowRes = 0;
		u16 unused7 = 0;
		u8 lowQualityTextureMap[16] = { 0 }; // Used to determine where doodads are shown. Values are an array of two bit
		u8 disableDoodadMap[8] = { 0 }; // Used to determine if Doodads are disabled (This takes priority over lowQualityTextureMap) Values are an array of 1 bit
		u32 unused8 = 0;
		u32 unused9 = 0;
		u32 unused10 = 0;
		u32 unused11 = 0;
		vec3 position = vec3(0, 0, 0); // Location is stored as Z, X, Y
		u32 unused12 = 0;
		u32 unused13 = 0;
		u32 unused14 = 0;
	};
	struct MH2O // MH2O provides us with multiple lists related to water information.
	{
	public:
		struct LiquidHeader
		{
			u32 instanceOffset = 0;
			u32 layerCount = 0;
			u32 attributeOffset = 0;
		};
		struct LiquidInstance
		{
			u16 liquidType = 0; // Foreign Key (Referencing LiquidType.dbc)
			u16 liquidVertexFormat = 0; // Classic, TBC and WOTLK Only (Cata+ Foreign Key)

			vec2 heightLevel = vec2(0.0f, 0.0f);

			u8 offsetX = 0;
			u8 offsetY = 0;
			u8 width = 0;
			u8 height = 0;

			u32 bitmapDataOffset = 0; // BitmapData, contains ((width * height + 7) / 8) bytes.
			u32 vertexDataOffset = 0; // Vertex Data, can be in 2 formats for WOTLK and will always contain vertexCount entries for both arrays (f32* heightMap, char* depthMap), (f32* heightMap, UVEntry* uvMap)
		};
		struct LiquidAttribute
		{
			u64 fishableBitmap = 0;
			u64 fatigueBitmap = 0;
		};

		// The following 3 Structs only exists for the purpose of being able to sizeof()
		// This makes it easier to read the code (The actual) structs in memory are arrays one after another
		struct LiquidUVMapEntry
		{
			u16 x;
			u16 y;
		};
		struct LiquidVertexFormat_Height
		{
			f32 heightMap;
		};
		struct LiquidVertexFormat_Height_UV
		{
			f32 heightMap;
			LiquidUVMapEntry uv;
		};

		std::vector<LiquidHeader> headers;
		std::vector<LiquidInstance> instances;
		std::vector<LiquidAttribute> attributes;

		std::vector<u8> bitmapData;
		std::vector<u8> vertexData;
	};
	struct MTXF // MTXF provides us with a list of flags for MTEX entries (They map 1:1)
	{
	public:
		struct Flags
		{
			u32 UseCubemap : 1;
			u32 : 3;
			u32 TextureScalingLayer0 : 1;
			u32 TextureScalingLayer1 : 1;
			u32 TextureScalingLayer2 : 1;
			u32 TextureScalingLayer3 : 1;
		};

		std::vector<Flags> data = { };
	};
	struct MFBO // MFBO provides us with a min/max height plane that defines the 
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
	struct MODF // MODF provides us with a list of WMO object placement information.
	{
	public:
		struct PlacementInfo
		{
		public:
			struct Flags
			{
				u16 Destroyable : 1;
				u16 UseLod : 1;
				u16 HasScale : 1;
				u16 EntryIsFiledataID : 1;
				u16 : 3;
				u16 UseSetsFromMWDS : 1;
			};

			u32 fileID = 0;
			u32 uniqueID = 0;	// [Original Client, Runtime Only] This is a unique value that can be used to determine if this placement already was processed

			vec3 position = vec3(0.0f, 0.0f, 0.0f); // This determines the position of the placement.
			vec3 rotation = vec3(0.0f, 0.0f, 0.0f); // This determines the rotation of the placement.

			// TODO : replace these 2 lines with a single data type
			// Bounding Box
			vec3 min = vec3(0, 0, 0);
			vec3 max = vec3(0, 0, 0);

			Flags flags = { };
			u16 doodadSet = 0;
			u16 nameSet = 0; // This is a name override, this allows one model to go by multiple names (Example : Goldshire Inn vs Northshire Inn)
			u16 scale = 0; // This determines the scale of the placement, however the mapping here is 1024 == 1.0f.
		};

		std::vector<PlacementInfo> data = { };
	};
	struct MDDF // MDDF provides us with a list of M2 object placement information.
	{
	public:
		struct PlacementInfo
		{
		public:
			struct Flags
			{
				u16 Biodome : 1;
				u16 Shrubbery : 1;
				u16 Unk0x4 : 1;
				u16 Unk0x8 : 1;
				u16 : 1;
				u16 LiquidKnown : 1;
				u16 EntryIsFiledataID : 1;
				u16 : 1;
				u16 Unk0x100 : 1;
			};

			u32 fileID = 0;
			u32 uniqueID = 0;	// [Original Client, Runtime Only] This is a unique value that can be used to determine if this placement already was processed

			vec3 position = vec3(0.0f, 0.0f, 0.0f); // This determines the position of the placement.
			vec3 rotation = vec3(0.0f, 0.0f, 0.0f); // This determines the rotation of the placement.
			u16 scale = 0; // This dtermines the scale of the placement, however the mapping here is 1024 == 1.0f;

			Flags flags;
		};

		std::vector<PlacementInfo> data = { };
	};
	struct MHID // MHID provides us a list of height texture ids.
	{
	public:
		std::vector<u32> data = { };
	};
	struct MDID // MDID provides us a list of diffuse texture ids.
	{
	public:
		std::vector<u32> data = { };
	};
	struct MHDR // MHDR provides us a list of offsets to other chunks in the file.
	{
	public:
		struct Flags
		{
			u32 HasFlyingBoundingBox : 1;
			u32 Unk0x2 : 1; // This is set for some Northrend Terrain Chunks (Investigate)
		};

		struct ChunkOffsets
		{
			// Chunk Offsets are specified relative to MHDR's own address (After Token & Size), we however convert them to absolute
			// Unused fields used to have a purpose in older versions, but is now either moved out of the root file or is no longer used.

			u32 unused1 = 0;
			u32 unused2 = 0;
			u32 unused3 = 0;
			u32 unused4 = 0;
			u32 unused5 = 0;
			u32 unused6 = 0;
			u32 unused7 = 0;
			u32 unused8 = 0;
			u32 mfbo = 0;
			u32 mh2o = 0;
			u32 unused9 = 0;
		};

		Flags flags = { };
		ChunkOffsets offsets = { };

		uint8_t mampExponent;
		uint8_t padding[15] = { 0 };
	};

	struct MPHD
	{
	public:
		struct Flags
		{
			u32 UseGlobalMapObj : 1;
			u32 UseMCCV : 1;
			u32 UseBigAlpha : 1;
			u32 SortDoodadsBySizeCategory : 1;
			u32 UseLightingVertices : 1;
			u32 UseUpsideDownGround : 1;
			u32 Unk_0x40 : 1;
			u32 UseHeightTexturing : 1;
			u32 Unk_0x100 : 1;
			u32 UseMAID : 1;
			u32 Unk_0x400 : 1;
			u32 Unk_0x800 : 1;
			u32 Unk_0x1000 : 1;
			u32 Unk_0x2000 : 1;
			u32 Unk_0x4000 : 1;
			u32 Unk_0x8000 : 1;
		};

		struct FileIDs
		{
			u32 lgtFileID = 0;
			u32 occFileID = 0;
			u32 fogsFileID = 0;
			u32 mpvFileID = 0;
			u32 texFileID = 0;
			u32 wdlFileID = 0;
			u32 pd4FileID = 0;
		};

		Flags flags = { };
		FileIDs fileIDs = { };
	};

	struct MAIN
	{
		struct Flags
		{
			u32 IsUsed : 1;
			u32 IsWaterOnly : 1;
			u32 IsLoaded : 1; // [Runtime Only, Original Client]
		};

		struct AreaInfo
		{
			Flags flags;
			u32 asyncID; // [Runtime Only, Original Client]
		};

		AreaInfo areaInfos[Terrain::CHUNK_NUM_PER_MAP_STRIDE][Terrain::CHUNK_NUM_PER_MAP_STRIDE] = {};
	};
	struct MAID
	{
	public:
		struct FileIDs
		{
			u32 adtRootFileID = 0;
			u32 adtObject1FileID = 0;
			u32 adtObject2FileID = 0;
			u32 adtTextureFileID = 0;
			u32 adtLodFileID = 0;
			u32 adtMapTextureFileID = 0;
			u32 adtMapTextureNFileID = 0;
			u32 adtMiniMapTextureFileID = 0;
		};

		FileIDs fileIDs[Terrain::CHUNK_NUM_PER_MAP_STRIDE][Terrain::CHUNK_NUM_PER_MAP_STRIDE] = {};
	};

	struct Wdt
	{
	public:
		MVER mver = { };
		MPHD mphd = { };
		MAIN main = { };
		MAID maid = { };
		MODF modf = { };
	};

	struct CellInfo // CellInfo contains MCNK + All Sub Chunks. Without reading the Sub Chunks MCNK in of itself is pretty redundant.
	{
		MCNK mcnk;
		MCVT mcvt;
		MCSH mcsh;
		MCLY mcly;
		MCAL mcal;
		MCNR mcnr;
		MCCV mccv;
	};

	struct Layout
	{
	public:
		u16 mapID = std::numeric_limits<u16>().max();
		u16 chunkID = std::numeric_limits<u16>().max();

		MVER mver = { };
		MHDR mhdr = { };
		MDID mdid = { };
		MHID mhid = { };
		MDDF mddf = { };
		MODF modf = { };
		MFBO mfbo = { };
		MTXF mtxf = { };
		MH2O mh2o = { };

		std::vector<CellInfo> cellInfos;
	};
PRAGMA_NO_PADDING_END;
}