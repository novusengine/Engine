#include "Model.h"

#include <Base/Memory/Bytebuffer.h>

#include <limits>

namespace
{
    constexpr size_t SECTION_ALIGNMENT = 16;

    size_t AlignSection(size_t size)
    {
        return (size + SECTION_ALIGNMENT - 1) & ~(SECTION_ALIGNMENT - 1);
    }

    template <typename T>
    size_t AddSectionSize(size_t size, const std::vector<T>& values)
    {
        static_assert(std::is_trivially_copyable_v<T>);
        if (values.empty())
            return size;

        return AlignSection(size) + values.size() * sizeof(T);
    }

    bool AlignWrite(std::shared_ptr<Bytebuffer>& buffer)
    {
        constexpr u8 ZERO_PADDING[SECTION_ALIGNMENT] = {};
        const size_t alignedSize = AlignSection(buffer->writtenData);
        return buffer->PutBytes(ZERO_PADDING, alignedSize - buffer->writtenData);
    }

    template <typename T>
    bool WriteSection(std::shared_ptr<Bytebuffer>& buffer, const std::vector<T>& values, u32& offset, u32& count)
    {
        static_assert(std::is_trivially_copyable_v<T>);

        if (values.empty())
        {
            offset = 0;
            count = 0;
            return true;
        }

        if (values.size() > std::numeric_limits<u32>::max() || !AlignWrite(buffer) || buffer->writtenData > std::numeric_limits<u32>::max())
            return false;

        offset = static_cast<u32>(buffer->writtenData);
        count = static_cast<u32>(values.size());
        return buffer->PutBytes(values.data(), values.size() * sizeof(T));
    }

    template <typename T>
    bool IsValidSection(const std::shared_ptr<Bytebuffer>& buffer, u32 offset, u32 count, size_t headerSize)
    {
        static_assert(std::is_trivially_copyable_v<T>);

        if (count == 0)
            return offset == 0;

        if (offset < headerSize || (offset & (SECTION_ALIGNMENT - 1)) != 0 || offset > buffer->writtenData)
            return false;

        return count <= (buffer->writtenData - offset) / sizeof(T);
    }
}

namespace FileFormat::Model
{
    size_t ModelAsset::GetSerializedSize(const ModelData& data) const
    {
        size_t size = sizeof(ModelAsset);
        size = AddSectionSize(size, data.meshes);
        size = AddSectionSize(size, data.meshLODs);
        size = AddSectionSize(size, data.submeshes);
        size = AddSectionSize(size, data.meshlets);
        size = AddSectionSize(size, data.positions);
        size = AddSectionSize(size, data.vertexAttributes);
        size = AddSectionSize(size, data.skinningData);
        size = AddSectionSize(size, data.meshletVertexIndices);
        size = AddSectionSize(size, data.meshletTriangles);
        size = AddSectionSize(size, data.jointPaletteRemaps);
        size = AddSectionSize(size, data.materialSlots);
        size = AddSectionSize(size, data.parameters);
        size = AddSectionSize(size, data.parameterBindings);
        size = AddSectionSize(size, data.embeddedInstanceSets);
        size = AddSectionSize(size, data.embeddedInstances);
        return size;
    }

    bool ModelAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const ModelData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        ModelAsset serialized = *this;
        serialized.header = FileHeader(FILE_TYPE, VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.meshes, serialized.meshesOffset, serialized.numMeshes);
        failed |= !WriteSection(buffer, data.meshLODs, serialized.meshLODsOffset, serialized.numMeshLODs);
        failed |= !WriteSection(buffer, data.submeshes, serialized.submeshesOffset, serialized.numSubmeshes);
        failed |= !WriteSection(buffer, data.meshlets, serialized.meshletsOffset, serialized.numMeshlets);
        failed |= !WriteSection(buffer, data.positions, serialized.positionsOffset, serialized.numPositions);
        failed |= !WriteSection(buffer, data.vertexAttributes, serialized.vertexAttributesOffset, serialized.numVertexAttributes);
        failed |= !WriteSection(buffer, data.skinningData, serialized.skinningDataOffset, serialized.numSkinningData);
        failed |= !WriteSection(buffer, data.meshletVertexIndices, serialized.meshletVertexIndicesOffset, serialized.numMeshletVertexIndices);
        failed |= !WriteSection(buffer, data.meshletTriangles, serialized.meshletTrianglesOffset, serialized.numMeshletTriangles);
        failed |= !WriteSection(buffer, data.jointPaletteRemaps, serialized.jointPaletteRemapsOffset, serialized.numJointPaletteRemaps);
        failed |= !WriteSection(buffer, data.materialSlots, serialized.materialSlotsOffset, serialized.numMaterialSlots);
        failed |= !WriteSection(buffer, data.parameters, serialized.parametersOffset, serialized.numParameters);
        failed |= !WriteSection(buffer, data.parameterBindings, serialized.parameterBindingsOffset, serialized.numParameterBindings);
        failed |= !WriteSection(buffer, data.embeddedInstanceSets, serialized.embeddedInstanceSetsOffset, serialized.numEmbeddedInstanceSets);
        failed |= !WriteSection(buffer, data.embeddedInstances, serialized.embeddedInstancesOffset, serialized.numEmbeddedInstances);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool ModelAsset::Read(std::shared_ptr<Bytebuffer>& buffer, ModelAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != FILE_TYPE || out.header.version != VERSION)
            return false;

        const size_t headerSize = sizeof(ModelAsset);
        return IsValidSection<Mesh>(buffer, out.meshesOffset, out.numMeshes, headerSize) &&
               IsValidSection<MeshLOD>(buffer, out.meshLODsOffset, out.numMeshLODs, headerSize) &&
               IsValidSection<Submesh>(buffer, out.submeshesOffset, out.numSubmeshes, headerSize) &&
               IsValidSection<Meshlet>(buffer, out.meshletsOffset, out.numMeshlets, headerSize) &&
               IsValidSection<PackedPosition>(buffer, out.positionsOffset, out.numPositions, headerSize) &&
               IsValidSection<PackedVertexAttributes>(buffer, out.vertexAttributesOffset, out.numVertexAttributes, headerSize) &&
               IsValidSection<PackedSkinningData>(buffer, out.skinningDataOffset, out.numSkinningData, headerSize) &&
               IsValidSection<u32>(buffer, out.meshletVertexIndicesOffset, out.numMeshletVertexIndices, headerSize) &&
               IsValidSection<PackedMeshletTriangle>(buffer, out.meshletTrianglesOffset, out.numMeshletTriangles, headerSize) &&
               IsValidSection<u16>(buffer, out.jointPaletteRemapsOffset, out.numJointPaletteRemaps, headerSize) &&
               IsValidSection<MaterialSlot>(buffer, out.materialSlotsOffset, out.numMaterialSlots, headerSize) &&
               IsValidSection<Parameter>(buffer, out.parametersOffset, out.numParameters, headerSize) &&
               IsValidSection<ParameterBinding>(buffer, out.parameterBindingsOffset, out.numParameterBindings, headerSize) &&
               IsValidSection<EmbeddedInstanceSet>(buffer, out.embeddedInstanceSetsOffset, out.numEmbeddedInstanceSets, headerSize) &&
               IsValidSection<EmbeddedInstance>(buffer, out.embeddedInstancesOffset, out.numEmbeddedInstances, headerSize);
    }
}
