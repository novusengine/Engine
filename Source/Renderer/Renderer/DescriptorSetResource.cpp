#include "DescriptorSetResource.h"

#include <Renderer/DescriptorSet.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderGraphResources.h>

#include <tracy/Tracy.hpp>

namespace Renderer
{
    DescriptorSetResource::DescriptorSetResource()
        : _id(DescriptorSetResourceID::Invalid())
        , _renderGraphResources(nullptr)
    {

    }

    DescriptorSetResource::DescriptorSetResource(DescriptorSetResourceID id, RenderGraphResources& renderGraphResources)
        : _id(id)
        , _renderGraphResources(&renderGraphResources)
    {

    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, ImageResource resource, u32 mipLevel) const
    {
        ZoneScoped;
        ImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);
        if (!descriptorSet->_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        DescriptorSetID descriptorSetID = descriptorSet->GetID();

        u32 bindingIndex = descriptorSet->GetBindingIndex(nameHash.computedHash);

        // Verify non-mutable access type
        FileFormat::DescriptorReflection& descriptorReflection = descriptorSet->GetDescriptorReflection(bindingIndex);
        if (descriptorReflection.accessType != FileFormat::DescriptorAccessTypeReflection::Read)
        {
            NC_LOG_CRITICAL("DescriptorSetResource::Bind: Tried to bind non-mutable ImageResources to a descriptor namehash '{}' that is not a read-only descriptor", nameHash.computedHash);
        }

        u32 frameIndex = descriptorSet->_renderer->GetCurrentFrameIndex();
        descriptorSet->_renderer->BindDescriptor(descriptorSetID, bindingIndex, imageID, mipLevel, DescriptorType::SampledImage, frameIndex);
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, ImageMutableResource resource, u32 mipLevel) const
    {
        ZoneScoped;
        ImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);
        if (!descriptorSet->_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        DescriptorSetID descriptorSetID = descriptorSet->GetID();

        u32 bindingIndex = descriptorSet->GetBindingIndex(nameHash.computedHash);

        FileFormat::DescriptorReflection& descriptorReflection = descriptorSet->GetDescriptorReflection(bindingIndex);
        DescriptorType descriptorType = (descriptorReflection.accessType == FileFormat::DescriptorAccessTypeReflection::Read) ? DescriptorType::SampledImage : DescriptorType::StorageImage;

        u32 frameIndex = descriptorSet->_renderer->GetCurrentFrameIndex();
        descriptorSet->_renderer->BindDescriptor(descriptorSetID, bindingIndex, imageID, mipLevel, descriptorType, frameIndex);
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, ImageMutableResource resource, u32 mipLevel, u32 mipCount) const
    {
        ZoneScoped;
        ImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);
        if (!descriptorSet->_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        DescriptorSetID descriptorSetID = descriptorSet->GetID();

        u32 bindingIndex = descriptorSet->GetBindingIndex(nameHash.computedHash);

        FileFormat::DescriptorReflection& descriptorReflection = descriptorSet->GetDescriptorReflection(bindingIndex);
        DescriptorType descriptorType = (descriptorReflection.accessType == FileFormat::DescriptorAccessTypeReflection::Read) ? DescriptorType::SampledImage : DescriptorType::StorageImage;

        u32 frameIndex = descriptorSet->_renderer->GetCurrentFrameIndex();
        descriptorSet->_renderer->BindDescriptorArray(descriptorSetID, bindingIndex, imageID, mipLevel, mipCount, descriptorType, frameIndex);
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, DepthImageResource resource) const
    {
        ZoneScoped;
        DepthImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);
        if (!descriptorSet->_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        DescriptorSetID descriptorSetID = descriptorSet->GetID();

        u32 bindingIndex = descriptorSet->GetBindingIndex(nameHash.computedHash);

        // Verify non-mutable access type
        FileFormat::DescriptorReflection& descriptorReflection = descriptorSet->GetDescriptorReflection(bindingIndex);
        if (descriptorReflection.accessType != FileFormat::DescriptorAccessTypeReflection::Read)
        {
            NC_LOG_CRITICAL("DescriptorSetResource::Bind: Tried to bind non-mutable DepthImageResource to a descriptor namehash '{}' that is not a read-only descriptor", nameHash.computedHash);
        }

        u32 frameIndex = descriptorSet->_renderer->GetCurrentFrameIndex();
        descriptorSet->_renderer->BindDescriptor(descriptorSetID, bindingIndex, imageID, DescriptorType::SampledImage, frameIndex);
    }

    void DescriptorSetResource::BindArray(StringUtils::StringHash nameHash, DepthImageResource resource, u32 arrayIndex) const
    {
        ZoneScoped;
        DepthImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);
        if (!descriptorSet->_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        DescriptorSetID descriptorSetID = descriptorSet->GetID();

        u32 bindingIndex = descriptorSet->GetBindingIndex(nameHash.computedHash);

        // Verify non-mutable access type
        FileFormat::DescriptorReflection& descriptorReflection = descriptorSet->GetDescriptorReflection(bindingIndex);
        if (descriptorReflection.accessType != FileFormat::DescriptorAccessTypeReflection::Read)
        {
            NC_LOG_CRITICAL("DescriptorSetResource::Bind: Tried to bind non-mutable DepthImageResource to a descriptor namehash '{}' that is not a read-only descriptor", nameHash.computedHash);
        }

        u32 frameIndex = descriptorSet->_renderer->GetCurrentFrameIndex();
        descriptorSet->_renderer->BindDescriptorArray(descriptorSetID, bindingIndex, imageID, arrayIndex, DescriptorType::SampledImage, frameIndex);
    }

    void DescriptorSetResource::BindArray(StringUtils::StringHash nameHash, DepthImageMutableResource resource, u32 arrayIndex) const
    {
        ZoneScoped;
        DepthImageID imageID = _renderGraphResources->GetImage(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);
        if (!descriptorSet->_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        DescriptorSetID descriptorSetID = descriptorSet->GetID();

        u32 bindingIndex = descriptorSet->GetBindingIndex(nameHash.computedHash);

        FileFormat::DescriptorReflection& descriptorReflection = descriptorSet->GetDescriptorReflection(bindingIndex);
        DescriptorType descriptorType = (descriptorReflection.accessType == FileFormat::DescriptorAccessTypeReflection::Read) ? DescriptorType::SampledImage : DescriptorType::StorageImage;

        u32 frameIndex = descriptorSet->_renderer->GetCurrentFrameIndex();
        descriptorSet->_renderer->BindDescriptorArray(descriptorSetID, bindingIndex, imageID, arrayIndex, descriptorType, frameIndex);
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, BufferResource resource)
    {
        ZoneScoped;
        BufferID bufferID = _renderGraphResources->GetBuffer(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);
        if (!descriptorSet->_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        DescriptorSetID descriptorSetID = descriptorSet->GetID();

        u32 bindingIndex = descriptorSet->GetBindingIndex(nameHash.computedHash);

        FileFormat::DescriptorReflection& descriptorReflection = descriptorSet->GetDescriptorReflection(bindingIndex);

        u32 frameIndex = descriptorSet->_renderer->GetCurrentFrameIndex();
        descriptorSet->_renderer->BindDescriptor(descriptorSetID, bindingIndex, bufferID, DescriptorType::StorageBuffer, frameIndex);
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, BufferMutableResource resource)
    {
        ZoneScoped;
        BufferID bufferID = _renderGraphResources->GetBuffer(resource);
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);
        if (!descriptorSet->_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        DescriptorSetID descriptorSetID = descriptorSet->GetID();

        u32 bindingIndex = descriptorSet->GetBindingIndex(nameHash.computedHash);

        FileFormat::DescriptorReflection& descriptorReflection = descriptorSet->GetDescriptorReflection(bindingIndex);

        u32 frameIndex = descriptorSet->_renderer->GetCurrentFrameIndex();
        descriptorSet->_renderer->BindDescriptor(descriptorSetID, bindingIndex, bufferID, DescriptorType::StorageBuffer, frameIndex);
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, TextureID textureID, u32 mipLevel)
    {
        ZoneScoped;
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);
        if (!descriptorSet->_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        u32 bindingIndex = descriptorSet->GetBindingIndex(nameHash.computedHash);
        FileFormat::DescriptorReflection& descriptorReflection = descriptorSet->GetDescriptorReflection(bindingIndex);

        DescriptorType descriptorType = (descriptorReflection.accessType == FileFormat::DescriptorAccessTypeReflection::Read) ? DescriptorType::SampledImage : DescriptorType::StorageImage;

        DescriptorSetID descriptorSetID = descriptorSet->GetID();
        u32 frameIndex = descriptorSet->_renderer->GetCurrentFrameIndex();
        descriptorSet->_renderer->BindDescriptor(descriptorSetID, bindingIndex, textureID, mipLevel, descriptorType, frameIndex);
    }

    void DescriptorSetResource::Bind(StringUtils::StringHash nameHash, TextureID textureID, u32 mipLevel, u32 mipCount)
    {
        ZoneScoped;
        DescriptorSet* descriptorSet = _renderGraphResources->GetDescriptorSet(_id);
        if (!descriptorSet->_initialized)
        {
            NC_LOG_CRITICAL("DescriptorSet : Tried to Bind to a DescriptorSet that has not been initialized yet.");
        }

        u32 bindingIndex = descriptorSet->GetBindingIndex(nameHash.computedHash);
        FileFormat::DescriptorReflection& descriptorReflection = descriptorSet->GetDescriptorReflection(bindingIndex);

        DescriptorType descriptorType = (descriptorReflection.accessType == FileFormat::DescriptorAccessTypeReflection::Read) ? DescriptorType::SampledImage : DescriptorType::StorageImage;

        DescriptorSetID descriptorSetID = descriptorSet->GetID();
        u32 frameIndex = descriptorSet->_renderer->GetCurrentFrameIndex();
        descriptorSet->_renderer->BindDescriptorArray(descriptorSetID, bindingIndex, textureID, mipLevel, mipCount, descriptorType, frameIndex);
    }
}