#include "MaterialPack.h"

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
        return values.empty() ? size : AlignSection(size) + values.size() * sizeof(T);
    }

    bool AlignWrite(std::shared_ptr<Bytebuffer>& buffer)
    {
        constexpr u8 ZERO_PADDING[SECTION_ALIGNMENT] = {};
        const size_t alignedSize = AlignSection(buffer->writtenData);
        return buffer->PutBytes(ZERO_PADDING, alignedSize - buffer->writtenData);
    }

    template <typename T>
    bool WriteSection(std::shared_ptr<Bytebuffer>& buffer, const std::vector<T>& values,
                      u32& offset, u32& count)
    {
        static_assert(std::is_trivially_copyable_v<T>);
        if (values.empty())
        {
            offset = 0;
            count = 0;
            return true;
        }
        if (values.size() > std::numeric_limits<u32>::max() || !AlignWrite(buffer) ||
            buffer->writtenData > std::numeric_limits<u32>::max())
            return false;

        offset = static_cast<u32>(buffer->writtenData);
        count = static_cast<u32>(values.size());
        return buffer->PutBytes(values.data(), values.size() * sizeof(T));
    }

    template <typename T>
    bool IsValidSection(const std::shared_ptr<Bytebuffer>& buffer, u32 offset, u32 count)
    {
        static_assert(std::is_trivially_copyable_v<T>);
        if (count == 0)
            return offset == 0;
        if (offset < sizeof(FileFormat::Material::MaterialPack) ||
            (offset & (SECTION_ALIGNMENT - 1)) != 0 || offset > buffer->writtenData)
            return false;
        return count <= (buffer->writtenData - offset) / sizeof(T);
    }
}

namespace FileFormat::Material
{
    size_t MaterialPack::GetSerializedSize(const MaterialPackData& data) const
    {
        size_t size = sizeof(MaterialPack);
        size = AddSectionSize(size, data.executionGroups);
        size = AddSectionSize(size, data.programs);
        size = AddSectionSize(size, data.programLookups);
        size = AddSectionSize(size, data.parameterDefinitions);
        return size;
    }

    bool MaterialPack::Save(std::shared_ptr<Bytebuffer>& buffer, const MaterialPackData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        MaterialPack serialized = *this;
        serialized.header = FileHeader(MATERIAL_PACK_FILE_TYPE, MATERIAL_PACK_DEVELOPMENT_VERSION);
        serialized.reserved = 0;
        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.executionGroups, serialized.executionGroupsOffset,
                                serialized.numExecutionGroups);
        failed |= !WriteSection(buffer, data.programs, serialized.programsOffset,
                                serialized.numPrograms);
        failed |= !WriteSection(buffer, data.programLookups, serialized.programLookupsOffset,
                                serialized.numProgramLookups);
        failed |= !WriteSection(buffer, data.parameterDefinitions,
                                serialized.parameterDefinitionsOffset,
                                serialized.numParameterDefinitions);
        failed |= !buffer->Put(serialized, 0);
        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool MaterialPack::Read(std::shared_ptr<Bytebuffer>& buffer, MaterialPack& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;
        if (out.header.type != MATERIAL_PACK_FILE_TYPE ||
            out.header.version != MATERIAL_PACK_DEVELOPMENT_VERSION || out.reserved != 0)
            return false;

        return IsValidSection<MaterialExecutionGroup>(buffer, out.executionGroupsOffset,
                                                       out.numExecutionGroups) &&
               IsValidSection<MaterialProgramRecord>(buffer, out.programsOffset,
                                                      out.numPrograms) &&
               IsValidSection<MaterialProgramLookup>(buffer, out.programLookupsOffset,
                                                      out.numProgramLookups) &&
               IsValidSection<ParameterDefinition>(buffer, out.parameterDefinitionsOffset,
                                                    out.numParameterDefinitions);
    }
} // namespace FileFormat::Material
