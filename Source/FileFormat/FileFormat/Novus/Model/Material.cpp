#include "Material.h"

#include <Base/Memory/Bytebuffer.h>

#include <xxhash/xxhash64.h>

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

namespace FileFormat::Material
{
    u64 CalculateParameterLayoutHash(std::span<const ParameterDefinition> parameters, u32 parameterBlockSize)
    {
        const u64 definitionsHash = XXHash64::hash(parameters.data(), parameters.size_bytes(), 0);
        return XXHash64::hash(&definitionsHash, sizeof(definitionsHash), parameterBlockSize);
    }

    size_t MaterialAsset::GetSerializedSize(const MaterialData& data) const
    {
        size_t size = sizeof(MaterialAsset);
        size = AddSectionSize(size, data.parameters);
        size = AddSectionSize(size, data.defaultParameterData);
        return size;
    }

    bool MaterialAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const MaterialData& data)
    {
        if (!buffer || buffer->writtenData != 0 || data.defaultParameterData.size() > std::numeric_limits<u32>::max() || GetSerializedSize(data) > buffer->size)
            return false;

        MaterialAsset serialized = *this;
        serialized.header = FileHeader(MATERIAL_FILE_TYPE, VERSION);
        serialized.parameterBlockSize = static_cast<u32>(data.defaultParameterData.size());

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.parameters, serialized.parametersOffset, serialized.numParameters);
        failed |= !WriteSection(buffer, data.defaultParameterData, serialized.defaultParameterDataOffset, serialized.defaultParameterDataSize);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool MaterialAsset::Read(std::shared_ptr<Bytebuffer>& buffer, MaterialAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != MATERIAL_FILE_TYPE || out.header.version != VERSION || out.parameterBlockSize != out.defaultParameterDataSize)
            return false;

        const size_t headerSize = sizeof(MaterialAsset);
        return IsValidSection<ParameterDefinition>(buffer, out.parametersOffset, out.numParameters, headerSize) &&
               IsValidSection<u8>(buffer, out.defaultParameterDataOffset, out.defaultParameterDataSize, headerSize);
    }

    size_t MaterialInstanceAsset::GetSerializedSize(const MaterialInstanceData& data) const
    {
        size_t size = sizeof(MaterialInstanceAsset);
        size = AddSectionSize(size, data.parameterData);
        size = AddSectionSize(size, data.textureBindings);
        size = AddSectionSize(size, data.animationBindings);
        return size;
    }

    bool MaterialInstanceAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const MaterialInstanceData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        MaterialInstanceAsset serialized = *this;
        serialized.header = FileHeader(MATERIAL_INSTANCE_FILE_TYPE, VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.parameterData, serialized.parameterDataOffset, serialized.parameterDataSize);
        failed |= !WriteSection(buffer, data.textureBindings, serialized.textureBindingsOffset, serialized.numTextureBindings);
        failed |= !WriteSection(buffer, data.animationBindings, serialized.animationBindingsOffset, serialized.numAnimationBindings);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool MaterialInstanceAsset::Read(std::shared_ptr<Bytebuffer>& buffer, MaterialInstanceAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != MATERIAL_INSTANCE_FILE_TYPE || out.header.version != VERSION)
            return false;

        const size_t headerSize = sizeof(MaterialInstanceAsset);
        return IsValidSection<u8>(buffer, out.parameterDataOffset, out.parameterDataSize, headerSize) &&
               IsValidSection<TextureBinding>(buffer, out.textureBindingsOffset, out.numTextureBindings, headerSize) &&
               IsValidSection<MaterialAnimationBinding>(buffer, out.animationBindingsOffset, out.numAnimationBindings, headerSize);
    }

    size_t MaterialAnimationAsset::GetSerializedSize(const MaterialAnimationData& data) const
    {
        size_t size = sizeof(MaterialAnimationAsset);
        size = AddSectionSize(size, data.tracks);
        size = AddSectionSize(size, data.samples);
        return size;
    }

    bool MaterialAnimationAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const MaterialAnimationData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        MaterialAnimationAsset serialized = *this;
        serialized.header = FileHeader(MATERIAL_ANIMATION_FILE_TYPE, VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.tracks, serialized.tracksOffset, serialized.numTracks);
        failed |= !WriteSection(buffer, data.samples, serialized.samplesOffset, serialized.numSamples);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool MaterialAnimationAsset::Read(std::shared_ptr<Bytebuffer>& buffer, MaterialAnimationAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != MATERIAL_ANIMATION_FILE_TYPE || out.header.version != VERSION)
            return false;

        const size_t headerSize = sizeof(MaterialAnimationAsset);
        return IsValidSection<MaterialAnimationTrack>(buffer, out.tracksOffset, out.numTracks, headerSize) &&
               IsValidSection<vec4>(buffer, out.samplesOffset, out.numSamples, headerSize);
    }
}
