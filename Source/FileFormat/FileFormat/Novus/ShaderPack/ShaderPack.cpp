#pragma once
#include "ShaderPack.h"

#include <Base/Util/DebugHandler.h>

#include <fstream>

namespace FileFormat
{
    ShaderRef* ShaderPack::GetShaderRef(std::shared_ptr<Bytebuffer>& buffer, u32 shaderIndex)
    {
        if (shaderIndex >= manifest.numShaders)
            return nullptr;

        u64 shaderRefOffset = sizeof(FileHeader) + sizeof(ShaderPackManifest) + (shaderIndex * sizeof(ShaderRef));
        ShaderRef* shaderRef = reinterpret_cast<ShaderRef*>(buffer->GetDataPointer() + shaderRefOffset);

        return shaderRef;
    }

    u8* ShaderPack::GetShaderDataPtr(std::shared_ptr<Bytebuffer>& buffer, u32 shaderOffset)
    {
        if (shaderOffset >= buffer->writtenData)
            return nullptr;

        return buffer->GetDataPointer() + shaderOffset;
    }

    bool ShaderPack::Save(const std::string& path, const std::vector<ShaderInMemory>& shaders)
    {
        std::ofstream output(path, std::ofstream::out | std::ofstream::binary);
        if (!output)
        {
            NC_LOG_ERROR("Failed to create ShaderPack file. Check admin permissions {0}", path);
            return false;
        }

        // Fill manifest
        manifest.numShaders = static_cast<u32>(shaders.size());
        NC_ASSERT(shaders.size() > 0, "ShaderPack must have at least one shader");

        // Write the Header and manifest to file
        output.write(reinterpret_cast<char const*>(&header), sizeof(header));
        output.write(reinterpret_cast<char const*>(&manifest), sizeof(manifest));

        // Write the ShaderRefs to file
        u64 dataOffset = static_cast<u64>(output.tellp()) + (sizeof(ShaderRef) * manifest.numShaders);
        ShaderRef shaderRef;
        for (u32 i = 0; i < shaders.size(); i++)
        {
            shaderRef.permutationNameHash = shaders[i].permutationNameHash;
            shaderRef.dataOffset = dataOffset;
            shaderRef.dataSize = shaders[i].size;

            output.write(reinterpret_cast<char const*>(&shaderRef), sizeof(ShaderRef));
            dataOffset += shaders[i].size;
        }

        // Write the shader blobs to file
        for (u32 i = 0; i < shaders.size(); i++)
        {
            output.write(reinterpret_cast<char const*>(shaders[i].data), shaders[i].size);
        }

        output.close();

        return true;
    }

    bool ShaderPack::Read(std::shared_ptr<Bytebuffer>& buffer, ShaderPack& out)
    {
        bool failed = false;
        failed |= !buffer->Get(out.header);
        failed |= !buffer->Get(out.manifest);

        if (failed || out.header.type != FileHeader::Type::ShaderPack || out.header.version != ShaderPack::CURRENT_VERSION)
            return false;

        return true;
    }
}