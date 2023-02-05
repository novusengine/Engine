#include "MapChunk.h"
#include "FileFormat/Warcraft/ADT/Adt.h"

#include <Base/Util/DebugHandler.h>

#include <glm/gtx/euler_angles.hpp>
#include <fstream>
#include <bitset>

using namespace Adt;

namespace Map
{
	bool Chunk::Save(const std::string& path)
	{
		// Create a file
		std::ofstream output(path, std::ofstream::out | std::ofstream::binary);
		if (!output)
		{
			DebugHandler::PrintError("Failed to create Terrain Chunk file. Check admin permissions");
			return false;
		}

		// Write the Chunk to file
		output.write(reinterpret_cast<char const*>(&header), sizeof(header));
		output.write(reinterpret_cast<char const*>(&heightHeader), sizeof(heightHeader));
		output.write(reinterpret_cast<char const*>(&heightBox), sizeof(heightBox));
		output.write(reinterpret_cast<char const*>(&cells[0]), Terrain::CHUNK_NUM_CELLS * sizeof(Cell));

		output.write(reinterpret_cast<char const*>(&chunkAlphaMapTextureHash), sizeof(u32)); // Write alpha map string index

		u32 numMapObjectPlacements = static_cast<u32>(mapObjectPlacements.size());
		output.write(reinterpret_cast<char const*>(&numMapObjectPlacements), sizeof(u32)); // Write number of map object placements

		if (numMapObjectPlacements > 0)
		{
			output.write(reinterpret_cast<char const*>(mapObjectPlacements.data()), numMapObjectPlacements * sizeof(Terrain::Placement)); // Write map object placements
		}

		u32 numComplexModelPlacements = static_cast<u32>(complexModelPlacements.size());
		output.write(reinterpret_cast<char const*>(&numComplexModelPlacements), sizeof(u32)); // Write number of complex model placements

		if (numComplexModelPlacements > 0)
		{
			output.write(reinterpret_cast<char const*>(complexModelPlacements.data()), numComplexModelPlacements * sizeof(Terrain::Placement)); // Write map object placements
		}

		// Water (This data is prepared in the Mh2o::Read, because we need direct access to the buffer in order to convert the data)
		{
			u32 numLiquidHeaders = 0;// static_cast<u32>(mh2o.headers.size());
			output.write(reinterpret_cast<char const*>(&numLiquidHeaders), sizeof(u32)); // Write number of liquid headers
		}

		output.close();

		return true;
	}

	bool Chunk::Read(std::shared_ptr<Bytebuffer>& buffer, Chunk& out)
	{
		out = { };

		if (!buffer->Get(out.header))
			return false;

		if (!buffer->Get(out.heightHeader))
			return false;

		if (!buffer->Get(out.heightBox))
			return false;

		if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.cells[0]), Terrain::CHUNK_NUM_CELLS * sizeof(Cell)))
			return false;

		if (!buffer->GetU32(out.chunkAlphaMapTextureHash))
			return false;

		// Read Map Object Placements
		{
			u32 numPlacements = 0;
			if (!buffer->GetU32(numPlacements))
				return false;

			if (numPlacements)
			{
				out.mapObjectPlacements.resize(numPlacements);
				if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.mapObjectPlacements[0]), numPlacements * sizeof(Terrain::Placement)))
					return false;
			}
		}

		// Read CModel Placements
		{
			u32 numPlacements = 0;
			if (!buffer->GetU32(numPlacements))
				return false;

			if (numPlacements)
			{
				out.complexModelPlacements.resize(numPlacements);
				if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.complexModelPlacements[0]), numPlacements * sizeof(Terrain::Placement)))
					return false;
			}
		}

		// Read Water
		{
			u32 NumLiquidHeaders = 0;
			if (!buffer->GetU32(NumLiquidHeaders))
				return false;

			// TODO : Read Water
		}

		return true;
	}

	bool Chunk::FromADT(Adt::Layout& layout, Chunk& out)
	{
		// TODO : Set HeightBox

		for (u16 i = 0; i < Terrain::CHUNK_NUM_CELLS; i++)
		{
			const CellInfo& cellInfo = layout.cellInfos[i];
			const MCNK& mcnk = cellInfo.mcnk;

			Cell& cell = out.cells[i];
			cell.areaID = mcnk.areaId;

			if (mcnk.flags.HighResHoles)
			{
				cell.hole = mcnk.holesHighRes;
			}
			else
			{
				// Convert from holesLowRes to holesHighRes
				u16 holesLowRes = mcnk.holesLowRes;

				std::bitset<64> holesHighResBits = { 0 };

				constexpr u16 LOWRES_BITS_PER_ROW = 4;
				constexpr u16 HIGHRES_BITS_PER_ROW = 16;

				for (u32 rowIndex = 0; rowIndex < 4; rowIndex++)
				{
					u32 rowLowResBitBaseIndex = rowIndex * LOWRES_BITS_PER_ROW;
					u32 rowHighResBitBaseIndex = rowIndex * HIGHRES_BITS_PER_ROW;

					for (u32 columnIndex = 0; columnIndex < 4; columnIndex++)
					{
						u16 lowResBitIndex = rowLowResBitBaseIndex + columnIndex;
						u16 lowRestBitMask = 1 << lowResBitIndex;

						if ((holesLowRes & lowRestBitMask) == 0)
							continue;

						u16 highResBaseBitNum = rowHighResBitBaseIndex + (columnIndex * 2);
						u16 highResBit1 = highResBaseBitNum;
						u16 highResBit2 = highResBaseBitNum + 1;
						u16 highResBit3 = highResBaseBitNum + Terrain::CELL_INNER_GRID_STRIDE;
						u16 highResBit4 = highResBit3 + 1;

						holesHighResBits[highResBit1] = 1;
						holesHighResBits[highResBit2] = 1;
						holesHighResBits[highResBit3] = 1;
						holesHighResBits[highResBit4] = 1;
					}
				}

				cell.hole = holesHighResBits.to_ullong();
			}

			f32 cellBaseHeight = mcnk.position.z;

			for (u16 j = 0; j < Terrain::CELL_TOTAL_GRID_SIZE; j++)
			{
				f32 height = cellBaseHeight + cellInfo.mcvt.heightMap[j];

				cell.cellMinHeight = glm::min(cell.cellMinHeight, static_cast<f16>(height));
				cell.cellMaxHeight = glm::max(cell.cellMaxHeight, static_cast<f16>(height));
				cell.vertexData[j].height = height;

				out.heightHeader.gridMinHeight = glm::min(out.heightHeader.gridMinHeight, height);
				out.heightHeader.gridMaxHeight = glm::max(out.heightHeader.gridMaxHeight, height);

				// Normal data, this is swizzled read https://wowdev.wiki/ADT/v18#MCNR_sub-chunk
				i8 normalX = cellInfo.mcnr.normals[j].normal[0];
				i8 normalY = cellInfo.mcnr.normals[j].normal[1];
				i8 normalZ = cellInfo.mcnr.normals[j].normal[2];

				// Make sure to convert to u8 [0 .. 256]
				cell.vertexData[j].normal[0] = normalX + 127;
				cell.vertexData[j].normal[1] = normalY + 127;
				cell.vertexData[j].normal[2] = normalZ + 127;

				// Read Color Data (MCCV)
				cell.vertexData[j].color[0] = cellInfo.mccv.color[j].red;
				cell.vertexData[j].color[1] = cellInfo.mccv.color[j].green;
				cell.vertexData[j].color[2] = cellInfo.mccv.color[j].blue;
			}

			if (layout.mdid.data.size())
			{
				// Get the texture ID from the MCYL, this should be an index pointing into our textureNames vector

				for (u32 j = 0; j < cellInfo.mcly.data.size(); j++)
				{
					u32 fileID = layout.mdid.data[cellInfo.mcly.data[j].textureID];
					cell.layers[j].textureID = fileID;
				}
			}
		}

		// Read WMO Placement Data
		{
			u32 numWMOPlacements = static_cast<u32>(layout.modf.data.size());
			out.mapObjectPlacements.reserve(numWMOPlacements);

			for (u32 i = 0; i < numWMOPlacements; i++)
			{
				const MODF::PlacementInfo& placementInfo = layout.modf.data[i];

				if (!placementInfo.flags.EntryIsFiledataID || placementInfo.fileID == 0)
					continue;

				Terrain::Placement& placement = out.mapObjectPlacements.emplace_back();
				placement.uniqueID = placementInfo.uniqueID;
				placement.nameHash = placementInfo.fileID;
				placement.position = vec3(Terrain::MAP_HALF_SIZE - placementInfo.position.z, Terrain::MAP_HALF_SIZE - placementInfo.position.x, placementInfo.position.y);

				vec3 placementRotation = glm::radians(vec3(placementInfo.rotation.z, placementInfo.rotation.x, placementInfo.rotation.y + 180.f));
				glm::mat4 matrix = glm::eulerAngleZYX(placementRotation.z, placementRotation.y, placementRotation.x);
				placement.rotation = glm::quat_cast(matrix);

				bool hasScale = placementInfo.flags.HasScale;
				placement.scale = (placementInfo.scale * hasScale) + (1 * !hasScale);
			}
		}

		// Read M2 Placement Data
		{
			u32 numM2Placements = static_cast<u32>(layout.mddf.data.size());
			out.complexModelPlacements.reserve(numM2Placements);

			for (u32 i = 0; i < numM2Placements; i++)
			{
				const MDDF::PlacementInfo& placementInfo = layout.mddf.data[i];

				if (!placementInfo.flags.EntryIsFiledataID || placementInfo.fileID == 0)
					continue;

				Terrain::Placement& placement = out.complexModelPlacements.emplace_back();
				placement.uniqueID = placementInfo.uniqueID;
				placement.nameHash = placementInfo.fileID;
				placement.position = vec3(Terrain::MAP_HALF_SIZE - placementInfo.position.z, Terrain::MAP_HALF_SIZE - placementInfo.position.x, placementInfo.position.y);

				vec3 placementRotation = glm::radians(vec3(placementInfo.rotation.z, placementInfo.rotation.x, placementInfo.rotation.y + 180.f));
				glm::mat4 matrix = glm::eulerAngleZYX(placementRotation.z, placementRotation.y, placementRotation.x);
				placement.rotation = glm::quat_cast(matrix);
				placement.scale = placementInfo.scale;
			}
		}

		return true;
	}
}