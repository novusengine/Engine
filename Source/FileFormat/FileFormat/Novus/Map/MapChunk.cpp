#include "MapChunk.h"
#include "FileFormat/Warcraft/ADT/Adt.h"

#include <Base/Util/DebugHandler.h>

#include <glm/gtx/euler_angles.hpp>
#include <fstream>
#include <bitset>

using namespace Adt;

namespace Map
{
    bool Chunk::Save(const std::string& path, const std::vector<Terrain::Placement>& modelPlacements, const LiquidInfo& liquidInfo, const std::vector<u8>& physicsData)
    {
        std::ofstream output(path, std::ofstream::out | std::ofstream::binary);
        if (!output)
        {
            NC_LOG_ERROR("Failed to create Map Chunk file. Check admin permissions {0}", path);
            return false;
        }

        // Write the Chunk to file
        output.write(reinterpret_cast<char const*>(&header), sizeof(header));
        output.write(reinterpret_cast<char const*>(&heightHeader), sizeof(heightHeader));
        output.write(reinterpret_cast<char const*>(&heightBox), sizeof(heightBox));
        output.write(reinterpret_cast<char const*>(&cellsData), sizeof(cellsData));

        output.write(reinterpret_cast<char const*>(&chunkAlphaMapTextureHash), sizeof(u64)); // Write alpha map string index

        // Headers
        {
            placementHeader.numPlacements = static_cast<u32>(modelPlacements.size());
            liquidHeader.numHeaders = static_cast<u32>(liquidInfo.headers.size());

            bool hasHeaders = liquidHeader.numHeaders > 0;
            liquidHeader.numInstances = static_cast<u32>(liquidInfo.instances.size()) * hasHeaders;
            liquidHeader.numAttributes = static_cast<u32>(liquidInfo.attributes.size()) * hasHeaders;
            liquidHeader.numBitmapBytes = static_cast<u32>(liquidInfo.bitmapData.size()) * hasHeaders;
            liquidHeader.numVertexBytes = static_cast<u32>(liquidInfo.vertexData.size()) * hasHeaders;
            physicsHeader.numBytes = static_cast<u32>(physicsData.size());

            u32 numPlacementBytes = placementHeader.numPlacements * sizeof(Terrain::Placement);
            u32 numLiquidBytes = liquidHeader.numHeaders * sizeof(CellLiquidHeader) +
                liquidHeader.numInstances * sizeof(CellLiquidInstance) +
                liquidHeader.numAttributes * sizeof(CellLiquidAttributes) +
                liquidHeader.numBitmapBytes * sizeof(u8) +
                liquidHeader.numVertexBytes * sizeof(u8);

            placementHeader.dataOffset = static_cast<u64>(output.tellp()) + sizeof(placementHeader) + sizeof(liquidHeader) + sizeof(physicsHeader);
            liquidHeader.dataOffset = placementHeader.dataOffset + numPlacementBytes;
            physicsHeader.dataOffset = liquidHeader.dataOffset + numLiquidBytes;

            output.write(reinterpret_cast<char const*>(&placementHeader), sizeof(Map::Chunk::PlacementHeader)); // Write number of model placements
            output.write(reinterpret_cast<char const*>(&liquidHeader), sizeof(Map::Chunk::LiquidHeader)); // Write liquid header
            output.write(reinterpret_cast<char const*>(&physicsHeader), sizeof(Map::Chunk::PhysicsHeader)); // Write physics header

            // Placements
            if (placementHeader.numPlacements > 0)
            {
                output.write(reinterpret_cast<char const*>(modelPlacements.data()), placementHeader.numPlacements * sizeof(Terrain::Placement)); // Write map object placements
            }

            // Liquid
            if (liquidHeader.numHeaders > 0)
            {
                output.write(reinterpret_cast<char const*>(liquidInfo.headers.data()), liquidHeader.numHeaders * sizeof(CellLiquidHeader)); // Write liquid headers
            }

            if (liquidHeader.numInstances > 0)
            {
                output.write(reinterpret_cast<char const*>(liquidInfo.instances.data()), liquidHeader.numInstances * sizeof(CellLiquidInstance)); // Write liquid instances
            }

            if (liquidHeader.numAttributes > 0)
            {
                output.write(reinterpret_cast<char const*>(liquidInfo.attributes.data()), liquidHeader.numAttributes * sizeof(CellLiquidAttributes)); // Write liquid attributes
            }

            if (liquidHeader.numBitmapBytes > 0)
            {
                output.write(reinterpret_cast<char const*>(liquidInfo.bitmapData.data()), liquidHeader.numBitmapBytes * sizeof(u8)); // Write liquid bitmap data bytes
            }

            if (liquidHeader.numVertexBytes > 0)
            {
                output.write(reinterpret_cast<char const*>(liquidInfo.vertexData.data()), liquidHeader.numVertexBytes * sizeof(u8)); // Write liquid vertex data bytes
            }

            // Physics
            if (physicsHeader.numBytes > 0)
            {
                output.write(reinterpret_cast<char const*>(physicsData.data()), physicsHeader.numBytes * sizeof(u8)); // Write physics bytes
            }
        }

        output.close();

        return true;
    }

    bool Chunk::Read(std::shared_ptr<Bytebuffer>& buffer, Chunk& out)
    {
        bool failed = false;
        failed |= !buffer->Get(out.header);
        failed |= !buffer->Get(out.heightHeader);
        failed |= !buffer->Get(out.heightBox);
        failed |= !buffer->Get(out.cellsData);
        failed |= !buffer->Get(out.chunkAlphaMapTextureHash);
        failed |= !buffer->Get(out.placementHeader);
        failed |= !buffer->Get(out.liquidHeader);
        failed |= !buffer->Get(out.physicsHeader);

        if (failed || out.header.type != FileHeader::Type::MapChunk || out.header.version != Chunk::CURRENT_VERSION)
            return false;

        return true;
    }

    bool Chunk::FromADT(Adt::Layout& layout, Chunk& out, std::vector<Terrain::Placement>& modelPlacements, LiquidInfo& liquidInfo)
    {
        // TODO : Set HeightBox

        for (u16 i = 0; i < Terrain::CHUNK_NUM_CELLS; i++)
        {
            const CellInfo& cellInfo = layout.cellInfos[i];
            const MCNK& mcnk = cellInfo.mcnk;

            u16 cellIndex = i;

            out.cellsData.areaIDs[cellIndex] = mcnk.areaId;

            if (mcnk.flags.HighResHoles)
            {
                u64 holesHighRes = mcnk.holesHighResA | (static_cast<u64>(mcnk.holesHighResB) << 32);

                out.cellsData.holes[cellIndex] = holesHighRes;
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

                out.cellsData.holes[cellIndex] = holesHighResBits.to_ullong();
            }

            f32 cellBaseHeight = mcnk.position.z;

            vec2& currentHeightBounds = out.cellsData.heightBounds[cellIndex];
            currentHeightBounds = vec2(10000.0f, -10000.0f);

            for (u16 j = 0; j < Terrain::CELL_TOTAL_GRID_SIZE; j++)
            {
                f32 height = cellBaseHeight + cellInfo.mcvt.heightMap[j];
                currentHeightBounds.x = glm::min(currentHeightBounds.x, height);
                currentHeightBounds.y = glm::max(currentHeightBounds.y, height);

                out.cellsData.heightField[cellIndex][j] = height;

                out.heightHeader.gridMinHeight = glm::min(out.heightHeader.gridMinHeight, height);
                out.heightHeader.gridMaxHeight = glm::max(out.heightHeader.gridMaxHeight, height);

                // Normal data, this is swizzled read https://wowdev.wiki/ADT/v18#MCNR_sub-chunk
                i8 normalX = cellInfo.mcnr.normals[j].normal[0];
                i8 normalY = cellInfo.mcnr.normals[j].normal[1];
                i8 normalZ = cellInfo.mcnr.normals[j].normal[2];

                ivec3 normal = CoordinateSpaces::TerrainPosToNovus(vec3(normalX, normalY, normalZ));

                // Make sure to convert to u8 [0 .. 256]
                out.cellsData.normals[cellIndex][j][0] = static_cast<u8>(normal.x) + 127;
                out.cellsData.normals[cellIndex][j][1] = static_cast<u8>(normal.y) + 127;
                out.cellsData.normals[cellIndex][j][2] = static_cast<u8>(normal.z) + 127;

                // Read Color Data (MCCV)
                out.cellsData.colors[cellIndex][j][0] = cellInfo.mccv.color[j].red;
                out.cellsData.colors[cellIndex][j][1] = cellInfo.mccv.color[j].green;
                out.cellsData.colors[cellIndex][j][2] = cellInfo.mccv.color[j].blue;
            }

            if (layout.mdid.data.size())
            {
                // Get the texture ID from the MCYL, this should be an index pointing into our textureNames vector

                for (u32 j = 0; j < cellInfo.mcly.data.size(); j++)
                {
                    u32 fileID = layout.mdid.data[cellInfo.mcly.data[j].textureID];
                    out.cellsData.layerTextureIDs[cellIndex][j] = fileID;
                }
            }
        }

        // Read WMO Placement Data
        {
            u32 numWMOPlacements = static_cast<u32>(layout.modf.data.size());
            modelPlacements.reserve(numWMOPlacements);

            for (u32 i = 0; i < numWMOPlacements; i++)
            {
                const MODF::PlacementInfo& placementInfo = layout.modf.data[i];

                if (!placementInfo.flags.EntryIsFiledataID || placementInfo.fileID == 0)
                    continue;

                Terrain::Placement& placement = modelPlacements.emplace_back();
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
            modelPlacements.reserve(modelPlacements.size() + numM2Placements);

            for (u32 i = 0; i < numM2Placements; i++)
            {
                const MDDF::PlacementInfo& placementInfo = layout.mddf.data[i];

                if (!placementInfo.flags.EntryIsFiledataID || placementInfo.fileID == 0)
                    continue;

                Terrain::Placement& placement = modelPlacements.emplace_back();
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
            if (numHeaders > 0)
            {
                u32 numInstances = static_cast<u32>(layout.mh2o.instances.size());
                u32 numAttributes = static_cast<u32>(layout.mh2o.attributes.size());
                u32 numBitmapDataBytes = static_cast<u32>(layout.mh2o.bitmapData.size());
                u32 numVertexDataBytes = static_cast<u32>(layout.mh2o.vertexData.size());

                liquidInfo.headers.resize(numHeaders);
                liquidInfo.instances.resize(numInstances);
                liquidInfo.attributes.resize(numAttributes);

                if (numBitmapDataBytes)
                {
                    liquidInfo.bitmapData.resize(numBitmapDataBytes);
                    memcpy(liquidInfo.bitmapData.data(), layout.mh2o.bitmapData.data(), numBitmapDataBytes);
                }

                if (numVertexDataBytes)
                {
                    liquidInfo.vertexData.resize(numVertexDataBytes);
                    memcpy(liquidInfo.vertexData.data(), layout.mh2o.vertexData.data(), numVertexDataBytes);
                }

                u32 attributeCounter = 0;

                for (u32 i = 0; i < numHeaders; i++)
                {
                    const Adt::MH2O::LiquidHeader& header = layout.mh2o.headers[i];
                    CellLiquidHeader& outHeader = liquidInfo.headers[i];

                    bool hasAttributes = header.attributeOffset > 0;
                    outHeader.packedData = header.layerCount | (hasAttributes << 7);

                    if (hasAttributes)
                    {
                        u32 attributeIndex = attributeCounter++;

                        const Adt::MH2O::LiquidAttribute& attribute = layout.mh2o.attributes[attributeIndex];
                        CellLiquidAttributes& outAttribute = liquidInfo.attributes[attributeIndex];

                        outAttribute.fishableBitmap = attribute.fishableBitmap;
                        outAttribute.fatigueBitmap = attribute.fatigueBitmap;
                    }
                }

                for (u32 i = 0; i < numInstances; i++)
                {
                    const Adt::MH2O::LiquidInstance& instance = layout.mh2o.instances[i];
                    CellLiquidInstance& outInstance = liquidInfo.instances[i];

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
        }

        return true;
    }
}
