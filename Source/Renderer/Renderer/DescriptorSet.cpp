#include "DescriptorSet.h"
#include "DescriptorType.h"
#include "Renderer.h"
#include "RenderGraphResources.h"

#include <tracy/Tracy.hpp>

namespace Renderer
{
    DescriptorSet::~DescriptorSet()
    {
        Release();
    }

    DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
        : _renderer(other._renderer), _initialized(other._initialized), _slot(other._slot),
          _metaInfo(std::move(other._metaInfo)), _combinedReflection(std::move(other._combinedReflection)),
          _nameHashToBindingIndex(std::move(other._nameHashToBindingIndex)), _descriptorSetID(other._descriptorSetID),
          _locked(other._locked)
    {
        other._renderer = nullptr;
        other._initialized = false;
    }

    DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept
    {
        if (this == &other)
            return *this;

        Release();
        _renderer = other._renderer;
        _initialized = other._initialized;
        _slot = other._slot;
        _metaInfo = std::move(other._metaInfo);
        _combinedReflection = std::move(other._combinedReflection);
        _nameHashToBindingIndex = std::move(other._nameHashToBindingIndex);
        _descriptorSetID = other._descriptorSetID;
        _locked = other._locked;
        other._renderer = nullptr;
        other._initialized = false;
        return *this;
    }

    void DescriptorSet::Release()
    {
        if (!_initialized)
            return;

        _renderer->DestroyDescriptorSet(_descriptorSetID);
        _renderer = nullptr;
        _initialized = false;
    }

    void VerifyDescriptorMatch(const FileFormat::DescriptorReflection& a, const FileFormat::DescriptorReflection& b)
    {
        if (a.name != b.name)
        {
            NC_LOG_CRITICAL("Descriptor name mismatch in combined descriptor set reflection: '{}' != '{}'", a.name, b.name);
        }
        if (a.type != b.type)
        {
            NC_LOG_CRITICAL("Descriptor type mismatch in combined descriptor set reflection for descriptor '{}'", a.name);
        }
        if (a.subType != b.subType)
        {
            NC_LOG_CRITICAL("Descriptor subType mismatch in combined descriptor set reflection for descriptor '{}'", a.name);
        }
        if (a.count != b.count)
        {
            NC_LOG_CRITICAL("Descriptor count mismatch in combined descriptor set reflection for descriptor '{}'", a.name);
        }
    }

    void DescriptorSet::RegisterPipeline(Renderer* renderer, ComputePipelineID pipelineID)
    {
        ZoneScoped;
        u32 slotIndex = static_cast<u32>(_slot);

        const ComputePipelineDesc& pipelineDesc = renderer->GetDesc(pipelineID);
        const ComputeShaderDesc& shaderDesc = renderer->GetDesc(pipelineDesc.computeShader);

        const FileFormat::ShaderReflection& reflection = shaderDesc.shaderEntry->reflection;

        if (!reflection.descriptorSets.contains(slotIndex))
        {
            return;
        }

        const FileFormat::DescriptorSetReflection& descriptorSet = reflection.descriptorSets.at(slotIndex);
        for(const auto& [bindingIndex, descriptor] : descriptorSet.descriptors)
        {
            if (_combinedReflection.descriptors.contains(bindingIndex))
            {
                VerifyDescriptorMatch(_combinedReflection.descriptors[bindingIndex], descriptor);
            }
            else
            {
                _combinedReflection.descriptors[bindingIndex] = descriptor;
                u32 nameHash = StringUtils::fnv1a_32(descriptor.name.c_str(), descriptor.name.size());
                _nameHashToBindingIndex[nameHash] = bindingIndex;
            }
        }
    }

    void DescriptorSet::RegisterPipeline(Renderer* renderer, GraphicsPipelineID pipelineID)
    {
        ZoneScoped;
        u32 slotIndex = static_cast<u32>(_slot);

        const GraphicsPipelineDesc& pipelineDesc = renderer->GetDesc(pipelineID);
        auto registerShader = [&](const auto& shaderDesc)
        {
            const FileFormat::ShaderReflection& reflection = shaderDesc.shaderEntry->reflection;
            if (!reflection.descriptorSets.contains(slotIndex))
            {
                return;
            }

            const FileFormat::DescriptorSetReflection& descriptorSet = reflection.descriptorSets.at(slotIndex);
            for (const auto& [bindingIndex, descriptor] : descriptorSet.descriptors)
            {
                if (_combinedReflection.descriptors.contains(bindingIndex))
                {
                    VerifyDescriptorMatch(_combinedReflection.descriptors[bindingIndex], descriptor);
                }
                else
                {
                    _combinedReflection.descriptors[bindingIndex] = descriptor;
                    u32 nameHash = StringUtils::fnv1a_32(descriptor.name.c_str(), descriptor.name.size());
                    _nameHashToBindingIndex[nameHash] = bindingIndex;
                }
            }
        };

        if (std::holds_alternative<VertexPipelineStages>(pipelineDesc.shaderStages))
        {
            const VertexPipelineStages& stages = std::get<VertexPipelineStages>(pipelineDesc.shaderStages);
            registerShader(renderer->GetDesc(stages.vertexShader));
        }
        else
        {
            const MeshPipelineStages& stages = std::get<MeshPipelineStages>(pipelineDesc.shaderStages);
            if (stages.taskShader != TaskShaderID::Invalid())
            {
                registerShader(renderer->GetDesc(stages.taskShader));
            }
            registerShader(renderer->GetDesc(stages.meshShader));
        }

        if (pipelineDesc.states.pixelShader != PixelShaderID::Invalid())
        {
            registerShader(renderer->GetDesc(pipelineDesc.states.pixelShader));
        }
    }

    void DescriptorSet::Init(Renderer* renderer)
    {
        ZoneScoped;
        _renderer = renderer;

        DescriptorSetDesc desc;
        desc.reflection = &_combinedReflection;

        _descriptorSetID = renderer->CreateDescriptorSet(desc);
        _initialized = true;
    }

    void DescriptorSet::Bind(StringUtils::StringHash nameHash, BufferID bufferID, bool optional)
    {
        ZoneScoped;
        if (!_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        if (optional && !HasBinding(nameHash))
        {
            return;
        }

        u32 bindingIndex = GetBindingIndex(nameHash);
        FileFormat::DescriptorReflection& descriptorReflection = GetDescriptorReflection(bindingIndex); // TODO: Add verification stuff

        u32 frameIndexCount = _renderer->GetFrameIndexCount();
        for(u32 i = 0; i < frameIndexCount; i++)
        {
            _renderer->BindDescriptor(_descriptorSetID, bindingIndex, bufferID, DescriptorType::StorageBuffer, i);
        }
    }

    void DescriptorSet::Bind(StringUtils::StringHash nameHash, SamplerID samplerID, bool optional)
    {
        ZoneScoped;
        if (!_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        if (optional && !HasBinding(nameHash))
        {
            return;
        }

        u32 bindingIndex = GetBindingIndex(nameHash);
        //FileFormat::DescriptorReflection& descriptorReflection = GetDescriptorReflection(bindingIndex); // TODO: Add verification stuff

        u32 frameIndexCount = _renderer->GetFrameIndexCount();
        for (u32 i = 0; i < frameIndexCount; i++)
        {
            _renderer->BindDescriptor(_descriptorSetID, bindingIndex, samplerID, i);
        }
    }

    void DescriptorSet::BindArray(StringUtils::StringHash nameHash, SamplerID samplerID, u32 arrayIndex, bool optional)
    {
        ZoneScoped;
        if (!_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        if (optional && !HasBinding(nameHash))
        {
            return;
        }

        u32 bindingIndex = GetBindingIndex(nameHash);
        //FileFormat::DescriptorReflection& descriptorReflection = GetDescriptorReflection(bindingIndex); // TODO: Add verification stuff

        u32 frameIndexCount = _renderer->GetFrameIndexCount();
        for (u32 i = 0; i < frameIndexCount; i++)
        {
            _renderer->BindDescriptorArray(_descriptorSetID, bindingIndex, samplerID, arrayIndex, i);
        }
    }

    void DescriptorSet::Bind(StringUtils::StringHash nameHash, TextureArrayID textureArrayID, bool optional)
    {
        ZoneScoped;
        if (!_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        if (optional && !HasBinding(nameHash))
        {
            return;
        }

        u32 bindingIndex = GetBindingIndex(nameHash);
        FileFormat::DescriptorReflection& descriptorReflection = GetDescriptorReflection(bindingIndex); // TODO: Add verification stuff

        _renderer->BindDescriptor(_descriptorSetID, bindingIndex, textureArrayID);
    }

    bool DescriptorSet::HasBinding(StringUtils::StringHash nameHash) const
    {
        return _nameHashToBindingIndex.find(nameHash.computedHash) != _nameHashToBindingIndex.end();
    }

    u32 DescriptorSet::GetBindingIndex(StringUtils::StringHash nameHash) const
    {
        if (!HasBinding(nameHash))
        {
            NC_LOG_CRITICAL("DescriptorSet::Bind: Tried to bind to a descriptor namehash '{}' that does not exist in the combined reflection for this DescriptorSet, are you trying to bind to a non existing binding in the shader?", nameHash.computedHash);
        }

        return _nameHashToBindingIndex.at(nameHash.computedHash);
    }

    FileFormat::DescriptorReflection& DescriptorSet::GetDescriptorReflection(u32 bindingIndex)
    {
        if (_combinedReflection.descriptors.find(bindingIndex) == _combinedReflection.descriptors.end())
        {
            NC_LOG_CRITICAL("DescriptorSet::GetDescriptorReflection: Tried to get descriptor reflection for binding index '{}' that does not exist in the combined reflection for this DescriptorSet", bindingIndex);
        }

        return _combinedReflection.descriptors[bindingIndex];
    }
}
