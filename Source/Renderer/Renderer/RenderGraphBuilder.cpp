#include "RenderGraphBuilder.h"
#include "Renderer.h"
#include "RenderGraph.h"

namespace Renderer
{

    RenderGraphBuilder::RenderGraphBuilder(Memory::Allocator* allocator, Renderer* renderer, size_t numPasses)
        : _allocator(allocator)
        , _renderer(renderer)
        , _resources(allocator, renderer, numPasses)
    {

    }

    void RenderGraphBuilder::PreExecute(CommandList& commandList, u32 currentPassIndex)
    {
        if (!_resources.NeedsPreExecute(currentPassIndex))
            return;

        // Queue up all the clears for this pass
        commandList.PushMarker("RenderGraph::PreExecute", Color::White);

        const DynamicArray<ImageMutableResource>& colorClears = _resources.GetColorClears(currentPassIndex);
        for (ImageMutableResource resource : colorClears)
        {
            ImageID imageID = _resources.GetImage(resource);
            const ImageDesc& imageDesc = _renderer->GetImageDesc(imageID);

            ImageComponentType imageComponentType = ToImageComponentType(imageDesc.format);

            if (imageComponentType == ImageComponentType::FLOAT || imageComponentType == ImageComponentType::UNORM || imageComponentType == ImageComponentType::SNORM)
            {
                commandList.Clear(resource, imageDesc.clearColor);
            }
            else if (imageComponentType == ImageComponentType::UINT)
            {
                commandList.Clear(resource, imageDesc.clearUInts);
            }
            else if (imageComponentType == ImageComponentType::SINT)
            {
                commandList.Clear(resource, imageDesc.clearInts);
            }
            else
            {
                DebugHandler::PrintFatal("Please implement more of these");
            }

            _resources.SetLastBarrier(imageID, currentPassIndex);
        }

        const DynamicArray<DepthImageMutableResource>& depthClears = _resources.GetDepthClears(currentPassIndex);
        for (DepthImageMutableResource resource : depthClears)
        {
            DepthImageID imageID = _resources.GetImage(resource);

            const DepthImageDesc& imageDesc = _renderer->GetImageDesc(imageID);
            commandList.Clear(resource, imageDesc.depthClearValue);

            _resources.SetLastBarrier(imageID, currentPassIndex);
        }

        // Handling between-pass barriers
        const DynamicArray<TrackedImageAccess>& imageAccesses = _resources.GetImageAccesses(currentPassIndex);
        for (const TrackedImageAccess& imageAccess : imageAccesses)
        {
            u32 lastBarrier = _resources.GetLastBarrier(imageAccess.imageID);

            const DynamicArray<TrackedPassAccess>& passAccesses = _resources.GetPassAccesses(imageAccess.imageID);

            for (i32 i = static_cast<i32>(passAccesses.Count()) - 1; i >= 0; i--)
            {
                const TrackedPassAccess& passAccess = passAccesses[i];

                if (passAccess.passIndex >= currentPassIndex)
                {
                    // Keep iterating until we get to previous passes
                    continue;
                }

                if (passAccess.passIndex < lastBarrier)
                {
                    // We've stepped back until our last barrier, the image is already synced
                    break;
                }

                if (imageAccess.pipelineType == PipelineType::GRAPHICS  && passAccess.pipelineType == PipelineType::GRAPHICS && imageAccess.accessType == AccessType::WRITE && passAccess.accessType == AccessType::WRITE)
                {
                    // We do not need to add ImageBarriers between graphics pipelines that both write to the rendertarget, that is handled by the pipeline
                    break;
                }

                if (passAccess.accessType == AccessType::WRITE)
                {
                    _resources.SetLastBarrier(imageAccess.imageID, currentPassIndex);

                    if (imageAccess.accessType == AccessType::WRITE)
                    {
                        ImageMutableResource resource = _resources.GetMutableResource(imageAccess.imageID);
                        commandList.ImageBarrier(resource);
                        _numPlacedBarriers++;
                    }
                    else
                    {
                        ImageResource resource = _resources.GetResource(imageAccess.imageID);
                        commandList.ImageBarrier(resource);
                        _numPlacedBarriers++;
                    }

                    break;
                }
            }
        }

        const DynamicArray<TrackedDepthImageAccess>& depthImageAccesses = _resources.GetDepthImageAccesses(currentPassIndex);
        for (const TrackedDepthImageAccess& imageAccess : depthImageAccesses)
        {
            u32 lastBarrier = _resources.GetLastBarrier(imageAccess.imageID);

            const DynamicArray<TrackedPassAccess>& passAccesses = _resources.GetPassAccesses(imageAccess.imageID);

            for (i32 i = static_cast<i32>(passAccesses.Count()) - 1; i >= 0; i--)
            {
                const TrackedPassAccess& passAccess = passAccesses[i];

                if (passAccess.passIndex >= currentPassIndex)
                {
                    // Keep iterating until we get to previous passes
                    continue;
                }

                if (passAccess.passIndex < lastBarrier)
                {
                    // We've stepped back until our last barrier, the image is already synced
                    break;
                }

                if (imageAccess.pipelineType == PipelineType::GRAPHICS && passAccess.pipelineType == PipelineType::GRAPHICS && imageAccess.accessType == AccessType::WRITE && passAccess.accessType == AccessType::WRITE)
                {
                    // We do not need to add ImageBarriers between graphics pipelines that both write to the rendertarget, that is handled by the pipeline
                    break;
                }

                if (passAccess.accessType == AccessType::WRITE)
                {
                    _resources.SetLastBarrier(imageAccess.imageID, currentPassIndex);

                    if (imageAccess.accessType == AccessType::WRITE)
                    {
                        DepthImageMutableResource resource = _resources.GetMutableResource(imageAccess.imageID);
                        commandList.ImageBarrier(resource);
                        _numPlacedBarriers++;
                    }
                    else
                    {
                        DepthImageResource resource = _resources.GetResource(imageAccess.imageID);
                        commandList.ImageBarrier(resource);
                        _numPlacedBarriers++;
                    }

                    break;
                }
            }
        }
        

        commandList.PopMarker();
    }

    void RenderGraphBuilder::PostExecute(CommandList& /*commandList*/, u32 /*passIndex*/)
    {
        //if (!_resources.NeedsPostExecute(passIndex))
        //    return;
    }

    RenderGraphResources& RenderGraphBuilder::GetResources()
    {
        return _resources;
    }

    ImageID RenderGraphBuilder::Create(ImageDesc& /*desc*/)
    {
        return ImageID::Invalid();
    }

    DepthImageID RenderGraphBuilder::Create(DepthImageDesc& /*desc*/)
    {
        return DepthImageID::Invalid();
    }

    ImageResource RenderGraphBuilder::Read(ImageID id, PipelineType pipelineType)
    {
        ImageResource resource = _resources.GetResource(id);

        _resources.Access(_currentPassIndex, resource, AccessType::READ, pipelineType);

        return resource;
    }

    ImageResource RenderGraphBuilder::Read(TextureID id, PipelineType /*pipelineType*/)
    {
        ImageResource resource = _resources.GetResource(id);

        return resource;
    }

    DepthImageResource RenderGraphBuilder::Read(DepthImageID id, PipelineType pipelineType)
    {
        DepthImageResource resource = _resources.GetResource(id);

        _resources.Access(_currentPassIndex, resource, AccessType::READ, pipelineType);

        return resource;
    }

    ImageMutableResource RenderGraphBuilder::Write(ImageID id, PipelineType pipelineType, LoadMode loadMode)
    {
        ImageMutableResource resource = _resources.GetMutableResource(id);

        if (loadMode == LoadMode::CLEAR)
        {
            _resources.Clear(_currentPassIndex, resource);
        }

        _resources.Access(_currentPassIndex, resource, AccessType::WRITE, pipelineType);

        return resource;
    }

    DepthImageMutableResource RenderGraphBuilder::Write(DepthImageID id, PipelineType pipelineType, LoadMode loadMode)
    {
        DepthImageMutableResource resource = _resources.GetMutableResource(id);

        if (loadMode == LoadMode::CLEAR)
        {
            _resources.Clear(_currentPassIndex, resource);
        }

        _resources.Access(_currentPassIndex, resource, AccessType::WRITE, pipelineType);

        return resource;
    }

    DescriptorSetResource RenderGraphBuilder::Use(DescriptorSet& descriptorSet)
    {
        DescriptorSetResource resource = _resources.GetResource(descriptorSet);

        _resources.Use(_currentPassIndex, resource);

        return resource;
    }
}