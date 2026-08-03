#include "Map.h"

#include <Base/Types.h>
#include <Base/Memory/Bytebuffer.h>
#include <Base/Util/DebugHandler.h>

#include <fstream>

namespace Map
{
    ModelResourceAllocationHints& ModelResourceAllocationHints::operator+=(const ModelResourceAllocationHints& other)
    {
        models += other.models;
        meshes += other.meshes;
        meshLODs += other.meshLODs;
        submeshes += other.submeshes;
        meshlets += other.meshlets;
        positions += other.positions;
        vertexAttributes += other.vertexAttributes;
        skinningRecords += other.skinningRecords;
        meshletVertexIndices += other.meshletVertexIndices;
        meshletTriangleRecords += other.meshletTriangleRecords;
        jointPaletteRemaps += other.jointPaletteRemaps;
        materialSlots += other.materialSlots;
        embeddedInstanceSets += other.embeddedInstanceSets;
        embeddedInstanceRecords += other.embeddedInstanceRecords;
        return *this;
    }

    ModelSceneAllocationHints& ModelSceneAllocationHints::operator+=(const ModelSceneAllocationHints& other)
    {
        rootPlacements += other.rootPlacements;
        selectedRenderableEmbeddedInstances += other.selectedRenderableEmbeddedInstances;
        totalModelInstances += other.totalModelInstances;
        geometryGroupMaskWords += other.geometryGroupMaskWords;
        meshletHistoryWords += other.meshletHistoryWords;
        return *this;
    }

    bool MapHeader::Save(std::shared_ptr<Bytebuffer>& buffer)
    {
        bool failed = false;

        failed |= !buffer->Put(header);
        failed |= !buffer->Put(flags);
        failed |= !buffer->Put(placement);
        failed |= !buffer->Put(modelAllocationHints);

        u32 numChunks = static_cast<u32>(chunkHashes.size());
        failed |= !buffer->PutU32(numChunks);

        if (numChunks > 0)
        {
            failed |= !buffer->PutBytes(chunkHashes.data(), numChunks * sizeof(u64));
        }

        return !failed;
    }
    bool MapHeader::Read(std::shared_ptr<Bytebuffer>& buffer, MapHeader& out)
    {
        if (!buffer->Get(out.header))
            return false;

        if (out.header.type != FileHeader::Type::MapHeader || out.header.version != MapHeader::CURRENT_VERSION)
            return false;

        if (!buffer->Get(out.flags))
            return false;

        if (!buffer->Get(out.placement))
            return false;

        if (!buffer->Get(out.modelAllocationHints))
            return false;

        u32 numChunks = 0;
        if (!buffer->GetU32(numChunks))
            return false;

        if (!buffer->GetVector(out.chunkHashes, numChunks))
            return false;

        return true;
    }
}
