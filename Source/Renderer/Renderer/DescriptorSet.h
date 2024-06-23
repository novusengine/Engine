#pragma once
#include "RenderPassResources.h"
#include "Descriptors/BufferDesc.h"
#include "Descriptors/SamplerDesc.h"
#include "Descriptors/TextureDesc.h"
#include "Descriptors/ImageDesc.h"
#include "Descriptors/DepthImageDesc.h"
#include "Descriptors/TextureArrayDesc.h"

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

namespace Renderer
{
    class RenderGraphResources;

    enum DescriptorType
    {
        DESCRIPTOR_TYPE_BUFFER,
        DESCRIPTOR_TYPE_BUFFER_ARRAY,
        DESCRIPTOR_TYPE_SAMPLER,
        DESCRIPTOR_TYPE_TEXTURE,
        DESCRIPTOR_TYPE_TEXTURE_ARRAY,
        DESCRIPTOR_TYPE_IMAGE,
        DESCRIPTOR_TYPE_IMAGE_ARRAY,
        DESCRIPTOR_TYPE_DEPTH_IMAGE,
        DESCRIPTOR_TYPE_DEPTH_IMAGE_ARRAY,
        DESCRIPTOR_TYPE_STORAGE_IMAGE,
        DESCRIPTOR_TYPE_STORAGE_IMAGE_ARRAY,
    };

    struct Descriptor
    {
        u32 nameHash;
        u32 imageMipLevel;
        u32 count = 1;
        u32 arrayIndex = 0;
        DescriptorType descriptorType;

        TextureID textureID;
        ImageID imageID;
        DepthImageID depthImageID;
        SamplerID samplerID;
        TextureArrayID textureArrayID;
        BufferID bufferID;
    };

    enum DescriptorSetSlot
    {
        DEBUG,
        GLOBAL,
        SHADOWS,
        PER_PASS,
        PER_DRAW,
        TERRAIN,
        MODEL
    };

    inline const char* DescriptorSetToName(DescriptorSetSlot slot)
    {
        switch (slot)
        {
            case DEBUG: return "DEBUG";
            case GLOBAL: return "GLOBAL";
            case SHADOWS: return "SHADOWS";
            case PER_PASS: return "PER_PASS";
            case PER_DRAW: return "PER_DRAW";
            case TERRAIN: return "TERRAIN";
            case MODEL: return "MODEL";
            default:
                NC_LOG_CRITICAL("Unknown DescriptorSet, did we forget to extend the DescriptorSetToName function after adding new DescriptorSetSlots?");
        }
        return "";
    }

    class DescriptorSet
    {
    public:
        
    public:
        DescriptorSet()
        {}

        void Bind(StringUtils::StringHash nameHash, BufferID bufferID);
        void BindArray(StringUtils::StringHash nameHash, BufferID bufferID, u32 arrayIndex);

        void Bind(StringUtils::StringHash nameHash, SamplerID samplerID);

        void Bind(StringUtils::StringHash nameHash, TextureID textureID);

        void Bind(StringUtils::StringHash nameHash, TextureArrayID textureArrayID);

        const std::vector<Descriptor>& GetDescriptors() const { return _boundDescriptors; }

    private:
        std::vector<Descriptor>& GetMutableDescriptors() { return _boundDescriptors; }

        void Lock() { _locked = true; }
        void Unlock() { _locked = false; }

    private:
        std::vector<Descriptor> _boundDescriptors;
        bool _locked = false;

        friend class DescriptorSetResource;
        friend class RenderGraphBuilder;
    };
}