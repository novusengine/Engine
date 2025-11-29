#pragma once
#include "DescriptorMeta.h"
#include "RenderPassResources.h"
#include "Descriptors/BufferDesc.h"
#include "Descriptors/ComputePipelineDesc.h"
#include "Descriptors/DepthImageDesc.h"
#include "Descriptors/GraphicsPipelineDesc.h"
#include "Descriptors/ImageDesc.h"
#include "Descriptors/SamplerDesc.h"
#include "Descriptors/TextureArrayDesc.h"
#include "Descriptors/TextureDesc.h"

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

namespace Renderer
{
    class Renderer;
    class RenderGraphResources;

    enum DescriptorType
    {
        DESCRIPTOR_TYPE_BUFFER,
        DESCRIPTOR_TYPE_BUFFER_ARRAY,
        DESCRIPTOR_TYPE_SAMPLER,
        DESCRIPTOR_TYPE_SAMPLER_ARRAY,
        DESCRIPTOR_TYPE_TEXTURE,
        DESCRIPTOR_TYPE_TEXTURE_ARRAY,
        DESCRIPTOR_TYPE_TEXTURE_WRITE,
        DESCRIPTOR_TYPE_TEXTURE_READ_WRITE,
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
        TILES,
        LIGHT,
        TERRAIN,
        MODEL,
        PER_PASS,
        PER_DRAW
    };

    inline const char* DescriptorSetToName(DescriptorSetSlot slot)
    {
        switch (slot)
        {
            case DEBUG: return "DEBUG";
            case GLOBAL: return "GLOBAL";
            case TILES: return "TILES";
            case LIGHT: return "LIGHT";
            case TERRAIN: return "TERRAIN";
            case MODEL: return "MODEL";
            case PER_PASS: return "PER_PASS";
            case PER_DRAW: return "PER_DRAW";
            default:
                NC_LOG_CRITICAL("Unknown DescriptorSet, did we forget to extend the DescriptorSetToName function after adding new DescriptorSetSlots?");
        }
        return "";
    }

    class DescriptorSet
    {
    public:
        
    public:
        DescriptorSet(DescriptorSetSlot slot) : _slot(slot) {};

        void RegisterPipeline(Renderer* renderer, ComputePipelineID pipelineID);
        void RegisterPipeline(Renderer* renderer, GraphicsPipelineID pipelineID);

        void Bind(StringUtils::StringHash nameHash, BufferID bufferID);
        void BindArray(StringUtils::StringHash nameHash, BufferID bufferID, u32 arrayIndex);

        void Bind(StringUtils::StringHash nameHash, SamplerID samplerID);
        void BindArray(StringUtils::StringHash nameHash, SamplerID samplerID, u32 arrayIndex);

        void Bind(StringUtils::StringHash nameHash, TextureID textureID);

        void Bind(StringUtils::StringHash nameHash, TextureArrayID textureArrayID);

        const std::vector<Descriptor>& GetDescriptors() const { return _boundDescriptors; }

    private:
        std::vector<Descriptor>& GetMutableDescriptors() { return _boundDescriptors; }

        void Lock() { _locked = true; }
        void Unlock() { _locked = false; }

    private:
        DescriptorSetSlot _slot;
        DescriptorMetaInfo _metaInfo;

        std::vector<Descriptor> _boundDescriptors;
        bool _locked = false;

        friend class DescriptorSetResource;
        friend class RenderGraphBuilder;
    };
}