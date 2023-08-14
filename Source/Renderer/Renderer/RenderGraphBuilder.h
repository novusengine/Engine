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
        RenderGraphBuilder(Memory::Allocator* allocator, Renderer* renderer, size_t numPasses, u32 numTotalBuffers);

        // Create transient resources
        ImageID Create(ImageDesc& desc);
        DepthImageID Create(DepthImageDesc& desc);

        // Reads
        ImageResource Read(ImageID id, PipelineType pipelineType);
        DepthImageResource Read(DepthImageID id, PipelineType pipelineType);
        BufferResource Read(BufferID id, BufferPassUsage bufferPassUsage);

        // Writes
        ImageMutableResource Write(ImageID id, PipelineType pipelineType, LoadMode loadMode);
        DepthImageMutableResource Write(DepthImageID id, PipelineType pipelineType, LoadMode loadMode);
        BufferMutableResource Write(BufferID id, BufferPassUsage bufferPassUsage);

        // Uses
        DescriptorSetResource Use(DescriptorSet& descriptorSet);

    private:
        void PrePass(CommandList& commandList, u32 currentPassIndex, const std::string& passName);
        void PostPass(CommandList& commandList, u32 currentPassIndex, const std::string& passName);
        RenderGraphResources& GetResources();

        void SetCurrentPassIndex(u32 index) { _currentPassIndex = index; }

        u32 GetNumPlacedImageBarriers() { return _numPlacedImageBarriers; }
        u32 GetNumPlacedBufferBarriers() { return _numPlacedBufferBarriers; }

    private:
        Memory::Allocator* _allocator;
        Renderer* _renderer;

        RenderGraphResources _resources;
        u32 _currentPassIndex;

        u32 _numPlacedImageBarriers = 0;
        u32 _numPlacedBufferBarriers = 0;

        static BitSet* _dirty;

        static BitSet* _lastWriteWasTransfer;
        static BitSet* _lastWriteWasGraphics;
        static BitSet* _lastWriteWasCompute;

        friend class RenderGraph;
    };
}