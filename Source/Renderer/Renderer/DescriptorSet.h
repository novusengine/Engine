#pragma once
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
        MAPOBJECT,
        CMODEL
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
            case MAPOBJECT: return "MAPOBJECT";
            case CMODEL: return "CMODEL";
            default:
                DebugHandler::PrintFatal("Unknown DescriptorSet, did we forget to extend the DescriptorSetToName function after adding new DescriptorSetSlots?");
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

        void Bind(StringUtils::StringHash nameHash, ImageID imageID, u32 mipLevel = 0);
        void BindArray(StringUtils::StringHash nameHash, ImageID imageID, u32 mipLevel, u32 arrayIndex);

        void Bind(StringUtils::StringHash nameHash, DepthImageID imageID);
        void BindArray(StringUtils::StringHash nameHash, DepthImageID imageID, u32 arrayIndex);

        void BindStorage(StringUtils::StringHash nameHash, ImageID imageID, u32 mipLevel = 0, u32 mipCount = 1);
        void BindStorageArray(StringUtils::StringHash nameHash, ImageID imageID, u32 mipLevel = 0, u32 mipCount = 1);

        const std::vector<Descriptor>& GetDescriptors() const { return _boundDescriptors; }

    private:

    private:
        std::vector<Descriptor> _boundDescriptors;
    };
}