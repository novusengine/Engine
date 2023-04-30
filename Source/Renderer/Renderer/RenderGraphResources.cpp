#include "RenderGraphResources.h"
#include "Renderer.h"
#include "Descriptors/GraphicsPipelineDesc.h"

namespace Renderer
{
    struct TrackedPass
    {
        TrackedPass(Memory::Allocator* allocator)
            : colorClears(allocator, 16)
            , depthClears(allocator, 16)
            , imageAccesses(allocator, 16)
            , depthImageAccesses(allocator, 16)
            , descriptorSetIDs(allocator, 8)
        {

        }

        bool needsPreExecute = false;
        bool needsPostExecute = false;
        DynamicArray<ImageMutableResource> colorClears;
        DynamicArray<DepthImageMutableResource> depthClears;

        DynamicArray<TrackedImageAccess> imageAccesses;
        DynamicArray<TrackedDepthImageAccess> depthImageAccesses;

        DynamicArray<DescriptorSetID> descriptorSetIDs;
    };

    struct RenderGraphResourcesData : IRenderGraphResourcesData
    {
        RenderGraphResourcesData(Memory::Allocator* allocator, size_t numPasses)
            : trackedImages(allocator, 32)
            , trackedTextures(allocator, 32)
            , trackedDepthImages(allocator, 32)
            , trackedImageAccesses(allocator, 32)
            , trackedDepthImageAccesses(allocator, 32)
            , trackedImageLastBarrier(allocator, 32)
            , trackedDepthImageLastBarrier(allocator, 32)
            , trackedPasses(allocator, numPasses)
            , trackedDescriptorSets(allocator, 32)
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

        DynamicArray<DynamicArray<TrackedPassAccess>> trackedImageAccesses;
        DynamicArray<DynamicArray<TrackedPassAccess>> trackedDepthImageAccesses;

        DynamicArray<u32> trackedImageLastBarrier;
        DynamicArray<u32> trackedDepthImageLastBarrier;

        DynamicArray<TrackedPass> trackedPasses;

        DynamicArray<DescriptorSet*> trackedDescriptorSets;
    };

	RenderGraphResources::RenderGraphResources(Memory::Allocator* allocator, Renderer* renderer, size_t numPasses)
		: _allocator(allocator)
        , _renderer(renderer)
        , _data(Memory::Allocator::New<RenderGraphResourcesData>(allocator, allocator, numPasses))
    {
	}

    void RenderGraphResources::InitializePipelineDesc(GraphicsPipelineDesc& desc)
    {
        desc.ResourceToImageID = [&](ImageResource resource)
        {
            return GetImage(resource);
        };
        desc.ResourceToDepthImageID = [&](DepthImageResource resource)
        {
            return GetImage(resource);
        };
        desc.MutableResourceToImageID = [&](ImageMutableResource resource)
        {
            return GetImage(resource);
        };
        desc.MutableResourceToDepthImageID = [&](DepthImageMutableResource resource)
        {
            return GetImage(resource);
        };
    }

    void RenderGraphResources::InitializePipelineDesc(ComputePipelineDesc& desc)
    {
    }

    const ImageDesc& RenderGraphResources::GetImageDesc(ImageResource resource)
    {
        ImageID imageID = GetImage(resource);
        return _renderer->GetImageDesc(imageID);
    }
    const ImageDesc& RenderGraphResources::GetImageDesc(ImageMutableResource resource)
    {
        ImageID imageID = GetImage(resource);
        return _renderer->GetImageDesc(imageID);
    }
    const DepthImageDesc& RenderGraphResources::GetImageDesc(DepthImageResource resource)
    {
        DepthImageID imageID = GetImage(resource);
        return _renderer->GetImageDesc(imageID);
    }
    const DepthImageDesc& RenderGraphResources::GetImageDesc(DepthImageMutableResource resource)
    {
        DepthImageID imageID = GetImage(resource);
        return _renderer->GetImageDesc(imageID);
    }

    uvec2 RenderGraphResources::GetImageDimensions(ImageResource resource, u32 mipLevel)
    {
        ImageID imageID = GetImage(resource);
        return _renderer->GetImageDimensions(imageID, mipLevel);
    }
    uvec2 RenderGraphResources::GetImageDimensions(ImageMutableResource resource, u32 mipLevel)
    {
        ImageID imageID = GetImage(resource);
        return _renderer->GetImageDimensions(imageID, mipLevel);
    }
    uvec2 RenderGraphResources::GetImageDimensions(DepthImageResource resource)
    {
        DepthImageID imageID = GetImage(resource);
        return _renderer->GetImageDimensions(imageID);
    }
    uvec2 RenderGraphResources::GetImageDimensions(DepthImageMutableResource resource)
    {
        DepthImageID imageID = GetImage(resource);
        return _renderer->GetImageDimensions(imageID);
    }


    ImageID RenderGraphResources::GetImage(ImageResource resource)
    {
        DebugHandler::Assert(resource != ImageResource::Invalid(), "RenderGraphResources : GetImage tried to get image of invalid ImageResource");

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedImages[static_cast<ImageResource::type> (resource)];
    }

    ImageID RenderGraphResources::GetImage(ImageMutableResource resource)
    {
        DebugHandler::Assert(resource != ImageMutableResource::Invalid(), "RenderGraphResources : GetImage tried to get image of invalid ImageMutableResource");

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedImages[static_cast<ImageMutableResource::type>(resource)];
    }

    DepthImageID RenderGraphResources::GetImage(DepthImageResource resource)
    {
        DebugHandler::Assert(resource != DepthImageResource::Invalid(), "RenderGraphResources : GetImage tried to get image of invalid DepthImageResource");

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedDepthImages[static_cast<DepthImageResource::type>(resource)];
    }

    DepthImageID RenderGraphResources::GetImage(DepthImageMutableResource resource)
    {
        DebugHandler::Assert(resource != DepthImageMutableResource::Invalid(), "RenderGraphResources : GetImage tried to get image of invalid DepthImageMutableResource");

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedDepthImages[static_cast<DepthImageMutableResource::type>(resource)];
    }

    DescriptorSet* RenderGraphResources::GetDescriptorSet(DescriptorSetID descriptorSetID)
    {
        DebugHandler::Assert(descriptorSetID != DescriptorSetID::Invalid(), "RenderGraphResources : GetDescriptorSet tried to get image of invalid DescriptorSetResourceID");

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedDescriptorSets[static_cast<DepthImageMutableResource::type>(descriptorSetID)];
    }

    ImageResource RenderGraphResources::GetResource(ImageID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        ImageID::type i = 0;
        for (ImageID& trackedID : data->trackedImages)
        {
            if (trackedID == id)
            {
                return ImageResource(i);
            }

            i++;
        }

        data->trackedImages.Insert(id);

        return ImageResource(i);
    }

    ImageResource RenderGraphResources::GetResource(TextureID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        TextureID::type i = 0;
        for (TextureID& trackedID : data->trackedTextures)
        {
            if (trackedID == id)
            {
                return ImageResource(i);
            }

            i++;
        }

        data->trackedTextures.Insert(id);
        return ImageResource(i);
    }

    DepthImageResource RenderGraphResources::GetResource(DepthImageID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        DepthImageID::type i = 0;
        for (DepthImageID& trackedID : data->trackedDepthImages)
        {
            if (trackedID == id)
            {
                return DepthImageResource(i);
            }

            i++;
        }

        data->trackedDepthImages.Insert(id);
        return DepthImageResource(i);
    }

    DescriptorSetResource RenderGraphResources::GetResource(DescriptorSet& descriptorSet)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        DescriptorSetID id = DescriptorSetID::Invalid();
        for (u32 i = 0; i < data->trackedDescriptorSets.Count(); i++)
        {
            DescriptorSet* trackedDescriptorSet = data->trackedDescriptorSets[i];

            if (trackedDescriptorSet == &descriptorSet)
            {
                id = DescriptorSetID(i);
                break;
            }
        }

        if (id == DescriptorSetID::Invalid())
        {
            id = DescriptorSetID(static_cast<DescriptorSetID::type>(data->trackedDescriptorSets.Count()));
            data->trackedDescriptorSets.Insert(&descriptorSet);
        }

        return DescriptorSetResource(id, *this);
    }

    ImageMutableResource RenderGraphResources::GetMutableResource(ImageID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        ImageID::type i = 0;
        for (ImageID& trackedID : data->trackedImages)
        {
            if (trackedID == id)
            {
                return ImageMutableResource(i);
            }

            i++;
        }

        data->trackedImages.Insert(id);
        return ImageMutableResource(i);
    }

    DepthImageMutableResource RenderGraphResources::GetMutableResource(DepthImageID id)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        DepthImageID::type i = 0;
        for (DepthImageID& trackedID : data->trackedDepthImages)
        {
            if (trackedID == id)
            {
                return DepthImageMutableResource(i);
            }

            i++;
        }

        data->trackedDepthImages.Insert(id);
        return DepthImageMutableResource(i);
    }

    void RenderGraphResources::Clear(u32 passIndex, ImageMutableResource resource)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access color clears of pass that hasn't been tracked yet");
        }

        data->trackedPasses[passIndex].needsPreExecute = true;
        data->trackedPasses[passIndex].colorClears.Insert(resource);
    }

    void RenderGraphResources::Clear(u32 passIndex, DepthImageMutableResource resource)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access color clears of pass that hasn't been tracked yet");
        }

        data->trackedPasses[passIndex].needsPreExecute = true;
        data->trackedPasses[passIndex].depthClears.Insert(resource);
    }

    void RenderGraphResources::Access(u32 passIndex, ImageResource resource, AccessType accessType, PipelineType pipelineType)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to Access pass that hasn't been tracked yet");
        }

        ImageID imageID = GetImage(resource);

        // We want to track this access in two ways
        // First make sure that each tracked renderpass knows what accesses it has
        TrackedImageAccess imageAccess;
        imageAccess.imageID = imageID;
        imageAccess.accessType = accessType;
        imageAccess.pipelineType = pipelineType;

        data->trackedPasses[passIndex].imageAccesses.Insert(imageAccess);

        // Secondly we make sure that each resource knows what passes access it
        ImageID::type index = static_cast<ImageID::type>(imageID);
        while (data->trackedImageAccesses.Count() <= index)
        {
            data->trackedImageAccesses.Insert(DynamicArray<TrackedPassAccess>(_allocator, 16)); // Init arrays
            data->trackedImageLastBarrier.Insert(0);
        }

        TrackedPassAccess passAccess;
        passAccess.passIndex = passIndex;
        passAccess.accessType = accessType;
        passAccess.pipelineType = pipelineType;

        data->trackedImageAccesses[index].Insert(passAccess);

        // Finally we need to make sure the pass will PreExecute
        data->trackedPasses[passIndex].needsPreExecute = true;
    }

    void RenderGraphResources::Access(u32 passIndex, ImageMutableResource resource, AccessType accessType, PipelineType pipelineType)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to Access pass that hasn't been tracked yet");
        }

        ImageID imageID = GetImage(resource);

        // We want to track this access in two ways
        // First make sure that each tracked renderpass knows what accesses it has
        TrackedImageAccess imageAccess;
        imageAccess.imageID = imageID;
        imageAccess.accessType = accessType;
        imageAccess.pipelineType = pipelineType;

        data->trackedPasses[passIndex].imageAccesses.Insert(imageAccess);

        // Secondly we make sure that each resource knows what passes access it
        ImageID::type index = static_cast<ImageID::type>(imageID);
        while (data->trackedImageAccesses.Count() <= index)
        {
            data->trackedImageAccesses.Insert(DynamicArray<TrackedPassAccess>(_allocator, 16)); // Init arrays
            data->trackedImageLastBarrier.Insert(0);
        }

        TrackedPassAccess passAccess;
        passAccess.passIndex = passIndex;
        passAccess.accessType = accessType;
        passAccess.pipelineType = pipelineType;

        data->trackedImageAccesses[index].Insert(passAccess);

        // Finally we need to make sure the pass will PreExecute
        data->trackedPasses[passIndex].needsPreExecute = true;
    }

    void RenderGraphResources::Access(u32 passIndex, DepthImageResource resource, AccessType accessType, PipelineType pipelineType)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to Access of pass that hasn't been tracked yet");
        }

        DepthImageID imageID = GetImage(resource);

        // We want to track this access in two ways
        // First make sure that each tracked renderpass knows what accesses it has
        TrackedDepthImageAccess imageAccess;
        imageAccess.imageID = imageID;
        imageAccess.accessType = accessType;
        imageAccess.pipelineType = pipelineType;

        data->trackedPasses[passIndex].depthImageAccesses.Insert(imageAccess);

        // Secondly we make sure that each resource knows what passes access it
        DepthImageID::type index = static_cast<DepthImageID::type>(imageID);
        while (data->trackedDepthImageAccesses.Count() <= index)
        {
            data->trackedDepthImageAccesses.Insert(DynamicArray<TrackedPassAccess>(_allocator, 16)); // Init arrays
            data->trackedDepthImageLastBarrier.Insert(0);
        }

        TrackedPassAccess passAccess;
        passAccess.passIndex = passIndex;
        passAccess.accessType = accessType;
        passAccess.pipelineType = pipelineType;

        data->trackedDepthImageAccesses[index].Insert(passAccess);

        // Finally we need to make sure the pass will PreExecute
        data->trackedPasses[passIndex].needsPreExecute = true;
    }

    void RenderGraphResources::Access(u32 passIndex, DepthImageMutableResource resource, AccessType accessType, PipelineType pipelineType)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to Access of pass that hasn't been tracked yet");
        }

        DepthImageID imageID = GetImage(resource);

        // We want to track this access in two ways
        // First make sure that each tracked renderpass knows what accesses it has
        TrackedDepthImageAccess imageAccess;
        imageAccess.imageID = imageID;
        imageAccess.accessType = accessType;
        imageAccess.pipelineType = pipelineType;

        data->trackedPasses[passIndex].depthImageAccesses.Insert(imageAccess);

        // Secondly we make sure that each resource knows what passes access it
        DepthImageID::type index = static_cast<DepthImageID::type>(imageID);
        while (data->trackedDepthImageAccesses.Count() <= index)
        {
            data->trackedDepthImageAccesses.Insert(DynamicArray<TrackedPassAccess>(_allocator, 16)); // Init arrays
            data->trackedDepthImageLastBarrier.Insert(0);
        }

        TrackedPassAccess passAccess;
        passAccess.passIndex = passIndex;
        passAccess.accessType = accessType;
        passAccess.pipelineType = pipelineType;

        data->trackedDepthImageAccesses[index].Insert(passAccess);

        // Finally we need to make sure the pass will PreExecute
        data->trackedPasses[passIndex].needsPreExecute = true;
    }

    void RenderGraphResources::Use(u32 passIndex, DescriptorSetResource resource)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access color clears of pass that hasn't been tracked yet");
        }

        data->trackedPasses[passIndex].descriptorSetIDs.Insert(resource.GetID());
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

    const DynamicArray<ImageMutableResource>& RenderGraphResources::GetColorClears(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access color clears of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].colorClears;
    }

    const DynamicArray<DepthImageMutableResource>& RenderGraphResources::GetDepthClears(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access depth clears of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].depthClears;
    }

    const DynamicArray<TrackedImageAccess>& RenderGraphResources::GetImageAccesses(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access depth clears of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].imageAccesses;
    }
    const DynamicArray<TrackedDepthImageAccess>& RenderGraphResources::GetDepthImageAccesses(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access depth clears of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].depthImageAccesses;
    }

    const DynamicArray<DescriptorSetID>& RenderGraphResources::GetUsedDescriptorSetIDs(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access used descriptor sets of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].descriptorSetIDs;
    }

    const DynamicArray<TrackedPassAccess>& RenderGraphResources::GetPassAccesses(ImageID imageID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        ImageID::type index = static_cast<ImageID::type>(imageID);
        if (index >= data->trackedImageAccesses.Count())
        {
            DebugHandler::PrintFatal("Tried to access trackedImageAccesses of ImageID that hasn't been tracked yet");
        }

        return data->trackedImageAccesses[index];
    }

    const DynamicArray<TrackedPassAccess>& RenderGraphResources::GetPassAccesses(DepthImageID imageID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        DepthImageID::type index = static_cast<DepthImageID::type>(imageID);
        if (index >= data->trackedDepthImageAccesses.Count())
        {
            DebugHandler::PrintFatal("Tried to access trackedDepthImageAccesses of DepthImageID that hasn't been tracked yet");
        }

        return data->trackedDepthImageAccesses[index];
    }

    u32 RenderGraphResources::GetLastBarrier(ImageID imageID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        ImageID::type index = static_cast<ImageID::type>(imageID);
        if (index >= data->trackedImageLastBarrier.Count())
        {
            DebugHandler::PrintFatal("Tried to get trackedImageLastBarrier of ImageID that hasn't been tracked yet");
        }

        return data->trackedImageLastBarrier[index];
    }

    u32 RenderGraphResources::GetLastBarrier(DepthImageID imageID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        DepthImageID::type index = static_cast<DepthImageID::type>(imageID);
        if (index >= data->trackedDepthImageLastBarrier.Count())
        {
            DebugHandler::PrintFatal("Tried to get trackedDepthImageLastBarrier of DepthImageID that hasn't been tracked yet");
        }

        return data->trackedDepthImageLastBarrier[index];
    }

    void RenderGraphResources::SetLastBarrier(ImageID imageID, u32 passID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        ImageID::type index = static_cast<ImageID::type>(imageID);
        if (index >= data->trackedImageLastBarrier.Count())
        {
            DebugHandler::PrintFatal("Tried to set trackedImageLastBarrier of ImageID that hasn't been tracked yet");
        }

        data->trackedImageLastBarrier[index] = passID;
    }

    void RenderGraphResources::SetLastBarrier(DepthImageID imageID, u32 passID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        DepthImageID::type index = static_cast<DepthImageID::type>(imageID);
        if (index >= data->trackedDepthImageLastBarrier.Count())
        {
            DebugHandler::PrintFatal("Tried to set trackedDepthImageLastBarrier of DepthImageID that hasn't been tracked yet");
        }

        data->trackedDepthImageLastBarrier[index] = passID;
    }
}