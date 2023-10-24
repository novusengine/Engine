#include "MapObject.h"
#include "FileFormat/Shared.h"
#include "FileFormat/Warcraft/WMO/Wmo.h"

#include <Base/Util/DebugHandler.h>

#include <glm/gtx/euler_angles.hpp>
#include <fstream>

using namespace Wmo;

namespace Model
{
    bool MapObject::Save(const std::string& path)
    {
        // Create a file
        std::ofstream output(path, std::ofstream::out | std::ofstream::binary);
        if (!output)
        {
            DebugHandler::PrintError("Failed to create Map Object file. Check admin permissions");
            return false;
        }

        // Write Header
        output.write(reinterpret_cast<char const*>(&header), sizeof(header));

        // Write Materials
        {
            u32 numMaterials = static_cast<u32>(materials.size());
            output.write(reinterpret_cast<char const*>(&numMaterials), sizeof(u32));

            if (numMaterials)
                output.write(reinterpret_cast<char const*>(&materials[0]), numMaterials * sizeof(Material));
        }

        // Write Decorations
        {
            u32 numDecorations = static_cast<u32>(decorations.size());
            output.write(reinterpret_cast<char const*>(&numDecorations), sizeof(u32));

            if (numDecorations)
                output.write(reinterpret_cast<char const*>(&decorations[0]), numDecorations * sizeof(Decoration));
        }

        // Write Decoration Sets
        {
            u32 numDecorationSets = static_cast<u32>(decorationSets.size());
            output.write(reinterpret_cast<char const*>(&numDecorationSets), sizeof(u32));

            if (numDecorationSets)
                output.write(reinterpret_cast<char const*>(&decorationSets[0]), numDecorationSets * sizeof(DecorationSet));
        }

        // Write Groups
        {
            u32 numGroups = static_cast<u32>(groups.size());
            output.write(reinterpret_cast<char const*>(&numGroups), sizeof(u32));

            for (u32 i = 0; i < numGroups; i++)
            {
                const MapObjectGroup& group = groups[i];

                // Write Header
                {
                    output.write(reinterpret_cast<char const*>(&group.header), sizeof(group.header));
                    output.write(reinterpret_cast<char const*>(&group.flags), sizeof(group.flags));
                }

                // Write Vertices
                {
                    u32 numVertices = static_cast<u32>(group.vertices.size());
                    output.write(reinterpret_cast<char const*>(&numVertices), sizeof(u32));

                    if (numVertices)
                        output.write(reinterpret_cast<char const*>(&group.vertices[0]), numVertices * sizeof(MapObjectGroup::Vertex));
                }

                // Write Indices
                {
                    u32 numIndices = static_cast<u32>(group.indices.size());
                    output.write(reinterpret_cast<char const*>(&numIndices), sizeof(u32));

                    if (numIndices)
                        output.write(reinterpret_cast<char const*>(&group.indices[0]), numIndices * sizeof(u16));
                }

                // Write Vertex Color Sets
                {
                    u32 numVertexColorSets = static_cast<u32>(group.vertexColorSets.size());
                    output.write(reinterpret_cast<char const*>(&numVertexColorSets), sizeof(u32));

                    for (u32 j = 0; j < numVertexColorSets; j++)
                    {
                        u32 numVertexColors = static_cast<u32>(group.vertexColorSets[j].colors.size());
                        output.write(reinterpret_cast<char const*>(&numVertexColors), sizeof(u32));

                        if (numVertexColors)
                            output.write(reinterpret_cast<char const*>(&group.vertexColorSets[j].colors[0]), numVertexColors * sizeof(IntColor));
                    }
                }

                // Write Triangle Material Infos
                {
                    u32 numTriangleMaterialInfos = static_cast<u32>(group.triangleMaterialInfo.size());
                    output.write(reinterpret_cast<char const*>(&numTriangleMaterialInfos), sizeof(u32));

                    if (numTriangleMaterialInfos)
                        output.write(reinterpret_cast<char const*>(&group.triangleMaterialInfo[0]), numTriangleMaterialInfos * sizeof(MapObjectGroup::TriangleMaterialInfo));
                }

                // Write Render Batches
                {
                    u32 numRenderBatches = static_cast<u32>(group.renderBatches.size());
                    output.write(reinterpret_cast<char const*>(&numRenderBatches), sizeof(u32));

                    if (numRenderBatches)
                        output.write(reinterpret_cast<char const*>(&group.renderBatches[0]), numRenderBatches * sizeof(MapObjectGroup::RenderBatch));
                }

                // Write Culling Datas
                {
                    u32 numCullingData = static_cast<u32>(group.cullingData.size());
                    output.write(reinterpret_cast<char const*>(&numCullingData), sizeof(u32));

                    if (numCullingData)
                        output.write(reinterpret_cast<char const*>(&group.cullingData[0]), numCullingData * sizeof(MapObjectGroup::CullingData));
                }
            }
        }

        return true;
    }

    bool MapObject::Read(std::shared_ptr<Bytebuffer>& buffer, MapObject& out)
    {
        if (!buffer->Get(out.header))
            return false;

        if (out.header.type != FileHeader::Type::MapObject || out.header.version != MapObject::CURRENT_VERSION)
            return false;

        // Read Material
        {
            u32 numMaterials = 0;
            if (!buffer->GetU32(numMaterials))
                return false;

            if (numMaterials)
            {
                out.materials.resize(numMaterials);
                if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.materials[0]), numMaterials * sizeof(Material)))
                    return false;
            }
        }

        // Read Decorations
        {
            u32 numDecorations = 0;
            if (!buffer->GetU32(numDecorations))
                return false;

            if (numDecorations)
            {
                out.decorations.resize(numDecorations);
                if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.decorations[0]), numDecorations * sizeof(Decoration)))
                    return false;
            }
        }

        // Read Decoration Sets
        {
            u32 numDecorationSets = 0;
            if (!buffer->GetU32(numDecorationSets))
                return false;

            if (numDecorationSets)
            {
                out.decorationSets.resize(numDecorationSets);
                if (!buffer->GetBytes(reinterpret_cast<u8*>(&out.decorationSets[0]), numDecorationSets * sizeof(DecorationSet)))
                    return false;
            }
        }

        // Read Groups
        {
            u32 numGroups = 0;
            if (!buffer->GetU32(numGroups))
                return false;

            if (numGroups)
            {
                out.groups.resize(numGroups);
                for (u32 i = 0; i < numGroups; i++)
                {
                    MapObjectGroup& group = out.groups[i];

                    // Read Header
                    {
                        if (!buffer->Get(group.header))
                            return false;

                        if (group.header.type != FileHeader::Type::MapObjectGroup || group.header.version != MapObjectGroup::CURRENT_VERSION)
                            return false;

                        if (!buffer->Get(group.flags))
                            return false;
                    }

                    // Read Vertices
                    {
                        u32 numVertices = 0;
                        if (!buffer->GetU32(numVertices))
                            return false;

                        if (numVertices)
                        {
                            group.vertices.resize(numVertices);
                            if (!buffer->GetBytes(reinterpret_cast<u8*>(&group.vertices[0]), numVertices * sizeof(MapObjectGroup::Vertex)))
                                return false;
                        }
                    }

                    // Read Indices
                    {
                        u32 numIndices = 0;
                        if (!buffer->GetU32(numIndices))
                            return false;

                        if (numIndices)
                        {
                            group.indices.resize(numIndices);
                            if (!buffer->GetBytes(reinterpret_cast<u8*>(&group.indices[0]), numIndices * sizeof(u16)))
                                return false;
                        }
                    }

                    // Read Vertex Color Sets
                    {
                        u32 numVertexColorSets = 0;
                        if (!buffer->GetU32(numVertexColorSets))
                            return false;

                        if (numVertexColorSets)
                        {
                            group.vertexColorSets.resize(numVertexColorSets);

                            for (u32 j = 0; j < numVertexColorSets; j++)
                            {
                                u32 numVertexColors = 0;
                                if (!buffer->GetU32(numVertexColors))
                                    return false;

                                group.vertexColorSets[j].colors.resize(numVertexColors);
                                if (!buffer->GetBytes(reinterpret_cast<u8*>(&group.vertexColorSets[j].colors[0]), numVertexColors * sizeof(IntColor)))
                                    return false;
                            }
                        }
                    }

                    // Read Triangle Material Infos
                    {
                        u32 numTriangleMaterialInfos = 0;
                        if (!buffer->GetU32(numTriangleMaterialInfos))
                            return false;

                        if (numTriangleMaterialInfos)
                        {
                            group.triangleMaterialInfo.resize(numTriangleMaterialInfos);
                            if (!buffer->GetBytes(reinterpret_cast<u8*>(&group.triangleMaterialInfo[0]), numTriangleMaterialInfos * sizeof(MapObjectGroup::TriangleMaterialInfo)))
                                return false;
                        }
                    }

                    // Read Render Batches
                    {
                        u32 numRenderBatches = 0;
                        if (!buffer->GetU32(numRenderBatches))
                            return false;

                        if (numRenderBatches)
                        {
                            group.renderBatches.resize(numRenderBatches);
                            if (!buffer->GetBytes(reinterpret_cast<u8*>(&group.renderBatches[0]), numRenderBatches * sizeof(MapObjectGroup::RenderBatch)))
                                return false;
                        }
                    }

                    // Read Culling Datas
                    {
                        u32 numCullingData = 0;
                        if (!buffer->GetU32(numCullingData))
                            return false;

                        if (numCullingData)
                        {
                            group.cullingData.resize(numCullingData);
                            if (!buffer->GetBytes(reinterpret_cast<u8*>(&group.cullingData[0]), numCullingData * sizeof(MapObjectGroup::CullingData)))
                                return false;
                        }
                    }
                }
            }
        }

        return true;
    }

	bool MapObject::FromWMO(Wmo::Layout& layout, MapObject& out)
	{
        // Convert Materials
        {
            out.materials.resize(layout.momt.data.size());

            for (u32 i = 0; i < layout.momt.data.size(); i++)
            {
                const MOMT::Material& wmoMaterial = layout.momt.data[i];
                MapObject::Material& mapObjectMaterial = out.materials[i];

                mapObjectMaterial.materialType = wmoMaterial.shaderIndex;
                mapObjectMaterial.blendMode = wmoMaterial.blendMode;
                mapObjectMaterial.flags = *reinterpret_cast<const MapObject::Material::Flags*>(&wmoMaterial.flags);

                if (wmoMaterial.texture1FileID)
                    mapObjectMaterial.textureID[0] = wmoMaterial.texture1FileID;

                if (wmoMaterial.texture2FileID)
                    mapObjectMaterial.textureID[1] = wmoMaterial.texture2FileID;

                if (wmoMaterial.texture3FileID)
                    mapObjectMaterial.textureID[2] = wmoMaterial.texture3FileID;
            }
        }

        // Convert Decorations here
        {
            u32 numDecorations = static_cast<u32>(layout.modd.data.size());
            out.decorations.resize(numDecorations);

            for (u32 i = 0; i < numDecorations; i++)
            {
                MODD::PlacementInfo& doodadPlacementInfo = layout.modd.data[i];
                MapObject::Decoration& decoration = out.decorations[i];

                u32 fileID = layout.modi.data[doodadPlacementInfo.flags.MODIIndex].fileID;
                decoration.nameID = fileID;
                decoration.position = CoordinateSpaces::ModelPosToNovus(doodadPlacementInfo.position);

                vec3 eulerAngles = glm::eulerAngles(doodadPlacementInfo.rotation);
                vec3 placementAngles = CoordinateSpaces::DecorationRotToNovus(eulerAngles);
                glm::mat4 matrix = glm::eulerAngleYXZ(placementAngles.y, placementAngles.x, placementAngles.z);

                decoration.rotation = glm::quat_cast(matrix);
                decoration.scale = doodadPlacementInfo.scale;
                decoration.color = doodadPlacementInfo.color.bgra;
            }
        }

        // Convert Decorations Set here
        {
            u32 numDoodadSets = static_cast<u32>(layout.mods.data.size());
            out.decorationSets.resize(numDoodadSets);

            for (u32 i = 0; i < numDoodadSets; i++)
            {
                MODS::DoodadSet& doodadSet = layout.mods.data[i];
                MapObject::DecorationSet& decorationSet = out.decorationSets[i];

                memcpy(&decorationSet.name[0], &doodadSet.name[0], 20);
                decorationSet.index = doodadSet.startIndex;
                decorationSet.count = doodadSet.count;
            }
        }

        // Convert Groups
        {
            u32 numGroups = static_cast<u32>(layout.groups.size());
            out.groups.resize(numGroups);

            for (u32 i = 0; i < numGroups; i++)
            {
                const WMOGroup& wmoGroup = layout.groups[i];
                MapObjectGroup& mapObjectGroup = out.groups[i];

                mapObjectGroup.flags = *reinterpret_cast<const MapObjectGroup::Flags*>(&wmoGroup.mogp.flags);

                // Check if we need to fix Vertex Colors
                {
                    u32 numMOCVs = static_cast<u32>(wmoGroup.mocvs.size());
                    mapObjectGroup.vertexColorSets.resize(numMOCVs);

                    for (u32 j = 0; j < numMOCVs; j++)
                    {
                        const MOCV& vertexColors = wmoGroup.mocvs[j];
                        MapObjectGroup::VertexColorSet& vertexColorSet = mapObjectGroup.vertexColorSets[j];

                        vertexColorSet.colors.resize(vertexColors.data.size());
                        memcpy(vertexColorSet.colors.data(), vertexColors.data.data(), vertexColors.data.size() * sizeof(IntColor));
                    }

                    if (wmoGroup.mogp.flags.HasVertexColors)
                    {
                        MapObjectGroup::VertexColorSet& vertexColorSet = mapObjectGroup.vertexColorSets[0];

                        u32 numVertexColors = static_cast<u32>(vertexColorSet.colors.size());
                        if (numVertexColors)
                        {
                            u32 endOfTransitionBatch = 0;

                            if (wmoGroup.mogp.transitionBatchCount)
                                endOfTransitionBatch = wmoGroup.moba.data[wmoGroup.mogp.transitionBatchCount - 1u].vertexEndOffset + 1;

                            if (layout.mohd.flags.DoNotFixVertexColorAlpha)
                            {
                                for (u32 j = endOfTransitionBatch; j < numVertexColors; j++)
                                {
                                    vertexColorSet.colors[j].a = wmoGroup.mogp.flags.IsExterior ? 255 : 0;
                                }
                            }
                            else
                            {
                                IntColor baseColor = { };
                                if (layout.mohd.flags.UseUnifiedRenderPath)
                                    baseColor.bgra = 0;
                                else
                                    baseColor = layout.mohd.ambientColor;

                                for (u32 j = 0; j < endOfTransitionBatch; j++)
                                {
                                    IntColor& color = vertexColorSet.colors[j];

                                    color.r -= baseColor.r;
                                    color.g -= baseColor.g;
                                    color.b -= baseColor.b;

                                    f32 alphaFactor = color.a / 255.0f;

                                    f32 redChannelModifier = color.r - alphaFactor * color.r;
                                    color.r = static_cast<u8>(glm::max(0.0f, glm::floor(redChannelModifier / 2.0f)));

                                    f32 greenChannelModifier = color.g - alphaFactor * color.g;
                                    color.g = static_cast<u8>(glm::max(0.0f, glm::floor(greenChannelModifier / 2.0f)));

                                    f32 blueChannelModifier = color.b - alphaFactor * color.b;
                                    color.b = static_cast<u8>(glm::max(0.0f, glm::floor(blueChannelModifier / 2.0f)));
                                }

                                for (u32 j = endOfTransitionBatch; j < numVertexColors; j++)
                                {
                                    IntColor& color = vertexColorSet.colors[j];

                                    f32 redChannelModifier = (color.r * color.a) / 64.0f + color.r - baseColor.r;
                                    color.r = static_cast<u8>(glm::min(255.0f, glm::max(0.0f, redChannelModifier / 2.0f)));

                                    f32 greenChannelModifier = (color.g * color.a) / 64.0f + color.g - baseColor.g;
                                    color.g = static_cast<u8>(glm::min(255.0f, glm::max(0.0f, greenChannelModifier / 2.0f)));

                                    f32 blueChannelModifier = (color.b * color.a) / 64.0f + color.b - baseColor.b;
                                    color.b = static_cast<u8>(glm::min(255.0f, glm::max(0.0f, blueChannelModifier / 2.0f)));

                                    // Default is 0, set to 255 if group is exterior
                                    color.a = (255 * wmoGroup.mogp.flags.IsExterior);
                                }
                            }
                        }
                    }
                }

                // Convert Render Batches
                {
                    u32 numRenderBatches = static_cast<u32>(wmoGroup.moba.data.size());
                    mapObjectGroup.renderBatches.resize(numRenderBatches);
                    mapObjectGroup.cullingData.resize(numRenderBatches);

                    for (u32 renderBatchIndex = 0; renderBatchIndex < numRenderBatches; renderBatchIndex++)
                    {
                        const MOBA::RenderBatch& wmoRenderBatch = wmoGroup.moba.data[renderBatchIndex];

                        MapObjectGroup::RenderBatch& renderBatch = mapObjectGroup.renderBatches[renderBatchIndex];
                        MapObjectGroup::CullingData& cullingData = mapObjectGroup.cullingData[renderBatchIndex];

                        renderBatch.startVertex = wmoRenderBatch.vertexStartOffset;
                        renderBatch.vertexCount = wmoRenderBatch.vertexEndOffset - wmoRenderBatch.vertexStartOffset;
                        renderBatch.startIndex = wmoRenderBatch.startIndex;
                        renderBatch.indexCount = wmoRenderBatch.indexCount;
                        renderBatch.materialID = (wmoRenderBatch.materialIDLarge * wmoRenderBatch.flags.UseMaterialIDLarge) + (wmoRenderBatch.materialIDSmall * !wmoRenderBatch.flags.UseMaterialIDLarge);

                        vec3 aabbMin = vec3(10000.0f, 10000.0f, 10000.0f);
                        vec3 aabbMax = vec3(-10000.0f, -10000.0f, -10000.0f);

                        const u32 start = renderBatch.startIndex;
                        const u32 end = start + renderBatch.indexCount;

                        for (u32 j = start; j < end; j++)
                        {
                            const u16 index = wmoGroup.movi.data[j];
                            const vec3& position = CoordinateSpaces::ModelPosToNovus(wmoGroup.movt.data[index].position);

                            for (u32 vecIndex = 0; vecIndex < 3; vecIndex++)
                            {
                                bool isSmaller = position[vecIndex] < aabbMin[vecIndex];
                                aabbMin[vecIndex] = (position[vecIndex] * isSmaller) + (aabbMin[vecIndex] * !isSmaller);

                                bool isLarger = position[vecIndex] > aabbMax[vecIndex];
                                aabbMax[vecIndex] = (position[vecIndex] * isLarger) + (aabbMax[vecIndex] * !isLarger);
                            }
                        }

                        cullingData.center = (aabbMin + aabbMax) * 0.5f;
                        cullingData.extents = hvec3(aabbMax) - cullingData.center;
                        cullingData.boundingSphereRadius = glm::distance(aabbMin, aabbMax) / 2.0f;
                    }
                }

                // Convert Vertex Data
                {
                    u32 numVertices = static_cast<u32>(wmoGroup.movt.data.size());
                    u32 numUVSets = static_cast<u32>(wmoGroup.motvs.size());

                    // Copy Indices
                    {
                        u32 numIndices = static_cast<u32>(wmoGroup.movi.data.size());
                        mapObjectGroup.indices.resize(numIndices);
                        memcpy(mapObjectGroup.indices.data(), wmoGroup.movi.data.data(), wmoGroup.movi.data.size() * sizeof(u16));
                    }

                    // Copy Triangle Material Info
                    {
                        u32 numTriangleMaterialInfo = static_cast<u32>(wmoGroup.mopy.data.size());
                        mapObjectGroup.triangleMaterialInfo.resize(numTriangleMaterialInfo);
                        memcpy(mapObjectGroup.triangleMaterialInfo.data(), wmoGroup.mopy.data.data(), wmoGroup.mopy.data.size() * sizeof(MOPY::TriangleMaterialInfo));
                    }

                    mapObjectGroup.vertices.resize(numVertices);
                    for (u32 j = 0; j < numVertices; j++)
                    {
                        MapObjectGroup::Vertex& vertex = mapObjectGroup.vertices[j];

                        // Position
                        const vec3& pos = wmoGroup.movt.data[j].position;
                        vertex.position = CoordinateSpaces::ModelPosToNovus(pos);

                        // Normal
                        vec3 normal = CoordinateSpaces::ModelPosToNovus(glm::normalize(wmoGroup.monr.data[j].normal));
                        vec2 octNormal = OctNormalEncode(normal);

                        vertex.octNormal[0] = static_cast<u8>(glm::round(octNormal.x * 255.0f));
                        vertex.octNormal[1] = static_cast<u8>(glm::round(octNormal.y * 255.0f));

                        // UV
                        if (numUVSets > 0)
                        {
                            vec2 uv = wmoGroup.motvs[0].data[j].uv;

                            vertex.uv.x = uv.x;
                            vertex.uv.y = uv.y;
                        }
                        if (numUVSets > 1)
                        {
                            vec2 uv = wmoGroup.motvs[1].data[j].uv;

                            vertex.uv.z = uv.x;
                            vertex.uv.w = uv.y;
                        }
                    }
                }
            }
        }

        return true;
	}
}