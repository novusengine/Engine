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
        std::ofstream output(path, std::ofstream::out | std::ofstream::binary);
        if (!output)
        {
            DebugHandler::PrintError("Failed to create Map Chunk file. Check admin permissions {0}", path);
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

        // Liquid
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

        // Physics
        {
            u32 numPhysicsBytes = static_cast<u32>(physicsData.size());
            output.write(reinterpret_cast<char const*>(&numPhysicsBytes), sizeof(u32)); // Write number of physics bytes

            if (numPhysicsBytes > 0)
            {
                output.write(reinterpret_cast<char const*>(physicsData.data()), numPhysicsBytes * sizeof(u8)); // Write physics bytes
            }
        }

        output.close();

        return true;
    }

    bool Chunk::Read(std::shared_ptr<Bytebuffer>& buffer, Chunk& out)
    {
        out.mapObjectPlacements.clear();
        out.complexModelPlacements.clear();

        out.liquidInfo.headers.clear();
        out.liquidInfo.instances.clear();
        out.liquidInfo.attributes.clear();
        out.liquidInfo.bitmapData.clear();
        out.liquidInfo.vertexData.clear();

        if (!buffer->Get(out.header))
            return false;

        if (out.header.type != FileHeader::Type::MapChunk || out.header.version != Chunk::CURRENT_VERSION)
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

        // Read Liquid
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

        // Read Physics
        {
            u32 numPhysicsBytes = 0;
            if (!buffer->GetU32(numPhysicsBytes))
                return false;

            if (numPhysicsBytes)
            {
                out.physicsData.resize(numPhysicsBytes);
                if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.physicsData[0]), numPhysicsBytes * sizeof(u8)))
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
                u64 holesHighRes = mcnk.holesHighResA | (static_cast<u64>(mcnk.holesHighResB) << 32);

                cell.hole = holesHighRes;
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

                cell.cellMinHeight = glm::min(cell.cellMinHeight, height);
                cell.cellMaxHeight = glm::max(cell.cellMaxHeight, height);
                cell.vertexData[j].height = height;

                out.heightHeader.gridMinHeight = glm::min(out.heightHeader.gridMinHeight, height);
                out.heightHeader.gridMaxHeight = glm::max(out.heightHeader.gridMaxHeight, height);

                // Normal data, this is swizzled read https://wowdev.wiki/ADT/v18#MCNR_sub-chunk
                i8 normalX = cellInfo.mcnr.normals[j].normal[0];
                i8 normalY = cellInfo.mcnr.normals[j].normal[1];
                i8 normalZ = cellInfo.mcnr.normals[j].normal[2];

                ivec3 normal = CoordinateSpaces::TerrainPosToNovus(vec3(normalX, normalY, normalZ));

                // Make sure to convert to u8 [0 .. 256]
                cell.vertexData[j].normal[0] = static_cast<u8>(normal.x) + 127;
                cell.vertexData[j].normal[1] = static_cast<u8>(normal.y) + 127;
                cell.vertexData[j].normal[2] = static_cast<u8>(normal.z) + 127;

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
                placement.doodadSet = placementInfo.doodadSet;

                vec3 centeredPosition = vec3(Terrain::MAP_HALF_SIZE - placementInfo.position.x, placementInfo.position.y, Terrain::MAP_HALF_SIZE - placementInfo.position.z);
                placement.position = CoordinateSpaces::PlacementPosToNovus(centeredPosition);

                vec3 placementRotation = glm::radians(CoordinateSpaces::PlacementRotToNovus(placementInfo.rotation));
                glm::mat4 matrix = glm::eulerAngleYXZ(placementRotation.y, placementRotation.x, placementRotation.z);
                placement.rotation = glm::quat_cast(matrix);

                bool hasScale = placementInfo.flags.HasScale;
                placement.scale = (placementInfo.scale * hasScale) + (1024 * !hasScale);
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

        // Read Liquid Data
        {
            u32 numHeaders = static_cast<u32>(layout.mh2o.headers.size());
            u32 numInstances = static_cast<u32>(layout.mh2o.instances.size());
            u32 numAttributes = static_cast<u32>(layout.mh2o.attributes.size());
            u32 numBitmapDataBytes = static_cast<u32>(layout.mh2o.bitmapData.size());
            u32 numVertexDataBytes = static_cast<u32>(layout.mh2o.vertexData.size());

            out.liquidInfo.headers.resize(numHeaders);
            out.liquidInfo.instances.resize(numInstances);
            out.liquidInfo.attributes.resize(numAttributes);

            if (numBitmapDataBytes)
            {
                out.liquidInfo.bitmapData.resize(numBitmapDataBytes);
                memcpy(out.liquidInfo.bitmapData.data(), layout.mh2o.bitmapData.data(), numBitmapDataBytes);
            }

            if (numVertexDataBytes)
            {
                out.liquidInfo.vertexData.resize(numVertexDataBytes);
                memcpy(out.liquidInfo.vertexData.data(), layout.mh2o.vertexData.data(), numVertexDataBytes);
            }

            u32 attributeCounter = 0;

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
            }

            for (u32 i = 0; i < numInstances; i++)
            {
                const Adt::MH2O::LiquidInstance& instance = layout.mh2o.instances[i];
                CellLiquidInstance& outInstance = out.liquidInfo.instances[i];

                bool hasBitmapData = instance.bitmapDataOffset != std::numeric_limits<u32>().max();
                bool hasVertexData = instance.vertexDataOffset != std::numeric_limits<u32>().max();

                outInstance.liquidTypeID = static_cast<u8>(instance.liquidType);
                outInstance.packedData = static_cast<u8>(instance.liquidVertexFormat) | (hasBitmapData << 6 | hasVertexData << 7);
                outInstance.height = instance.heightLevel.x;
                outInstance.packedOffset = instance.offsetX | instance.offsetY << 4;
                outInstance.packedSize = instance.width | instance.height << 4;

                outInstance.bitmapDataOffset = instance.bitmapDataOffset;
                outInstance.vertexDataOffset = instance.vertexDataOffset;
            }
        }

        return true;
    }
}
