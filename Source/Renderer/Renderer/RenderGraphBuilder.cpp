#include "RenderGraphBuilder.h"
#include "Renderer.h"
#include "RenderGraph.h"

namespace Renderer
{
    BitSet* RenderGraphBuilder::_dirty = nullptr;

    BitSet* RenderGraphBuilder::_lastWriteWasTransfer = nullptr;
    BitSet* RenderGraphBuilder::_lastWriteWasGraphics = nullptr;
    BitSet* RenderGraphBuilder::_lastWriteWasCompute = nullptr;

    RenderGraphBuilder::RenderGraphBuilder(Memory::Allocator* allocator, Renderer* renderer, size_t numPasses, u32 numTotalBuffers)
        : _allocator(allocator)
        , _renderer(renderer)
        , _resources(allocator, renderer, numPasses, numTotalBuffers)
    {
        ZoneScoped;
        // Create new dirty bitsets in the new per-frame allocator, and copy over the old bits if we had bitsets last frame
        BitSet* oldDirty = _dirty;

        BitSet* oldLastWriteWasTransfer = _lastWriteWasTransfer;
        BitSet* oldLastWriteWasGraphics = _lastWriteWasGraphics;
        BitSet* oldLastWriteWasCompute = _lastWriteWasCompute;

        u32 numBitSets = Math::FloorToInt(static_cast<f32>(_renderer->GetNumBuffers()) / 64.0f) + 1;
        _dirty = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);

        _lastWriteWasTransfer = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
        _lastWriteWasGraphics = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
        _lastWriteWasCompute = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);

        if (oldDirty != nullptr)
            _dirty->SetEquals(*oldDirty);
        else
            _dirty->Reset();

        if (oldLastWriteWasTransfer != nullptr)
            _lastWriteWasTransfer->SetEquals(*oldLastWriteWasTransfer);
        else
            _lastWriteWasTransfer->Reset();

        if (oldLastWriteWasGraphics != nullptr)
            _lastWriteWasGraphics->SetEquals(*oldLastWriteWasGraphics);
        else
            _lastWriteWasGraphics->Reset();

        if (oldLastWriteWasCompute != nullptr)
            _lastWriteWasCompute->SetEquals(*oldLastWriteWasCompute);
        else
            _lastWriteWasCompute->Reset();
    }

    void RenderGraphBuilder::PrePass(CommandList& commandList, u32 currentPassIndex, const std::string& passName)
    {
        ZoneScoped;
        // Queue up all the clears for this pass
        commandList.PushMarker("RenderGraph::PreExecute", Color::White);

        {
            ZoneScopedN("Clears");

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
        }
        
        // Handling between-pass Image barriers
        {
            ZoneScopedN("Image Barriers");
            const DynamicArray<TrackedImageAccess>& imageAccesses = _resources.GetImageAccesses(currentPassIndex);
            for (const TrackedImageAccess& imageAccess : imageAccesses)
            {
                u32 lastBarrier = _resources.GetLastBarrier(imageAccess.imageID);

                const DynamicArray<TrackedImagePassAccess>& passAccesses = _resources.GetPassAccesses(imageAccess.imageID);

                for (i32 i = static_cast<i32>(passAccesses.Count()) - 1; i >= 0; i--)
                {
                    const TrackedImagePassAccess& passAccess = passAccesses[i];

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
                            ImageMutableResource resource = _resources.GetMutableResource(imageAccess.imageID);
                            commandList.ImageBarrier(resource);
                            _numPlacedImageBarriers++;
                        }
                        else
                        {
                            ImageResource resource = _resources.GetResource(imageAccess.imageID);
                            commandList.ImageBarrier(resource);
                            _numPlacedImageBarriers++;
                        }

                        break;
                    }
                }
            }

            const DynamicArray<TrackedDepthImageAccess>& depthImageAccesses = _resources.GetDepthImageAccesses(currentPassIndex);
            for (const TrackedDepthImageAccess& imageAccess : depthImageAccesses)
            {
                u32 lastBarrier = _resources.GetLastBarrier(imageAccess.imageID);

                const DynamicArray<TrackedImagePassAccess>& passAccesses = _resources.GetPassAccesses(imageAccess.imageID);

                for (i32 i = static_cast<i32>(passAccesses.Count()) - 1; i >= 0; i--)
                {
                    const TrackedImagePassAccess& passAccess = passAccesses[i];

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
                            _numPlacedImageBarriers++;
                        }
                        else
                        {
                            DepthImageResource resource = _resources.GetResource(imageAccess.imageID);
                            commandList.ImageBarrier(resource);
                            _numPlacedImageBarriers++;
                        }

                        break;
                    }
                }
            }
        }

        {
            ZoneScopedN("Buffer Barriers");

            BitSet temp(_allocator, 2); // TODO: Get numBitSets here

            // Combine current pass read and write
            const TrackedBufferBitSets& currentPermissions = _resources.GetBufferPermissions(currentPassIndex);
            const BitSet& readAccess = currentPermissions.GetReadBitSet();
            const BitSet& writeAccess = currentPermissions.GetWriteBitSet();
            {
                ZoneScopedN("Combine BitSets");

                temp.SetEquals(readAccess);
                temp.BitwiseOR(writeAccess);

                // AND between current pass access and the dirty flags will tell us which buffers need a barrier
                temp.BitwiseAND(*_dirty);
            }

            // Add barriers
            const BitSet& transferAccess = currentPermissions.GetTransferBitSet();
            const BitSet& graphicsAccess = currentPermissions.GetGraphicsBitSet();
            const BitSet& computeAccess = currentPermissions.GetComputeBitSet();

            {
                ZoneScopedN("Place Barriers");
                temp.ForEachSetBit([&](u32 set, u32 bit)
                {
                    u32 bufferIndex = set * 64 + bit;

                    BufferPassUsage from = BufferPassUsage::NONE;

                    if (_lastWriteWasTransfer->Has(bufferIndex))
                    {
                        from |= BufferPassUsage::TRANSFER;
                    }
                    if (_lastWriteWasGraphics->Has(bufferIndex))
                    {
                        from |= BufferPassUsage::GRAPHICS;
                    }
                    if (_lastWriteWasCompute->Has(bufferIndex))
                    {
                        from |= BufferPassUsage::COMPUTE;
                    }

                    BufferID bufferID = BufferID(bufferIndex);
                    commandList.BufferBarrier(bufferID, from);
                    _numPlacedBufferBarriers++;
                });
            }
            
            // Calculate how this pass affected the dirty sets
            {
                ZoneScopedN("Affect Dirty");
                const TrackedBufferBitSets& bufferPermissions = _resources.GetBufferPermissions(currentPassIndex);

                const BitSet& readPermissions = bufferPermissions.GetReadBitSet();

                const BitSet& transferAccess = bufferPermissions.GetTransferBitSet();
                const BitSet& graphicsAccess = bufferPermissions.GetGraphicsBitSet();
                const BitSet& computeAccess = bufferPermissions.GetComputeBitSet();

                // First we want to unset any reads that got barriered by this pass
                _dirty->BitwiseUnset(readPermissions);

                // Then we want to set any writes made by this pass
                const BitSet& writePermissions = bufferPermissions.GetWriteBitSet();
                _dirty->BitwiseOR(writePermissions);

                {
                    ZoneScopedN("Iterate Bitset");
                    writeAccess.ForEachSetBit([&](u32 set, u32 bit)
                    {
                        u32 bufferIndex = set * 64 + bit;

                        if (transferAccess.Has(bufferIndex))
                            _lastWriteWasTransfer->Set(bufferIndex);
                        else
                            _lastWriteWasTransfer->Unset(bufferIndex);

                        if (graphicsAccess.Has(bufferIndex))
                            _lastWriteWasGraphics->Set(bufferIndex);
                        else
                            _lastWriteWasGraphics->Unset(bufferIndex);

                        if (computeAccess.Has(bufferIndex))
                            _lastWriteWasCompute->Set(bufferIndex);
                        else
                            _lastWriteWasCompute->Unset(bufferIndex);
                    });
                }
            }
        }

        // Lock the DescriptorSets so we have to go through the DescriptorSetResource
        const DynamicArray<DescriptorSetID>& usedDescriptorSets = _resources.GetUsedDescriptorSetIDs(currentPassIndex);
        for (DescriptorSetID descriptorSetID : usedDescriptorSets)
        {
            DescriptorSet* descriptorSet = _resources.GetDescriptorSet(descriptorSetID);
            descriptorSet->Lock();
        }

        commandList.PopMarker();
    }

    void RenderGraphBuilder::PostPass(CommandList& /*commandList*/, u32 currentPassIndex, const std::string& passName)
    {
        // Unlock the DescriptorSets so we can use them again
        const DynamicArray<DescriptorSetID>& usedDescriptorSets = _resources.GetUsedDescriptorSetIDs(currentPassIndex);
        for (DescriptorSetID descriptorSetID : usedDescriptorSets)
        {
            DescriptorSet* descriptorSet = _resources.GetDescriptorSet(descriptorSetID);
            descriptorSet->Unlock();
        }
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
        ZoneScoped;
        DebugHandler::Assert(id != ImageID::Invalid(), "RenderGraphBuilder : Read got invalid ImageID");

        ImageResource resource = _resources.GetResource(id);

        _resources.Access(_currentPassIndex, id, AccessType::READ, pipelineType);

        return resource;
    }

    DepthImageResource RenderGraphBuilder::Read(DepthImageID id, PipelineType pipelineType)
    {
        ZoneScoped;
        DebugHandler::Assert(id != DepthImageID::Invalid(), "RenderGraphBuilder : Read got invalid DepthImageID");

        DepthImageResource resource = _resources.GetResource(id);

        _resources.Access(_currentPassIndex, id, AccessType::READ, pipelineType);

        return resource;
    }

    BufferResource RenderGraphBuilder::Read(BufferID id, BufferPassUsage bufferPassUsage)
    {
        ZoneScoped;
        DebugHandler::Assert(id != BufferID::Invalid(), "RenderGraphBuilder : Read got invalid BufferID");

        BufferResource resource = _resources.GetResource(id);

        _resources.Access(_currentPassIndex, id, AccessType::READ, bufferPassUsage);

        return resource;
    }

    ImageMutableResource RenderGraphBuilder::Write(ImageID id, PipelineType pipelineType, LoadMode loadMode)
    {
        ZoneScoped;
        DebugHandler::Assert(id != ImageID::Invalid(), "RenderGraphBuilder : Write got invalid ImageID");

        ImageMutableResource resource = _resources.GetMutableResource(id);

        if (loadMode == LoadMode::CLEAR)
        {
            _resources.Clear(_currentPassIndex, resource);
        }

        _resources.Access(_currentPassIndex, id, AccessType::WRITE, pipelineType);

        return resource;
    }

    DepthImageMutableResource RenderGraphBuilder::Write(DepthImageID id, PipelineType pipelineType, LoadMode loadMode)
    {
        ZoneScoped;
        DebugHandler::Assert(id != DepthImageID::Invalid(), "RenderGraphBuilder : Write got invalid DepthImageID");

        DepthImageMutableResource resource = _resources.GetMutableResource(id);

        if (loadMode == LoadMode::CLEAR)
        {
            _resources.Clear(_currentPassIndex, resource);
        }

        _resources.Access(_currentPassIndex, id, AccessType::WRITE, pipelineType);

        return resource;
    }

    BufferMutableResource RenderGraphBuilder::Write(BufferID id, BufferPassUsage bufferPassUsage)
    {
        ZoneScoped;
        DebugHandler::Assert(id != BufferID::Invalid(), "RenderGraphBuilder : Write got invalid BufferID");

        BufferMutableResource resource = _resources.GetMutableResource(id);

        _resources.Access(_currentPassIndex, id, AccessType::WRITE, bufferPassUsage);

        return resource;
    }

    DescriptorSetResource RenderGraphBuilder::Use(DescriptorSet& descriptorSet)
    {
        ZoneScoped;
        DescriptorSetResource resource = _resources.GetResource(descriptorSet);

        _resources.Use(_currentPassIndex, resource);

        return resource;
    }
}