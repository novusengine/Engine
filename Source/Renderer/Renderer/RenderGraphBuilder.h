#pragma once
#include <Base/Types.h>

#include "RenderGraphResources.h"

#include "RenderStates.h"
#include "RenderPassResources.h"

#include "Descriptors/TextureDesc.h"
#include "Descriptors/ImageDesc.h"
#include "Descriptors/DepthImageDesc.h"

namespace Memory
{
    class Allocator;
}

namespace Renderer
{
    class Renderer;
    class CommandList;
    class RenderGraph;
    class DescriptorSetResource;

    class RenderGraphBuilder
    {
    public:
        RenderGraphBuilder(Memory::Allocator* allocator, Renderer* renderer, size_t numPasses);

        // Create transient resources
        ImageID Create(ImageDesc& desc);
        DepthImageID Create(DepthImageDesc& desc);

        // Reads
        ImageResource Read(ImageID id, PipelineType pipelineType);
        ImageResource Read(TextureID id, PipelineType pipelineType);
        DepthImageResource Read(DepthImageID id, PipelineType pipelineType);

        // Writes
        ImageMutableResource Write(ImageID id, PipelineType pipelineType, LoadMode loadMode);
        DepthImageMutableResource Write(DepthImageID id, PipelineType pipelineType, LoadMode loadMode);

        // Uses
        DescriptorSetResource Use(DescriptorSet& descriptorSet);

    private:
        void PreExecute(CommandList& commandList, u32 currentPassIndex);
        void PostExecute(CommandList& commandList, u32 currentPassIndex);
        RenderGraphResources& GetResources();

        void SetCurrentPassIndex(u32 index) { _currentPassIndex = index; }

        u32 GetNumPlacedBarriers() { return _numPlacedBarriers; }

    private:
        Memory::Allocator* _allocator;
        Renderer* _renderer;

        RenderGraphResources _resources;
        u32 _currentPassIndex;

        u32 _numPlacedBarriers = 0;

        friend class RenderGraph;
    };
}