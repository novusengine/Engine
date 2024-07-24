#include "DescriptorSet.h"
#include "RenderGraphResources.h"

namespace Renderer
{
    void DescriptorSet::Bind(StringUtils::StringHash nameHash, BufferID bufferID)
    {
        NC_ASSERT(!_locked, "DescriptorSet : Tried to Bind a BufferID to a DescriptorSet that is currently in use by a RenderPass. Please use the DescriptorSetResource");

        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_BUFFER;
                _boundDescriptors[i].bufferID = bufferID;
                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_BUFFER;
        boundDescriptor.bufferID = bufferID;
    }

    void DescriptorSet::BindArray(StringUtils::StringHash nameHash, BufferID bufferID, u32 arrayIndex)
    {
        NC_ASSERT(!_locked, "DescriptorSet : Tried to BindArray a BufferID to a DescriptorSet that is currently in use by a RenderPass. Please use the DescriptorSetResource");

        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash && arrayIndex == _boundDescriptors[i].arrayIndex)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_BUFFER_ARRAY;
                _boundDescriptors[i].bufferID = bufferID;
                _boundDescriptors[i].arrayIndex = arrayIndex;

                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_BUFFER_ARRAY;
        boundDescriptor.bufferID = bufferID;
        boundDescriptor.arrayIndex = arrayIndex;
    }

    void DescriptorSet::Bind(StringUtils::StringHash nameHash, SamplerID samplerID)
    {
        NC_ASSERT(!_locked, "DescriptorSet : Tried to Bind a SamplerID to a DescriptorSet that is currently in use by a RenderPass. Please use the DescriptorSetResource");

        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_SAMPLER;
                _boundDescriptors[i].samplerID = samplerID;
                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_SAMPLER;
        boundDescriptor.samplerID = samplerID;
    }

    void DescriptorSet::BindArray(StringUtils::StringHash nameHash, SamplerID samplerID, u32 arrayIndex)
    {
        NC_ASSERT(!_locked, "DescriptorSet : Tried to BindArray a SamplerID to a DescriptorSet that is currently in use by a RenderPass. Please use the DescriptorSetResource");

        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash && arrayIndex == _boundDescriptors[i].arrayIndex)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_SAMPLER_ARRAY;
                _boundDescriptors[i].samplerID = samplerID;
                _boundDescriptors[i].arrayIndex = arrayIndex;

                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_SAMPLER_ARRAY;
        boundDescriptor.samplerID = samplerID;
        boundDescriptor.arrayIndex = arrayIndex;
    }

    void DescriptorSet::Bind(StringUtils::StringHash nameHash, TextureID textureID)
    {
        NC_ASSERT(!_locked, "DescriptorSet : Tried to Bind a TextureID to a DescriptorSet that is currently in use by a RenderPass. Please use the DescriptorSetResource");

        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_TEXTURE;
                _boundDescriptors[i].textureID = textureID;
                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_TEXTURE;
        boundDescriptor.textureID = textureID;
    }

    void DescriptorSet::Bind(StringUtils::StringHash nameHash, TextureArrayID textureArrayID)
    {
        NC_ASSERT(!_locked, "DescriptorSet : Tried to Bind a TextureArrayID to a DescriptorSet that is currently in use by a RenderPass. Please use the DescriptorSetResource");

        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_TEXTURE_ARRAY;
                _boundDescriptors[i].textureArrayID = textureArrayID;

                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_TEXTURE_ARRAY;
        boundDescriptor.textureArrayID = textureArrayID;
    }
}