#pragma once
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>
#include <Base/Memory/Bytebuffer.h>

#include <robinhood/robinhood.h>
#include <limits>
namespace FileFormat
{
    static const std::string SHADER_PACK_EXTENSION = ".shaderpack";

    enum class DescriptorTypeReflection : u8
    {
        Unknown,

        Array, // If this is the type, we have a subType that tells us what type of array it is

        ByteAddressBuffer,
        ConstantBuffer,
        SamplerState,
        StructuredBuffer,
        Texture,
        TextureArray,

        COUNT
    };
    inline const char* DescriptorTypeReflectionNames[] =
    {
        "Unknown",

        "Array",

        "ByteAddressBuffer",
        "ConstantBuffer",
        "SamplerState",
        "StructuredBuffer",
        "Texture",
        "TextureArray",

        "COUNT"
    };
    STATIC_ASSERT_MATCHING_ENUM_ARRAY(DescriptorTypeReflection, DescriptorTypeReflectionNames);

    enum class DescriptorAccessTypeReflection : u8
    {
        None,
        Read,
        ReadWrite,
        RasterOrdered,
        Append,
        Consume,
        Write,
        Feedback,

        COUNT
    };
    inline const char* DescriptorAccessTypeReflectionNames[] =
    {
        "None",
        "Read",
        "ReadWrite",
        "RasterOrdered",
        "Append",
        "Consume",
        "Write",
        "ResourceAccessFeedback",

        "COUNT"
    };
    STATIC_ASSERT_MATCHING_ENUM_ARRAY(DescriptorAccessTypeReflection, DescriptorAccessTypeReflectionNames);

    struct DescriptorReflection
    {
    public:
        std::string ToString() const;

    public:
        u32 binding;
        std::string name;
        DescriptorTypeReflection type;
        DescriptorTypeReflection subType; // For arrays
        u32 count; // For arrays

        DescriptorAccessTypeReflection accessType;
        bool isUsed;
        u32 byteOffset;
        u32 byteSize;
    };

    struct DescriptorSetReflection
    {
    public:
        u32 slot;
        robin_hood::unordered_map<u32, DescriptorReflection> descriptors;
    };

    struct ShaderReflection
    {
        DescriptorSetReflection& GetDescriptorSetBySlot(u32 slot);

    public:
        robin_hood::unordered_map<u32, DescriptorSetReflection> descriptorSets;
    };

    struct ShaderPackManifest
    {
    public:
        ShaderPackManifest() {}
        ShaderPackManifest(u32 inNumShaders)
        {
            numShaders = inNumShaders;
        }

    public:
        u32 numShaders = 0;
    };

    struct ShaderRef
    {
    public:
        u32 permutationNameHash;

        u64 dataOffset = 0;
        u32 dataSize = 0;

        u64 reflectionOffset = 0;
        u32 reflectionSize = 0;
    };

    struct ShaderInMemory
    {
    public:
        u32 permutationNameHash;

        ShaderReflection reflection;

        u8* data = nullptr;
        u32 size = 0;
    };

    struct ShaderPack
    {
        static const u32 CURRENT_VERSION = 1;
    public:
        ShaderPack() {}

        u32 GetNumShaders() const { return manifest.numShaders; }

        ShaderRef* GetShaderRef(std::shared_ptr<Bytebuffer>& buffer, u32 shaderIndex);
        bool GetShaderReflection(std::shared_ptr<Bytebuffer>& buffer, u32 shaderIndex, ShaderReflection& reflection);
        u8* GetShaderDataPtr(std::shared_ptr<Bytebuffer>& buffer, u32 shaderIndex);

    public:
        FileHeader header = FileHeader(FileHeader::Type::ShaderPack, CURRENT_VERSION);

        ShaderPackManifest manifest;

    public:
        bool Save(const std::string& path, const std::vector<ShaderInMemory>& shaders);

        static bool Read(std::shared_ptr<Bytebuffer>& buffer, ShaderPack& out);
    };
}