#pragma once
#include "FileFormat/Novus/FileHeader.h"
#include "FileFormat/Warcraft/Shared.h"

#include <Base/Types.h>
#include <Base/Platform.h>

#include <vector>

class Bytebuffer;

namespace Wmo
{
	struct Layout;
}

namespace Model
{
PRAGMA_NO_PADDING_START;
	struct MapObjectGroup
	{
	public:
		static constexpr u32 CURRENT_VERSION = 2;

		struct Flags
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
			u32 HasWater : 1;
			u32 IsInterior : 1;
			u32 : 1;
			u32 IsMountQueryAllowed : 1;
			u32 NoPortalCulling : 1;
			u32 : 1;
			u32 ShowSkybox : 1;
			u32 WaterIsOcean : 1;
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

		struct Vertex
		{
			hvec3 position = hvec3(static_cast<f16>(0.0f));
			u8 octNormal[2] = { 0, 0 };
			hvec4 uv = hvec4(static_cast<f16>(0.0f));
		};

		struct VertexColorSet
		{
			std::vector<IntColor> colors = { };
		};

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

		struct RenderBatch
		{
			u32 startVertex = 0;
			u16 vertexCount = 0;
			u32 startIndex = 0;
			u16 indexCount = 0;
			u16 materialID = 0;
		};

		struct CullingData
		{
			hvec3 center = hvec3(static_cast<f16>(100000.0f));
			hvec3 extents = hvec3(static_cast<f16>(-100000.0f));
			f32 boundingSphereRadius = 0.0f;
		};

	public:
		FileHeader header = FileHeader(FileHeader::Type::MapObjectGroup, CURRENT_VERSION);

		Flags flags = { };
		std::vector<Vertex> vertices = { };
		std::vector<u16> indices = { };
		std::vector<VertexColorSet> vertexColorSets = { };
		std::vector<TriangleMaterialInfo> triangleMaterialInfo = { };
		std::vector<RenderBatch> renderBatches = { };
		std::vector<CullingData> cullingData = { };
	};
	struct MapObject
	{
	public:
		static constexpr u32 CURRENT_VERSION = 1;
		static constexpr u32 INVALID_TEXTURE_ID = std::numeric_limits<u32>().max();

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

			u16 materialType = 0;
			u16 blendMode = 0;
			Flags flags = { };
			u32 textureID[3] = { INVALID_TEXTURE_ID, INVALID_TEXTURE_ID, INVALID_TEXTURE_ID };
		};

		struct Decoration
		{
			u32 nameID = 0;
			vec3 position = { };
			quat rotation = { };
			f32 scale = 0.0f;
			u32 color = 0;
		};

		struct DecorationSet
		{
			char name[20] = { 0 };
			u16 index = 0;
			u32 count = 0;
		};

	public:
		FileHeader header = FileHeader(FileHeader::Type::MapObject, CURRENT_VERSION);

		std::vector<Material> materials = { };
		std::vector<Decoration> decorations = { };
		std::vector<DecorationSet> decorationSets = { };
		std::vector<MapObjectGroup> groups = { };

	public:
		bool Save(const std::string& path);

		static bool Read(std::shared_ptr<Bytebuffer>& buffer, MapObject& out);
		static bool FromWMO(Wmo::Layout& layout, MapObject& out);
	};
PRAGMA_NO_PADDING_END;
}