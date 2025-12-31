#pragma once
#include "ShaderPack.h"

#include <Base/Util/DebugHandler.h>

#include <fstream>

namespace FileFormat
{
    std::string DescriptorReflection::ToString() const
    {
        std::string result = "DescriptorReflection: { ";
        result += "Binding: " + std::to_string(binding) + ", ";
        result += "Name: " + name + ", ";
        result += "Type: " + std::string(DescriptorTypeReflectionNames[static_cast<u32>(type)]) + ", ";
        result += "SubType: " + std::string(DescriptorTypeReflectionNames[static_cast<u32>(subType)]) + ", ";
        result += "Count: " + std::to_string(count);
        result += " }";
        return result;
    }

    DescriptorSetReflection& ShaderReflection::GetDescriptorSetBySlot(u32 slot)
    {
        if (!descriptorSets.contains(slot))
        {
            DescriptorSetReflection descriptorSetReflection;
            descriptorSetReflection.slot = slot;

            descriptorSets[slot] = descriptorSetReflection;
        }

        return descriptorSets[slot];
    }

    ShaderRef* ShaderPack::GetShaderRef(std::shared_ptr<Bytebuffer>& buffer, u32 shaderIndex)
    {
        if (shaderIndex >= manifest.numShaders)
            return nullptr;

        u64 shaderRefOffset = sizeof(FileHeader) + sizeof(ShaderPackManifest) + (shaderIndex * sizeof(ShaderRef));
        ShaderRef* shaderRef = reinterpret_cast<ShaderRef*>(buffer->GetDataPointer() + shaderRefOffset);

        return shaderRef;
    }

    bool ShaderPack::GetShaderReflection(std::shared_ptr<Bytebuffer>& buffer, u32 shaderIndex, ShaderReflection& reflection)
    {
        ShaderRef* shaderRef = GetShaderRef(buffer, shaderIndex);
        if (shaderRef == nullptr || shaderRef->reflectionOffset == 0 || shaderRef->reflectionSize == 0)
            return false;

        u64 readOffset = buffer->readData;

        buffer->readData = shaderRef->reflectionOffset;
        u8 numDescriptorSets;
        if (!buffer->GetU8(numDescriptorSets))
            return false;

        reflection.descriptorSets.reserve(numDescriptorSets);

        for(u32 i = 0; i < numDescriptorSets; i++)
        {
            u8 setSlot;
            u8 numDescriptors;

            if (!buffer->GetU8(setSlot))
                return false;

            if (!buffer->GetU8(numDescriptors))
                return false;

            DescriptorSetReflection& descriptorSet = reflection.descriptorSets[setSlot];
            descriptorSet.slot = setSlot;
            descriptorSet.descriptors.reserve(numDescriptors);

            for (u32 j = 0; j < numDescriptors; j++)
            {
                u8 binding;
                std::string name;
                u8 type;
                u8 subType;
                u16 count;
                u8 accessType;
                bool isUsed;
                u32 byteOffset;
                u32 byteSize;

                bool failed = false;
                failed |= !buffer->GetU8(binding);
                failed |= !buffer->GetString(name);
                failed |= !buffer->GetU8(type);
                failed |= !buffer->GetU8(subType);
                failed |= !buffer->GetU16(count);
                failed |= !buffer->GetU8(accessType);
                failed |= !buffer->Get(isUsed);
                failed |= !buffer->GetU32(byteOffset);
                failed |= !buffer->GetU32(byteSize);

                if (failed)
                    return false;

                DescriptorReflection& descriptor = descriptorSet.descriptors[binding];
                descriptor.binding = binding;
                descriptor.name = std::move(name);
                descriptor.type = static_cast<DescriptorTypeReflection>(type);
                descriptor.subType = static_cast<DescriptorTypeReflection>(subType);
                descriptor.count = count;
                descriptor.accessType = static_cast<DescriptorAccessTypeReflection>(accessType);
                descriptor.isUsed = isUsed;
                descriptor.byteOffset = byteOffset;
                descriptor.byteSize = byteSize;
            }
        }

        buffer->readData = readOffset;
        return true;
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

        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<67108864>();

        // Write the Header and manifest to buffer
        buffer->Put(header);
        buffer->Put(manifest);

        // Write the ShaderRefs to file
        u64 baseShaderRefOffset = buffer->writtenData;
        u64 dataOffset = baseShaderRefOffset + (sizeof(ShaderRef) * manifest.numShaders);
        
        ShaderRef shaderRef;
        for (u32 i = 0; i < shaders.size(); i++)
        {
            shaderRef.permutationNameHash = shaders[i].permutationNameHash;
            shaderRef.dataOffset = dataOffset;
            shaderRef.dataSize = shaders[i].size;

            buffer->Put(shaderRef);
            dataOffset += shaders[i].size;
        }

        // Write the shader blobs to file
        for (u32 i = 0; i < shaders.size(); i++)
        {
            buffer->PutBytes(shaders[i].data, shaders[i].size);
        }

        // Write the reflection data to file
        for (u32 i = 0; i < shaders.size(); i++)
        {
            u64 reflectionDataStartOffset = static_cast<u64>(buffer->writtenData);

            const ShaderInMemory& shader = shaders[i];

            // Write reflection data
            u8 numDescriptorSets = static_cast<u8>(shader.reflection.descriptorSets.size());
            buffer->PutU8(numDescriptorSets);
            for(const auto& [setSlot, descriptorSet] : shader.reflection.descriptorSets)
            {
                buffer->PutU8(descriptorSet.slot);

                u8 numDescriptors = static_cast<u8>(descriptorSet.descriptors.size());
                buffer->PutU8(numDescriptors);
                for (const auto& [binding, descriptor] : descriptorSet.descriptors)
                {
                    buffer->PutU8(descriptor.binding);
                    buffer->PutString(descriptor.name);
                    buffer->PutU8(static_cast<u8>(descriptor.type));
                    buffer->PutU8(static_cast<u8>(descriptor.subType));
                    buffer->PutU16(descriptor.count);
                    buffer->PutU8(static_cast<u8>(descriptor.accessType));
                    buffer->Put(descriptor.isUsed);
                    buffer->PutU32(descriptor.byteOffset);
                    buffer->PutU32(descriptor.byteSize);
                }
            }
            
            u64 reflectionDataEndOffset = static_cast<u64>(buffer->writtenData);

            // Patch ShaderRef with reflection data offset and size
            u64 shaderRefOffset = baseShaderRefOffset + (i * sizeof(ShaderRef));
            ShaderRef* shaderRefInBuffer = reinterpret_cast<ShaderRef*>(buffer->GetDataPointer() + shaderRefOffset);
            shaderRefInBuffer->reflectionOffset = reflectionDataStartOffset;
            shaderRefInBuffer->reflectionSize = static_cast<u32>(reflectionDataEndOffset - reflectionDataStartOffset);
        }

        output.write(reinterpret_cast<char const*>(buffer->GetDataPointer()), buffer->writtenData);

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