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

		u32 numComplexModelPlacements = static_cast<u32>(complexModelPlacements.size());
		output.write(reinterpret_cast<char const*>(&numComplexModelPlacements), sizeof(u32)); // Write number of complex model placements

		// Placements
		{
			if (numMapObjectPlacements > 0)
			{
				output.write(reinterpret_cast<char const*>(mapObjectPlacements.data()), numMapObjectPlacements * sizeof(Terrain::Placement)); // Write map object placements
			}

			if (numComplexModelPlacements > 0)
			{
				output.write(reinterpret_cast<char const*>(complexModelPlacements.data()), numComplexModelPlacements * sizeof(Terrain::Placement)); // Write map object placements
			}
		}

		// Water
		{
			u32 numLiquidHeaders = static_cast<u32>(liquidInfo.headers.size());
			output.write(reinterpret_cast<char const*>(&numLiquidHeaders), sizeof(u32)); // Write number of liquid headers

			if (numLiquidHeaders > 0)
			{
				output.write(reinterpret_cast<char const*>(liquidInfo.headers.data()), numLiquidHeaders * sizeof(CellLiquidHeader)); // Write liquid headers
			}

			u32 numLiquidInstances = static_cast<u32>(liquidInfo.instances.size());
			output.write(reinterpret_cast<char const*>(&numLiquidInstances), sizeof(u32)); // Write number of liquid instances

			if (numLiquidInstances > 0)
			{
				output.write(reinterpret_cast<char const*>(liquidInfo.instances.data()), numLiquidInstances * sizeof(CellLiquidInstance)); // Write liquid instances
			}

			u32 numLiquidAttributes = static_cast<u32>(liquidInfo.attributes.size());
			output.write(reinterpret_cast<char const*>(&numLiquidAttributes), sizeof(u32)); // Write number of liquid attributes

			if (numLiquidAttributes > 0)
			{
				output.write(reinterpret_cast<char const*>(liquidInfo.attributes.data()), numLiquidAttributes * sizeof(CellLiquidAttributes)); // Write liquid attributes
			}

			u32 numLiquidBitmapDataBytes = static_cast<u32>(liquidInfo.bitmapData.size());
			output.write(reinterpret_cast<char const*>(&numLiquidBitmapDataBytes), sizeof(u32)); // Write number of liquid bitmap data bytes

			if (numLiquidBitmapDataBytes > 0)
			{
				output.write(reinterpret_cast<char const*>(liquidInfo.bitmapData.data()), numLiquidBitmapDataBytes * sizeof(u8)); // Write liquid bitmap data bytes
			}

			u32 numLiquidVertexDataBytes = static_cast<u32>(liquidInfo.vertexData.size());
			output.write(reinterpret_cast<char const*>(&numLiquidVertexDataBytes), sizeof(u32)); // Write number of liquid vertex data bytes

			if (numLiquidVertexDataBytes > 0)
			{
				output.write(reinterpret_cast<char const*>(liquidInfo.vertexData.data()), numLiquidVertexDataBytes * sizeof(u8)); // Write liquid vertex data bytes
			}
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

		if (!buffer->GetU32(out.numMapObjectPlacements))
			return false;

		if (!buffer->GetU32(out.numComplexModelPlacements))
			return false;

		// Read Map Object Placements
		{
			u32 numPlacements = out.numMapObjectPlacements;

			if (numPlacements)
			{
				out.mapObjectPlacements.resize(numPlacements);
				if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.mapObjectPlacements[0]), numPlacements * sizeof(Terrain::Placement)))
					return false;
			}
		}

		// Read CModel Placements
		{
			u32 numPlacements = out.numComplexModelPlacements;

			if (numPlacements)
			{
				out.complexModelPlacements.resize(numPlacements);
				if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.complexModelPlacements[0]), numPlacements * sizeof(Terrain::Placement)))
					return false;
			}
		}

		// Read Water
		{
			u32 numLiquidHeaders = 0;
			if (!buffer->GetU32(numLiquidHeaders))
				return false;

			if (numLiquidHeaders > 0)
			{
				out.liquidInfo.headers.resize(numLiquidHeaders);
				if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.liquidInfo.headers[0]), numLiquidHeaders * sizeof(CellLiquidHeader)))
					return false;
			}

			u32 numLiquidInstances = 0;
			if (!buffer->GetU32(numLiquidInstances))
				return false;

			if (numLiquidInstances > 0)
			{
				out.liquidInfo.instances.resize(numLiquidInstances);
				if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.liquidInfo.instances[0]), numLiquidInstances * sizeof(CellLiquidInstance)))
					return false;
			}

			u32 numLiquidAttributes = 0;
			if (!buffer->GetU32(numLiquidAttributes))
				return false;

			if (numLiquidAttributes > 0)
			{
				out.liquidInfo.attributes.resize(numLiquidAttributes);
				if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.liquidInfo.attributes[0]), numLiquidAttributes * sizeof(CellLiquidAttributes)))
					return false;
			}

			u32 numLiquidBitmapDataBytes = 0;
			if (!buffer->GetU32(numLiquidBitmapDataBytes))
				return false;

			if (numLiquidBitmapDataBytes > 0)
			{
				out.liquidInfo.bitmapData.resize(numLiquidBitmapDataBytes);
				if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.liquidInfo.bitmapData[0]), numLiquidBitmapDataBytes * sizeof(u8)))
					return false;
			}

			u32 numLiquidVertexDataBytes = 0;
			if (!buffer->GetU32(numLiquidVertexDataBytes))
				return false;

			if (numLiquidVertexDataBytes > 0)
			{
				out.liquidInfo.vertexData.resize(numLiquidVertexDataBytes);
				if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.liquidInfo.vertexData[0]), numLiquidVertexDataBytes * sizeof(u8)))
					return false;
			}
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

				vec3 centeredPosition = vec3(Terrain::MAP_HALF_SIZE - placementInfo.position.x, placementInfo.position.y, Terrain::MAP_HALF_SIZE - placementInfo.position.z);
				placement.position = CoordinateSpaces::PlacementPosToNovus(centeredPosition);

				vec3 placementRotation = glm::radians(CoordinateSpaces::PlacementRotToNovus(placementInfo.rotation));
				glm::mat4 matrix = glm::eulerAngleYXZ(placementRotation.y, placementRotation.x, placementRotation.z);
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

				vec3 centeredPosition = vec3(Terrain::MAP_HALF_SIZE - placementInfo.position.x, placementInfo.position.y, Terrain::MAP_HALF_SIZE - placementInfo.position.z);
				placement.position = CoordinateSpaces::PlacementPosToNovus(centeredPosition);

				vec3 placementRotation = glm::radians(CoordinateSpaces::PlacementRotToNovus(placementInfo.rotation));
				glm::mat4 matrix = glm::eulerAngleYXZ(placementRotation.y, placementRotation.x, placementRotation.z);
				placement.rotation = glm::quat_cast(matrix);
				placement.scale = placementInfo.scale;
			}
		}

		// Read Water Data
		{
			u32 numHeaders = static_cast<u32>(layout.mh2o.headers.size());
			u32 numInstances = static_cast<u32>(layout.mh2o.instances.size());
			u32 numAttributes = static_cast<u32>(layout.mh2o.attributes.size());

			out.liquidInfo.headers.resize(numHeaders);
			out.liquidInfo.instances.resize(numInstances);
			out.liquidInfo.attributes.resize(numAttributes);

			u32 attributeCounter = 0;
			u32 instanceCounter = 0;
			u32 bitmapOffset = 0;
			u32 vertexDataOffset = 0;

			for (u32 i = 0; i < numHeaders; i++)
			{
				const Adt::MH2O::LiquidHeader& header = layout.mh2o.headers[i];
				CellLiquidHeader& outHeader = out.liquidInfo.headers[i];

				bool hasAttributes = header.attributeOffset > 0;
				outHeader.packedData = header.layerCount | (hasAttributes << 7);

				if (hasAttributes)
				{
					u32 attributeIndex = attributeCounter++;

					const Adt::MH2O::LiquidAttribute& attribute = layout.mh2o.attributes[attributeIndex];
					CellLiquidAttributes& outAttribute = out.liquidInfo.attributes[attributeIndex];

					outAttribute.fishableBitmap = attribute.fishableBitmap;
					outAttribute.fatigueBitmap = attribute.fatigueBitmap;
				}

				if (header.layerCount == 0)
					continue;

				for (u32 j = 0; j < header.layerCount; j++)
				{
					u32 instanceIndex = instanceCounter++;

                    const Adt::MH2O::LiquidInstance& instance = layout.mh2o.instances[instanceIndex];
                    CellLiquidInstance& outInstance = out.liquidInfo.instances[instanceIndex];

					u16 liquidVertexFormat = instance.liquidVertexFormat;
					assert(liquidVertexFormat >= 0 && liquidVertexFormat <= 3);

					bool hasBitmapData = instance.bitmapDataOffset > 0;
					bool hasVertexData = instance.vertexDataOffset > 0 && liquidVertexFormat != 2;

					outInstance.liquidTypeID = static_cast<u8>(instance.liquidType);
					outInstance.packedData = static_cast<u8>(instance.liquidVertexFormat) | (hasBitmapData << 6 | hasVertexData << 7);
					outInstance.height = instance.heightLevel.x;
					outInstance.packedOffset = instance.offsetX | instance.offsetY << 4;
					outInstance.packedSize = instance.width | instance.height << 4;

					if (hasBitmapData)
					{
						u32 bitmapDataBeforeAdd = static_cast<u32>(out.liquidInfo.bitmapData.size());
						u32 bitmapDataToAdd = (instance.width * instance.height + 7) / 8;

						bool isOffsetWithinBufferRange = (bitmapOffset + bitmapDataToAdd) < static_cast<u32>(layout.mh2o.bitmapData.size());
						assert(isOffsetWithinBufferRange&& bitmapDataToAdd > 0);

						out.liquidInfo.bitmapData.resize(bitmapDataBeforeAdd + bitmapDataToAdd);
						memcpy(&out.liquidInfo.bitmapData[bitmapDataBeforeAdd], &layout.mh2o.bitmapData[bitmapOffset], bitmapDataToAdd);
						bitmapOffset += bitmapDataToAdd;
					}

					if (hasVertexData)
					{
						u32 numVertices = (instance.width + 1) * (instance.height + 1);
						u32 formatSize = ((liquidVertexFormat == 0) * sizeof(Adt::MH2O::LiquidVertexFormat_Height)) + ((liquidVertexFormat == 1 || liquidVertexFormat == 3) * sizeof(Adt::MH2O::LiquidVertexFormat_Height_UV));
						
						u32 vertexDataBeforeAdd = static_cast<u32>(out.liquidInfo.vertexData.size());
						u32 vertexDataToAdd = numVertices * formatSize;

						bool isOffsetWithinBufferRange = (vertexDataOffset + vertexDataToAdd) < static_cast<u32>(layout.mh2o.vertexData.capacity());
						assert(isOffsetWithinBufferRange && (numVertices > 0 && formatSize > 0));

						out.liquidInfo.vertexData.resize(vertexDataBeforeAdd + vertexDataToAdd);
						memcpy(&out.liquidInfo.vertexData[vertexDataBeforeAdd], &layout.mh2o.vertexData[vertexDataOffset], vertexDataToAdd);
						vertexDataOffset += vertexDataToAdd;
					}
                }
			}
		}

		return true;
	}
}