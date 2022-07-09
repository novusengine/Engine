#include "RenderGraphBuilder.h"
#include "Renderer.h"
#include "RenderGraph.h"

namespace Renderer
{
    RenderGraphBuilder::RenderGraphBuilder(Memory::Allocator* allocator, Renderer* renderer, size_t numPasses)
        : _renderer(renderer)
        , _resources(allocator, numPasses)
    {

    }

    void RenderGraphBuilder::PreExecute(CommandList& commandList, u32 passIndex)
    {
        if (!_resources.NeedsPreExecute(passIndex))
            return;

        // Queue up all the clears for this pass
        commandList.PushMarker("RenderGraph::PreExecute", Color::White);

        const DynamicArray<ImageID>& colorClears = _resources.GetColorClears(passIndex);

        for (ImageID image : colorClears)
        {
            const ImageDesc& imageDesc = _renderer->GetImageDesc(image);

            ImageComponentType imageComponentType = ToImageComponentType(imageDesc.format);

            if (imageComponentType == ImageComponentType::FLOAT || imageComponentType == ImageComponentType::UNORM || imageComponentType == ImageComponentType::SNORM)
            {
                commandList.Clear(image, imageDesc.clearColor);
            }
            else if (imageComponentType == ImageComponentType::UINT)
            {
                commandList.Clear(image, imageDesc.clearUInts);
            }
            else if (imageComponentType == ImageComponentType::SINT)
            {
                commandList.Clear(image, imageDesc.clearInts);
            }
            else
            {
                DebugHandler::PrintFatal("Please implement more of these");
            }
        }

        const DynamicArray<DepthImageID>& depthClears = _resources.GetDepthClears(passIndex);

        for (DepthImageID image : depthClears)
        {
            const DepthImageDesc& imageDesc = _renderer->GetDepthImageDesc(image);
            commandList.Clear(image, imageDesc.depthClearValue);
        }

        // TODO: Handling between-pass barriers here
        // If we have .Read operations we need to:
        // * Check if the same resource had any .Write operations with WriteMode UAV in earlier passes
        // * If it did, add a Compute-Compute barrier if this read has the shaderstage Compute, otherwise add a Compute-ShaderRead barrier'

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

    RenderPassResource RenderGraphBuilder::Read(ImageID id, ShaderStage /*shaderStage*/)
    {
        RenderPassResource resource = _resources.GetResource(id);

        return resource;
    }

    RenderPassResource RenderGraphBuilder::Read(TextureID id, ShaderStage /*shaderStage*/)
    {
        RenderPassResource resource = _resources.GetResource(id);

        return resource;
    }

    RenderPassResource RenderGraphBuilder::Read(DepthImageID id, ShaderStage /*shaderStage*/)
    {
        RenderPassResource resource = _resources.GetResource(id);

        return resource;
    }

    RenderPassMutableResource RenderGraphBuilder::Write(ImageID id, WriteMode /*writeMode*/, LoadMode loadMode)
    {
        RenderPassMutableResource resource = _resources.GetMutableResource(id);

        if (loadMode == LoadMode::CLEAR)
        {
            _resources.Clear(_currentPassIndex, id);
        }

        return resource;
    }

    RenderPassMutableResource RenderGraphBuilder::Write(DepthImageID id, WriteMode /*writeMode*/, LoadMode loadMode)
    {
        RenderPassMutableResource resource = _resources.GetMutableResource(id);

        if (loadMode == LoadMode::CLEAR)
        {
            _resources.Clear(_currentPassIndex, id);
        }

        return resource;
    }
}