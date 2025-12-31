#pragma once

#include <Renderer/Descriptors/TextureDesc.h>
#include <Renderer/RenderPassResources.h>

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

namespace Renderer
{
    class DescriptorSet;
    class RenderGraphResources;

    STRONG_TYPEDEF(DescriptorSetResourceID, u16);

    class DescriptorSetResource
    {
    public:
        DescriptorSetResource();
        DescriptorSetResource(DescriptorSetResourceID id, RenderGraphResources& renderGraphResources);

        // Image resources
        void Bind(StringUtils::StringHash nameHash, ImageResource resource, u32 mipLevel = 0) const;

        void Bind(StringUtils::StringHash nameHash, ImageMutableResource resource, u32 mipLevel = 0) const;
        void Bind(StringUtils::StringHash nameHash, ImageMutableResource resource, u32 mipLevel, u32 mipCount) const;

        void Bind(StringUtils::StringHash nameHash, DepthImageResource resource) const;
        void BindArray(StringUtils::StringHash nameHash, DepthImageResource resource, u32 arrayIndex) const;

        void BindArray(StringUtils::StringHash nameHash, DepthImageMutableResource resource, u32 arrayIndex) const;

        // Buffer resources
        void Bind(StringUtils::StringHash nameHash, BufferResource resource);

        void Bind(StringUtils::StringHash nameHash, BufferMutableResource resource);

        // Texture
        void Bind(StringUtils::StringHash nameHash, TextureID textureID, u32 mipLevel = 0);
        void Bind(StringUtils::StringHash nameHash, TextureID textureID, u32 mipLevel, u32 mipCount);

        DescriptorSetResourceID GetID() { return _id; }

    private:
        DescriptorSetResourceID _id;

        RenderGraphResources* _renderGraphResources;
    };
}