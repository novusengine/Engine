#include "DescriptorSetResource.h"

#include <Renderer/RenderGraphResources.h>
#include <Renderer/DescriptorSet.h>

namespace Renderer
{
    DescriptorSetResource::DescriptorSetResource()
        : _id(DescriptorSetID::Invalid())
        , _renderGraphResources(nullptr)
    {

    }

    DescriptorSetResource::DescriptorSetResource(DescriptorSetID id, RenderGraphResources& renderGraphResources)
        : _id(id)
        , _renderGraphResources(&renderGraphResources)
    {

    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, ImageResource resource, u32 mipLevel) const
    {
        ImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_IMAGE;
                boundDescriptors[i].imageID = imageID;
                boundDescriptors[i].imageMipLevel = mipLevel;
                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_IMAGE;
        boundDescriptor.imageID = imageID;
        boundDescriptor.imageMipLevel = mipLevel;
    }

    void DescriptorSetResource::BindArray(StringUtils::StringHash nameHash, ImageResource resource, u32 mipLevel, u32 arrayIndex) const
    {
        ImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash && arrayIndex == boundDescriptors[i].arrayIndex)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_IMAGE_ARRAY;
                boundDescriptors[i].imageID = imageID;
                boundDescriptors[i].imageMipLevel = mipLevel;
                boundDescriptors[i].arrayIndex = arrayIndex;

                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_IMAGE_ARRAY;
        boundDescriptor.imageID = imageID;
        boundDescriptor.imageMipLevel = mipLevel;
        boundDescriptor.arrayIndex = arrayIndex;
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, ImageMutableResource resource, u32 mipLevel) const
    {
        ImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_IMAGE;
                boundDescriptors[i].imageID = imageID;
                boundDescriptors[i].imageMipLevel = mipLevel;
                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_IMAGE;
        boundDescriptor.imageID = imageID;
        boundDescriptor.imageMipLevel = mipLevel;
    }

    void DescriptorSetResource::BindArray(StringUtils::StringHash nameHash, ImageMutableResource resource, u32 mipLevel, u32 arrayIndex) const
    {
        ImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash && arrayIndex == boundDescriptors[i].arrayIndex)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_IMAGE_ARRAY;
                boundDescriptors[i].imageID = imageID;
                boundDescriptors[i].imageMipLevel = mipLevel;
                boundDescriptors[i].arrayIndex = arrayIndex;

                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_IMAGE_ARRAY;
        boundDescriptor.imageID = imageID;
        boundDescriptor.imageMipLevel = mipLevel;
        boundDescriptor.arrayIndex = arrayIndex;
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, DepthImageResource resource) const
    {
        DepthImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_DEPTH_IMAGE;
                boundDescriptors[i].depthImageID = imageID;
                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_DEPTH_IMAGE;
        boundDescriptor.depthImageID = imageID;
    }

    void DescriptorSetResource::BindArray(StringUtils::StringHash nameHash, DepthImageResource resource, u32 arrayIndex) const
    {
        DepthImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash && arrayIndex == boundDescriptors[i].arrayIndex)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_DEPTH_IMAGE_ARRAY;
                boundDescriptors[i].depthImageID = imageID;
                boundDescriptors[i].arrayIndex = arrayIndex;

                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_DEPTH_IMAGE_ARRAY;
        boundDescriptor.depthImageID = imageID;
        boundDescriptor.arrayIndex = arrayIndex;
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, DepthImageMutableResource resource) const
    {
        DepthImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_DEPTH_IMAGE;
                boundDescriptors[i].depthImageID = imageID;
                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_DEPTH_IMAGE;
        boundDescriptor.depthImageID = imageID;
    }

    void DescriptorSetResource::BindArray(StringUtils::StringHash nameHash, DepthImageMutableResource resource, u32 arrayIndex) const
    {
        DepthImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash && arrayIndex == boundDescriptors[i].arrayIndex)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_DEPTH_IMAGE_ARRAY;
                boundDescriptors[i].depthImageID = imageID;
                boundDescriptors[i].arrayIndex = arrayIndex;

                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_DEPTH_IMAGE_ARRAY;
        boundDescriptor.depthImageID = imageID;
        boundDescriptor.arrayIndex = arrayIndex;
    }

    void DescriptorSetResource::BindStorage(StringUtils::StringHash nameHash, ImageMutableResource resource, u32 mipLevel, u32 mipCount) const
    {
        ImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_STORAGE_IMAGE;
                boundDescriptors[i].imageID = imageID;
                boundDescriptors[i].imageMipLevel = mipLevel;
                boundDescriptors[i].count = mipCount;
                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_STORAGE_IMAGE;
        boundDescriptor.imageID = imageID;
        boundDescriptor.imageMipLevel = mipLevel;
        boundDescriptor.count = mipCount;
    }

    void DescriptorSetResource::BindStorageArray(StringUtils::StringHash nameHash, ImageMutableResource resource, u32 mipLevel, u32 mipCount) const
    {
        ImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_STORAGE_IMAGE_ARRAY;
                boundDescriptors[i].imageID = imageID;
                boundDescriptors[i].imageMipLevel = mipLevel;
                boundDescriptors[i].count = mipCount;
                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_STORAGE_IMAGE_ARRAY;
        boundDescriptor.imageID = imageID;
        boundDescriptor.imageMipLevel = mipLevel;
        boundDescriptor.count = mipCount;
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, BufferResource resource)
    {
        BufferID bufferID = _renderGraphResources->GetBuffer(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_BUFFER;
                boundDescriptors[i].bufferID = bufferID;
                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_BUFFER;
        boundDescriptor.bufferID = bufferID;
    }

    void DescriptorSetResource::BindArray(StringUtils::StringHash nameHash, BufferResource resource, u32 arrayIndex)
    {
        BufferID bufferID = _renderGraphResources->GetBuffer(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash && arrayIndex == boundDescriptors[i].arrayIndex)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_BUFFER_ARRAY;
                boundDescriptors[i].bufferID = bufferID;
                boundDescriptors[i].arrayIndex = arrayIndex;

                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_BUFFER_ARRAY;
        boundDescriptor.bufferID = bufferID;
        boundDescriptor.arrayIndex = arrayIndex;
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, BufferMutableResource resource)
    {
        BufferID bufferID = _renderGraphResources->GetBuffer(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_BUFFER;
                boundDescriptors[i].bufferID = bufferID;
                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_BUFFER;
        boundDescriptor.bufferID = bufferID;
    }

    void DescriptorSetResource::BindArray(StringUtils::StringHash nameHash, BufferMutableResource resource, u32 arrayIndex)
    {
        BufferID bufferID = _renderGraphResources->GetBuffer(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);

        std::vector<Descriptor>& boundDescriptors = descriptorSet->GetMutableDescriptors();

        for (u32 i = 0; i < boundDescriptors.size(); i++)
        {
            if (nameHash == boundDescriptors[i].nameHash && arrayIndex == boundDescriptors[i].arrayIndex)
            {
                boundDescriptors[i].descriptorType = DescriptorType::DESCRIPTOR_TYPE_BUFFER_ARRAY;
                boundDescriptors[i].bufferID = bufferID;
                boundDescriptors[i].arrayIndex = arrayIndex;

                return;
            }
        }

        Descriptor& boundDescriptor = boundDescriptors.emplace_back();
        boundDescriptor.nameHash = nameHash;
        boundDescriptor.descriptorType = DESCRIPTOR_TYPE_BUFFER_ARRAY;
        boundDescriptor.bufferID = bufferID;
        boundDescriptor.arrayIndex = arrayIndex;
    }
}