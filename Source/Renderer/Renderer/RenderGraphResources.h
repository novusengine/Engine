#pragma once
#include "RenderPassResources.h"
#include "Descriptors/TextureDesc.h"
#include "Descriptors/ImageDesc.h"
#include "Descriptors/DepthImageDesc.h"

#include <Base/Types.h>
#include <Base/Memory/Allocator.h>
#include <Base/Container/DynamicArray.h>

namespace Renderer
{
    struct GraphicsPipelineDesc;
    struct ComputePipelineDesc;

    struct IRenderGraphResourcesData {};

    class RenderGraphResources
    {
    public:
        void InitializePipelineDesc(GraphicsPipelineDesc& desc);
        void InitializePipelineDesc(ComputePipelineDesc& desc);

        template<typename T, typename... Args>
        T* FrameNew(Args... args)
        {
            return Memory::Allocator::New<T>(_allocator, args...);
        }

    private:
        RenderGraphResources(Memory::Allocator* allocator, size_t numPasses);

        ImageID GetImage(RenderPassResource resource);
        ImageID GetImage(RenderPassMutableResource resource);
        DepthImageID GetDepthImage(RenderPassResource resource);
        DepthImageID GetDepthImage(RenderPassMutableResource resource);

        RenderPassResource GetResource(ImageID id);
        RenderPassResource GetResource(TextureID id);
        RenderPassResource GetResource(DepthImageID id);
        RenderPassMutableResource GetMutableResource(ImageID id);
        RenderPassMutableResource GetMutableResource(DepthImageID id);

        void Clear(u32 passIndex, ImageID id);
        void Clear(u32 passIndex, DepthImageID id);

        bool NeedsPreExecute(u32 passIndex);
        bool NeedsPostExecute(u32 passIndex);

        const DynamicArray<ImageID>& GetColorClears(u32 passIndex);
        const DynamicArray<DepthImageID>& GetDepthClears(u32 passIndex);

    private:
        Memory::Allocator* _allocator = nullptr;

        IRenderGraphResourcesData* _data = nullptr;

        friend class RenderGraphBuilder;
    };
}