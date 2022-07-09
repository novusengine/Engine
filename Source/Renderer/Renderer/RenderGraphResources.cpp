#include "RenderGraphResources.h"
#include "Descriptors/GraphicsPipelineDesc.h"

namespace Renderer
{
    struct TrackedPass
    {
        TrackedPass(Memory::Allocator* allocator)
            : colorClears(allocator, 4)
            , depthClears(allocator, 4)
        {

        }

        bool needsPreExecute = false;
        bool needsPostExecute = false;
        DynamicArray<ImageID> colorClears;
        DynamicArray<DepthImageID> depthClears;
    };

    struct RenderGraphResourcesData : IRenderGraphResourcesData
    {
        RenderGraphResourcesData(Memory::Allocator* allocator, size_t numPasses)
            : trackedImages(allocator, 32)
            , trackedTextures(allocator, 32)
            , trackedDepthImages(allocator, 32)
            , trackedPasses(allocator, numPasses)
        {
            for (u32 i = 0; i < numPasses; i++)
            {
                TrackedPass trackedPass(allocator);
                trackedPasses.Insert(trackedPass);
            }
        }

        DynamicArray<ImageID> trackedImages;
        DynamicArray<TextureID> trackedTextures;
        DynamicArray<DepthImageID> trackedDepthImages;
        DynamicArray<TrackedPass> trackedPasses;
    };

	RenderGraphResources::RenderGraphResources(Memory::Allocator* allocator, size_t numPasses)
		: _allocator(allocator)
        , _data(Memory::Allocator::New<RenderGraphResourcesData>(allocator, allocator, numPasses))
    {
	}

    void RenderGraphResources::InitializePipelineDesc(GraphicsPipelineDesc& desc)
    {
        desc.ResourceToImageID = [&](RenderPassResource resource)
        {
            return GetImage(resource);
        };
        desc.ResourceToDepthImageID = [&](RenderPassResource resource)
        {
            return GetDepthImage(resource);
        };
        desc.MutableResourceToImageID = [&](RenderPassMutableResource resource)
        {
            return GetImage(resource);
        };
        desc.MutableResourceToDepthImageID = [&](RenderPassMutableResource resource)
        {
            return GetDepthImage(resource);
        };
    }

    void RenderGraphResources::InitializePipelineDesc(ComputePipelineDesc& desc)
    {
    }

    ImageID RenderGraphResources::GetImage(RenderPassResource resource)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedImages[static_cast<RenderPassResource::type> (resource)];
    }

    ImageID RenderGraphResources::GetImage(RenderPassMutableResource resource)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedImages[static_cast<RenderPassMutableResource::type>(resource)];
    }

    DepthImageID RenderGraphResources::GetDepthImage(RenderPassResource resource)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedDepthImages[static_cast<RenderPassResource::type>(resource)];
    }

    DepthImageID RenderGraphResources::GetDepthImage(RenderPassMutableResource resource)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedDepthImages[static_cast<RenderPassMutableResource::type>(resource)];
    }

    RenderPassResource RenderGraphResources::GetResource(ImageID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        ImageID::type i = 0;
        for (ImageID& trackedID : data->trackedImages)
        {
            if (trackedID == id)
            {
                return RenderPassResource(i);
            }

            i++;
        }

        data->trackedImages.Insert(id);
        return RenderPassResource(i);
    }

    RenderPassResource RenderGraphResources::GetResource(TextureID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        TextureID::type i = 0;
        for (TextureID& trackedID : data->trackedTextures)
        {
            if (trackedID == id)
            {
                return RenderPassResource(i);
            }

            i++;
        }

        data->trackedTextures.Insert(id);
        return RenderPassResource(i);
    }

    RenderPassResource RenderGraphResources::GetResource(DepthImageID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        DepthImageID::type i = 0;
        for (DepthImageID& trackedID : data->trackedDepthImages)
        {
            if (trackedID == id)
            {
                return RenderPassResource(i);
            }

            i++;
        }

        data->trackedDepthImages.Insert(id);
        return RenderPassResource(i);
    }

    RenderPassMutableResource RenderGraphResources::GetMutableResource(ImageID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        ImageID::type i = 0;
        for (ImageID& trackedID : data->trackedImages)
        {
            if (trackedID == id)
            {
                return RenderPassMutableResource(i);
            }

            i++;
        }

        data->trackedImages.Insert(id);
        return RenderPassMutableResource(i);
    }

    RenderPassMutableResource RenderGraphResources::GetMutableResource(DepthImageID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        DepthImageID::type i = 0;
        for (DepthImageID& trackedID : data->trackedDepthImages)
        {
            if (trackedID == id)
            {
                return RenderPassMutableResource(i);
            }

            i++;
        }

        data->trackedDepthImages.Insert(id);
        return RenderPassMutableResource(i);
    }
    void RenderGraphResources::Clear(u32 passIndex, ImageID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access color clears of pass that hasn't been tracked yet");
        }

        data->trackedPasses[passIndex].needsPreExecute = true;
        data->trackedPasses[passIndex].colorClears.Insert(id);
    }
    void RenderGraphResources::Clear(u32 passIndex, DepthImageID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access color clears of pass that hasn't been tracked yet");
        }

        data->trackedPasses[passIndex].needsPreExecute = true;
        data->trackedPasses[passIndex].depthClears.Insert(id);
    }

    bool RenderGraphResources::NeedsPreExecute(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access color clears of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].needsPreExecute;
    }

    bool RenderGraphResources::NeedsPostExecute(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access color clears of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].needsPostExecute;
    }

    const DynamicArray<ImageID>& RenderGraphResources::GetColorClears(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access color clears of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].colorClears;
    }

    const DynamicArray<DepthImageID>& RenderGraphResources::GetDepthClears(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access depth clears of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].depthClears;
    }
}