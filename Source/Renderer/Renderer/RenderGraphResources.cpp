#include "RenderGraphResources.h"
#include "Renderer.h"
#include "TrackedBufferBitSets.h"
#include "Descriptors/GraphicsPipelineDesc.h"

#include <Base/Container/BitSet.h>

#include <tracy/Tracy.hpp>
#include <robinhood/robinhood.h>

namespace Renderer
{
    struct TrackedPass
    {
        TrackedPass(Memory::Allocator* allocator, u32 numBufferPermissionSets)
            : colorClears(allocator, 16)
            , depthClears(allocator, 16)
            , imageAccesses(allocator, 16)
            , depthImageAccesses(allocator, 16)
            , bufferPermissions(allocator, numBufferPermissionSets)
            , descriptorSetIDs(allocator, 8)
        {

        }

        DynamicArray<ImageMutableResource> colorClears;
        DynamicArray<DepthImageMutableResource> depthClears;

        DynamicArray<TrackedImageAccess> imageAccesses;
        DynamicArray<TrackedDepthImageAccess> depthImageAccesses;

        TrackedBufferBitSets bufferPermissions;

        DynamicArray<DescriptorSetID> descriptorSetIDs;
    };

    struct RenderGraphResourcesData : IRenderGraphResourcesData
    {
        RenderGraphResourcesData(Memory::Allocator* allocator, size_t numPasses, u32 numTotalBuffers)
            : trackedImages(allocator, 32)
            , trackedDepthImages(allocator, 32)
            , trackedBuffers(allocator, numTotalBuffers)
            , trackedImageAccesses(allocator, 32)
            , trackedDepthImageAccesses(allocator, 32)
            , trackedImageLastBarrier(allocator, 32)
            , trackedDepthImageLastBarrier(allocator, 32)
            , trackedPasses(allocator, numPasses)
            , trackedDescriptorSets(allocator, 32)
        {
            u32 numBufferPermissionsSets = Math::FloorToInt(static_cast<f32>(numTotalBuffers) / 64.0f) + 1;

            for (u32 i = 0; i < numPasses; i++)
            {
                TrackedPass trackedPass(allocator, numBufferPermissionsSets);
                trackedPasses.Insert(trackedPass);
            }
        }

        DynamicArray<ImageID> trackedImages;
        DynamicArray<DepthImageID> trackedDepthImages;
        DynamicArray<BufferID> trackedBuffers;

        DynamicArray<DynamicArray<TrackedImagePassAccess>> trackedImageAccesses;
        DynamicArray<DynamicArray<TrackedImagePassAccess>> trackedDepthImageAccesses;

        DynamicArray<u32> trackedImageLastBarrier;
        DynamicArray<u32> trackedDepthImageLastBarrier;

        DynamicArray<TrackedPass> trackedPasses;

        DynamicArray<DescriptorSet*> trackedDescriptorSets;

        static robin_hood::unordered_map<u32, u32> imageIDToResource;
        static robin_hood::unordered_map<u32, u32> depthImageIDToResource;
        static robin_hood::unordered_map<u32, u32> bufferIDToResource;
    };

    robin_hood::unordered_map<u32, u32> RenderGraphResourcesData::imageIDToResource;
    robin_hood::unordered_map<u32, u32> RenderGraphResourcesData::depthImageIDToResource;
    robin_hood::unordered_map<u32, u32> RenderGraphResourcesData::bufferIDToResource;

	RenderGraphResources::RenderGraphResources(Memory::Allocator* allocator, Renderer* renderer, size_t numPasses, u32 numTotalBuffers)
		: _allocator(allocator)
        , _renderer(renderer)
        , _data(Memory::Allocator::New<RenderGraphResourcesData>(allocator, allocator, numPasses, numTotalBuffers))
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        data->imageIDToResource.clear();
        data->depthImageIDToResource.clear();
        data->bufferIDToResource.clear();
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

    BufferID RenderGraphResources::GetBuffer(BufferResource resource)
    {
        DebugHandler::Assert(resource != BufferResource::Invalid(), "RenderGraphResources : GetBuffer tried to get buffer of invalid BufferResource");

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedBuffers[static_cast<DepthImageMutableResource::type>(resource)];
    }
    BufferID RenderGraphResources::GetBuffer(BufferMutableResource resource)
    {
        DebugHandler::Assert(resource != BufferMutableResource::Invalid(), "RenderGraphResources : GetBuffer tried to get buffer of invalid BufferMutableResource");

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedBuffers[static_cast<DepthImageMutableResource::type>(resource)];
    }

    DescriptorSet* RenderGraphResources::GetDescriptorSet(DescriptorSetID descriptorSetID)
    {
        DebugHandler::Assert(descriptorSetID != DescriptorSetID::Invalid(), "RenderGraphResources : GetDescriptorSet tried to get image of invalid DescriptorSetResourceID");

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);
        return data->trackedDescriptorSets[static_cast<DepthImageMutableResource::type>(descriptorSetID)];
    }

    ImageResource RenderGraphResources::GetResource(ImageID imageID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        ImageID::type id = static_cast<ImageID::type>(imageID);
        if (!data->imageIDToResource.contains(id))
        {
            u32 index = data->trackedImages.Count();
            data->trackedImages.Insert(imageID);

            data->imageIDToResource[id] = index;
            return ImageResource(index);
        }

        return ImageResource(data->imageIDToResource[id]);
    }
    DepthImageResource RenderGraphResources::GetResource(DepthImageID imageID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        DepthImageID::type id = static_cast<DepthImageID::type>(imageID);
        if (!data->depthImageIDToResource.contains(id))
        {
            u32 index = data->trackedDepthImages.Count();
            data->trackedDepthImages.Insert(imageID);

            data->depthImageIDToResource[id] = index;
            return DepthImageResource(index);
        }

        return DepthImageResource(data->depthImageIDToResource[id]);
    }

    BufferResource RenderGraphResources::GetResource(BufferID bufferID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        BufferID::type id = static_cast<BufferID::type>(bufferID);
        if (!data->bufferIDToResource.contains(id))
        {
            u32 index = data->trackedBuffers.Count();
            data->trackedBuffers.Insert(bufferID);

            data->bufferIDToResource[id] = index;
            return BufferResource(index);
        }

        return BufferResource(data->bufferIDToResource[id]);
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

    ImageMutableResource RenderGraphResources::GetMutableResource(ImageID imageID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        ImageID::type id = static_cast<ImageID::type>(imageID);
        if (!data->imageIDToResource.contains(id))
        {
            u32 index = data->trackedImages.Count();
            data->trackedImages.Insert(imageID);

            data->imageIDToResource[id] = index;
            return ImageMutableResource(index);
        }

        return ImageMutableResource(data->imageIDToResource[id]);
    }
    DepthImageMutableResource RenderGraphResources::GetMutableResource(DepthImageID imageID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        DepthImageID::type id = static_cast<DepthImageID::type>(imageID);
        if (!data->depthImageIDToResource.contains(id))
        {
            u32 index = data->trackedDepthImages.Count();
            data->trackedDepthImages.Insert(imageID);

            data->depthImageIDToResource[id] = index;
            return DepthImageMutableResource(index);
        }

        return DepthImageMutableResource(data->depthImageIDToResource[id]);
    }

    BufferMutableResource RenderGraphResources::GetMutableResource(BufferID bufferID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        BufferID::type id = static_cast<BufferID::type>(bufferID);
        if (!data->bufferIDToResource.contains(id))
        {
            u32 index = data->trackedBuffers.Count();
            data->trackedBuffers.Insert(bufferID);

            data->bufferIDToResource[id] = index;
            return BufferMutableResource(index);
        }

        return BufferMutableResource(data->bufferIDToResource[id]);
    }

    void RenderGraphResources::Clear(u32 passIndex, ImageMutableResource resource)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to Clear color image of pass that hasn't been tracked yet");
        }

        data->trackedPasses[passIndex].colorClears.Insert(resource);
    }

    void RenderGraphResources::Clear(u32 passIndex, DepthImageMutableResource resource)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to Clear depth image of pass that hasn't been tracked yet");
        }

        data->trackedPasses[passIndex].depthClears.Insert(resource);
    }

    inline void _Access(Memory::Allocator* allocator, DynamicArray<TrackedImageAccess>& imageAccesses, DynamicArray<DynamicArray<TrackedImagePassAccess>>& passAccesses, DynamicArray<u32>& lastBarriers, u32 passIndex, ImageID imageID, AccessType accessType, PipelineType pipelineType)
    {
        ZoneScoped;

        // We want to track this access in two ways
        // First make sure that each tracked renderpass knows what accesses it has
        {
            ZoneScopedN("TrackedImageAccess");

            TrackedImageAccess imageAccess;
            imageAccess.imageID = imageID;
            imageAccess.accessType = accessType;
            imageAccess.pipelineType = pipelineType;

            imageAccesses.Insert(imageAccess);
        }
        

        // Secondly we make sure that each resource knows what passes access it
        {
            ZoneScopedN("TrackedImagePassAccess");

            ImageID::type index = static_cast<ImageID::type>(imageID);
            while (passAccesses.Count() <= index)
            {
                passAccesses.Insert(DynamicArray<TrackedImagePassAccess>(allocator, 16)); // Init arrays
                lastBarriers.Insert(0);
            }

            TrackedImagePassAccess passAccess;
            passAccess.passIndex = passIndex;
            passAccess.accessType = accessType;
            passAccess.pipelineType = pipelineType;

            passAccesses[index].Insert(passAccess);
        }
    }

    void _Access(Memory::Allocator* allocator, DynamicArray<TrackedDepthImageAccess>& imageAccesses, DynamicArray<DynamicArray<TrackedImagePassAccess>>& passAccesses, DynamicArray<u32>& lastBarriers, u32 passIndex, DepthImageID imageID, AccessType accessType, PipelineType pipelineType)
    {
        ZoneScoped;

        // We want to track this access in two ways
        // First make sure that each tracked renderpass knows what accesses it has
        {
            ZoneScopedN("TrackedDepthImageAccess");

            TrackedDepthImageAccess imageAccess;
            imageAccess.imageID = imageID;
            imageAccess.accessType = accessType;
            imageAccess.pipelineType = pipelineType;

            imageAccesses.Insert(imageAccess);
        }
        

        // Secondly we make sure that each resource knows what passes access it
        {
            ZoneScopedN("TrackedImagePassAccess");
            ImageID::type index = static_cast<ImageID::type>(imageID);
            while (passAccesses.Count() <= index)
            {
                passAccesses.Insert(DynamicArray<TrackedImagePassAccess>(allocator, 16)); // Init arrays
                lastBarriers.Insert(0);
            }

            TrackedImagePassAccess passAccess;
            passAccess.passIndex = passIndex;
            passAccess.accessType = accessType;
            passAccess.pipelineType = pipelineType;

            passAccesses[index].Insert(passAccess);
        }
    }

    void _Access(Memory::Allocator* allocator, TrackedBufferBitSets& bufferPermissions, u32 passIndex, BufferID bufferID, AccessType accessType, BufferPassUsage bufferPassUsage)
    {
        ZoneScoped;

        // First make sure the pass didn't already track this resource
        {
            ZoneScopedN("Double Use Check");
            const BitSet& readBitSet = bufferPermissions.GetReadBitSet();

            BufferID::type bufferIndex = static_cast<BufferID::type>(bufferID);
            if (readBitSet.Has(bufferIndex))
            {
                DebugHandler::PrintFatal("RenderGraphResources : Pass {} tried to Access BufferID {} twice, you may only .Read or .Write a resource once in the same pass!", passIndex, static_cast<BufferID::type>(bufferID));
            }
        }

        // Lastly we update that passes accessBitset
        {
            ZoneScopedN("BitSet Add");
            bufferPermissions.Add(bufferID, accessType, bufferPassUsage);
        }
    }

    void RenderGraphResources::Access(u32 passIndex, ImageID imageID, AccessType accessType, PipelineType pipelineType)
    {
        ZoneScoped;

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to Access pass that hasn't been tracked yet");
        }

        _Access(_allocator, data->trackedPasses[passIndex].imageAccesses, data->trackedImageAccesses, data->trackedImageLastBarrier, passIndex, imageID, accessType, pipelineType);
    }

    void RenderGraphResources::Access(u32 passIndex, DepthImageID imageID, AccessType accessType, PipelineType pipelineType)
    {
        ZoneScoped;

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to Access pass that hasn't been tracked yet");
        }

        _Access(_allocator, data->trackedPasses[passIndex].depthImageAccesses, data->trackedDepthImageAccesses, data->trackedDepthImageLastBarrier, passIndex, imageID, accessType, pipelineType);
    }

    void RenderGraphResources::Access(u32 passIndex, BufferID bufferID, AccessType accessType, BufferPassUsage bufferPassUsage)
    {
        ZoneScoped;

        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to Access pass that hasn't been tracked yet");
        }

        TrackedPass& trackedPass = data->trackedPasses[passIndex];
        _Access(_allocator, trackedPass.bufferPermissions, passIndex, bufferID, accessType, bufferPassUsage);
    }

    void RenderGraphResources::Use(u32 passIndex, DescriptorSetResource resource)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to Use DescriptorSetResource in pass that hasn't been tracked yet");
        }

        data->trackedPasses[passIndex].descriptorSetIDs.Insert(resource.GetID());
    }

    void RenderGraphResources::EnforceHasAccess(u32 passIndex, BufferResource resource, BufferPassUsage bufferPassUsage)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to EnforceHasAccess in pass that hasn't been tracked yet");
        }

        BufferID bufferID = GetBuffer(resource);
        BufferID::type bufferIDTyped = static_cast<BufferID::type>(bufferID);

        const TrackedBufferBitSets& permissions = data->trackedPasses[passIndex].bufferPermissions;

        if (!permissions.Has(bufferID, bufferPassUsage))
        {
            const std::string& bufferName = _renderer->GetBufferName(bufferID);
            DebugHandler::PrintFatal("RenderGraphResources : EnforceHasAccess tried to enforce {} access on BufferID {} ({}) in RenderPass {}, but it did not have access", GetBufferPassUsageName(bufferPassUsage), bufferIDTyped, bufferName, passIndex);
        }
    }

    void RenderGraphResources::EnforceHasAccess(u32 passIndex, BufferMutableResource resource, BufferPassUsage bufferPassUsage)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to EnforceHasAccess in pass that hasn't been tracked yet");
        }

        BufferID bufferID = GetBuffer(resource);
        BufferID::type bufferIDTyped = static_cast<BufferID::type>(bufferID);

        const TrackedBufferBitSets& permissions = data->trackedPasses[passIndex].bufferPermissions;

        if (!permissions.Has(bufferID, bufferPassUsage))
        {
            const std::string& bufferName = _renderer->GetBufferName(bufferID);
            DebugHandler::PrintFatal("RenderGraphResources : EnforceHasAccess tried to enforce {} access on BufferID {} ({}) in RenderPass {}, but it did not have access", GetBufferPassUsageName(bufferPassUsage), bufferIDTyped, bufferName, passIndex);
        }
    }

    const DynamicArray<ImageMutableResource>& RenderGraphResources::GetColorClears(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access colorClears of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].colorClears;
    }

    const DynamicArray<DepthImageMutableResource>& RenderGraphResources::GetDepthClears(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access depthClears of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].depthClears;
    }

    const DynamicArray<TrackedImageAccess>& RenderGraphResources::GetImageAccesses(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access imageAccesses of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].imageAccesses;
    }
    const DynamicArray<TrackedDepthImageAccess>& RenderGraphResources::GetDepthImageAccesses(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access depthImageAccesses of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].depthImageAccesses;
    }

    const TrackedBufferBitSets& RenderGraphResources::GetBufferPermissions(u32 passIndex)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        if (passIndex >= data->trackedPasses.Count())
        {
            DebugHandler::PrintFatal("Tried to access bufferAccesses of pass that hasn't been tracked yet");
        }

        return data->trackedPasses[passIndex].bufferPermissions;
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

    const DynamicArray<TrackedImagePassAccess>& RenderGraphResources::GetPassAccesses(ImageID imageID)
    {
        RenderGraphResourcesData* data = static_cast<RenderGraphResourcesData*>(_data);

        ImageID::type index = static_cast<ImageID::type>(imageID);
        if (index >= data->trackedImageAccesses.Count())
        {
            DebugHandler::PrintFatal("Tried to access trackedImageAccesses of ImageID that hasn't been tracked yet");
        }

        return data->trackedImageAccesses[index];
    }

    const DynamicArray<TrackedImagePassAccess>& RenderGraphResources::GetPassAccesses(DepthImageID imageID)
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