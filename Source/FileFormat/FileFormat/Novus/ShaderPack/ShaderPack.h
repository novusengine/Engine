#pragma once
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>
#include <Base/Memory/Bytebuffer.h>

#include <limits>
namespace FileFormat
{
    static const std::string SHADER_PACK_EXTENSION = ".shaderpack";

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
    };

    struct ShaderInMemory
    {
    public:
        u32 permutationNameHash;

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
        u8* GetShaderDataPtr(std::shared_ptr<Bytebuffer>& buffer, u32 shaderIndex);

    public:
        FileHeader header = FileHeader(FileHeader::Type::ShaderPack, CURRENT_VERSION);

        ShaderPackManifest manifest;

    public:
        bool Save(const std::string& path, const std::vector<ShaderInMemory>& shaders);

        static bool Read(std::shared_ptr<Bytebuffer>& buffer, ShaderPack& out);
    };
}