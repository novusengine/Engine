#include "DescriptorSet.h"

namespace Renderer
{
    void DescriptorSet::Bind(StringUtils::StringHash nameHash, BufferID bufferID)
    {
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

    void DescriptorSet::Bind(StringUtils::StringHash nameHash, TextureID textureID)
    {
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

    void DescriptorSet::Bind(StringUtils::StringHash nameHash, ImageID imageID, u32 mipLevel)
    {
        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_IMAGE;
                _boundDescriptors[i].imageID = imageID;
                _boundDescriptors[i].imageMipLevel = mipLevel;
                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_IMAGE;
        boundDescriptor.imageID = imageID;
        boundDescriptor.imageMipLevel = mipLevel;
    }

    void DescriptorSet::BindArray(StringUtils::StringHash nameHash, ImageID imageID, u32 mipLevel, u32 arrayIndex)
    {
        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash && arrayIndex == _boundDescriptors[i].arrayIndex)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_IMAGE_ARRAY;
                _boundDescriptors[i].imageID = imageID;
                _boundDescriptors[i].imageMipLevel = mipLevel;
                _boundDescriptors[i].arrayIndex = arrayIndex;

                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_IMAGE_ARRAY;
        boundDescriptor.imageID = imageID;
        boundDescriptor.imageMipLevel = mipLevel;
        boundDescriptor.arrayIndex = arrayIndex;
    }

    void DescriptorSet::Bind(StringUtils::StringHash nameHash, DepthImageID imageID)
    {

        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_DEPTH_IMAGE;
                _boundDescriptors[i].depthImageID = imageID;
                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_DEPTH_IMAGE;
        boundDescriptor.depthImageID = imageID;
    }

    void DescriptorSet::BindArray(StringUtils::StringHash nameHash, DepthImageID imageID, u32 arrayIndex)
    {
        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash && arrayIndex == _boundDescriptors[i].arrayIndex)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_DEPTH_IMAGE_ARRAY;
                _boundDescriptors[i].depthImageID = imageID;
                _boundDescriptors[i].arrayIndex = arrayIndex;

                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_DEPTH_IMAGE_ARRAY;
        boundDescriptor.depthImageID = imageID;
        boundDescriptor.arrayIndex = arrayIndex;
    }

    void DescriptorSet::BindStorage(StringUtils::StringHash nameHash, ImageID imageID, u32 mipLevel, u32 mipCount)
    {
        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_STORAGE_IMAGE;
                _boundDescriptors[i].imageID = imageID;
                _boundDescriptors[i].imageMipLevel = mipLevel;
                _boundDescriptors[i].count = mipCount;
                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_STORAGE_IMAGE;
        boundDescriptor.imageID = imageID;
        boundDescriptor.imageMipLevel = mipLevel;
        boundDescriptor.count = mipCount;
    }

    void DescriptorSet::BindStorageArray(StringUtils::StringHash nameHash, ImageID imageID, u32 mipLevel, u32 mipCount)
    {
        for (u32 i = 0; i < _boundDescriptors.size(); i++)
        {
            if (nameHash == _boundDescriptors[i].nameHash)
            {
                _boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_STORAGE_IMAGE_ARRAY;
                _boundDescriptors[i].imageID = imageID;
                _boundDescriptors[i].imageMipLevel = mipLevel;
                _boundDescriptors[i].count = mipCount;
                return;
            }
        }

        Descriptor& boundDescriptor = _boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_STORAGE_IMAGE_ARRAY;
        boundDescriptor.imageID = imageID;
        boundDescriptor.imageMipLevel = mipLevel;
        boundDescriptor.count = mipCount;
    }
}