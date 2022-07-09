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

    class RenderGraphBuilder
    {
    public:
        RenderGraphBuilder(Memory::Allocator* allocator, Renderer* renderer, size_t numPasses);

        enum class WriteMode : u8
        {
            RENDERTARGET,
            UAV
        };

        enum class LoadMode : u8
        {
            LOAD, // Load the contents of the resource
            DISCARD, // We don't really care
            CLEAR // Clear the resource of existing data
        };

        enum class ShaderStage : u8
        {
            NONE,
            VERTEX,
            PIXEL,
            COMPUTE
        };

        // Create transient resources
        ImageID Create(ImageDesc& desc);
        DepthImageID Create(DepthImageDesc& desc);

        // Reads
        RenderPassResource Read(ImageID id, ShaderStage shaderStage);
        RenderPassResource Read(TextureID id, ShaderStage shaderStage);
        RenderPassResource Read(DepthImageID id, ShaderStage shaderStage);

        // Writes
        RenderPassMutableResource Write(ImageID id, WriteMode writeMode, LoadMode loadMode);
        RenderPassMutableResource Write(DepthImageID id, WriteMode writeMode, LoadMode loadMode);

    private:
        void PreExecute(CommandList& commandList, u32 passIndex);
        void PostExecute(CommandList& commandList, u32 passIndex);
        RenderGraphResources& GetResources();

        void SetCurrentPassIndex(u32 index) { _currentPassIndex = index; }

    private:
        Memory::Allocator* _allocator;
        Renderer* _renderer;

        RenderGraphResources _resources;
        u32 _currentPassIndex;

        friend class RenderGraph;
    };
}