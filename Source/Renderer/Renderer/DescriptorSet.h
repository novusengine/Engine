#pragma once
#include "DescriptorMeta.h"
#include "RenderPassResources.h"
#include "Descriptors/BufferDesc.h"
#include "Descriptors/ComputePipelineDesc.h"
#include "Descriptors/DepthImageDesc.h"
#include "Descriptors/DescriptorSetDesc.h"
#include "Descriptors/GraphicsPipelineDesc.h"
#include "Descriptors/ImageDesc.h"
#include "Descriptors/SamplerDesc.h"
#include "Descriptors/TextureArrayDesc.h"
#include "Descriptors/TextureDesc.h"

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

#include <FileFormat/Novus/ShaderPack/ShaderPack.h>

namespace Renderer
{
    class Renderer;
    class RenderGraphResources;

    enum DescriptorSetSlot
    {
        // 1 indexed because we let 0 be the push constants, it's not technically correct but we get nice error checking if we do this
        DEBUG = 1,
        GLOBAL,
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
        DescriptorSet(DescriptorSetSlot slot) : _slot(slot) 
        {
            _combinedReflection.slot = static_cast<u32>(slot);
        };

        void RegisterPipeline(Renderer* renderer, ComputePipelineID pipelineID);
        void RegisterPipeline(Renderer* renderer, GraphicsPipelineID pipelineID);
        void Init(Renderer* renderer);
        bool IsInitialized() const { return _initialized; }

        void Bind(StringUtils::StringHash nameHash, BufferID bufferID, bool optional = false);

        void Bind(StringUtils::StringHash nameHash, SamplerID samplerID, bool optional = false);
        void BindArray(StringUtils::StringHash nameHash, SamplerID samplerID, u32 arrayIndex, bool optional = false);

        void Bind(StringUtils::StringHash nameHash, TextureArrayID textureArrayID, bool optional = false);

        DescriptorSetID GetID() const { return _descriptorSetID; }
        DescriptorSetSlot GetSlot() const { return _slot; }

    private:
        bool HasBinding(StringUtils::StringHash nameHash) const;
        u32 GetBindingIndex(StringUtils::StringHash nameHash) const;
        FileFormat::DescriptorReflection& GetDescriptorReflection(u32 bindingIndex);

        void Lock() { _locked = true; }
        void Unlock() { _locked = false; }

    private:
        Renderer* _renderer = nullptr;
        bool _initialized = false;

        DescriptorSetSlot _slot;
        DescriptorMetaInfo _metaInfo;

        FileFormat::DescriptorSetReflection _combinedReflection;
        robin_hood::unordered_map<u32, u32> _nameHashToBindingIndex;
        DescriptorSetID _descriptorSetID;

        bool _locked = false;

        friend class DescriptorSetResource;
        friend class RenderGraphBuilder;
    };
}