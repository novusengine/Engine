#pragma once
#include "RenderPassResources.h"
#include "TrackedBufferBitSets.h"
#include "DescriptorSetResource.h"
#include "Descriptors/TextureDesc.h"
#include "Descriptors/ImageDesc.h"
#include "Descriptors/DepthImageDesc.h"
#include "Descriptors/BufferDesc.h"

#include <Base/Types.h>
#include <Base/Memory/Allocator.h>
#include <Base/Container/DynamicArray.h>

namespace Renderer
{
    class Renderer;
    class DescriptorSet;
    struct GraphicsPipelineDesc;
    struct ComputePipelineDesc;

    struct IRenderGraphResourcesData {};

    enum class LoadMode : u8
    {
        LOAD, // Load the contents of the resource
        DISCARD, // We don't really care
        CLEAR // Clear the resource of existing data
    };

    struct TrackedImagePassAccess
    {
        u32 passIndex;
        AccessType accessType;
        PipelineType pipelineType;
    };

    struct TrackedImageAccess
    {
        ImageID imageID;
        AccessType accessType;
        PipelineType pipelineType;
    };

    struct TrackedDepthImageAccess
    {
        DepthImageID imageID;
        AccessType accessType;
        PipelineType pipelineType;
    };

    struct TrackedBufferPassAccess
    {
        u32 passIndex;
        AccessType accessType;
        BufferPassUsage bufferPassUsage;
    };

    struct TrackedBufferAccess
    {
        BufferID bufferID;
        AccessType accessType;
        BufferPassUsage bufferPassUsage;
    };

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

        const ImageDesc& GetImageDesc(ImageResource resource);
        const ImageDesc& GetImageDesc(ImageMutableResource resource);
        const DepthImageDesc& GetImageDesc(DepthImageResource resource);
        const DepthImageDesc& GetImageDesc(DepthImageMutableResource resource);

        uvec2 GetImageDimensions(ImageResource resource, u32 mipLevel = 0);
        uvec2 GetImageDimensions(ImageMutableResource resource, u32 mipLevel = 0);
        uvec2 GetImageDimensions(DepthImageResource resource);
        uvec2 GetImageDimensions(DepthImageMutableResource resource);

    private:
        RenderGraphResources(Memory::Allocator* allocator, Renderer* renderer, size_t numPasses, u32 numTotalBuffers);

        ImageID GetImage(ImageResource resource);
        ImageID GetImage(ImageMutableResource resource);
        DepthImageID GetImage(DepthImageResource resource);
        DepthImageID GetImage(DepthImageMutableResource resource);

        BufferID GetBuffer(BufferResource resource);
        BufferID GetBuffer(BufferMutableResource resource);

        DescriptorSet* GetDescriptorSet(DescriptorSetID resource);

        ImageResource GetResource(ImageID id);
        DepthImageResource GetResource(DepthImageID id);
        BufferResource GetResource(BufferID id);
        DescriptorSetResource GetResource(DescriptorSet& descriptorSet);

        ImageMutableResource GetMutableResource(ImageID id);
        DepthImageMutableResource GetMutableResource(DepthImageID id);
        BufferMutableResource GetMutableResource(BufferID id);

        void Clear(u32 passIndex, ImageMutableResource resource);
        void Clear(u32 passIndex, DepthImageMutableResource resource);

        void Access(u32 passIndex, ImageID imageID, AccessType accessType, PipelineType pipelineType);
        void Access(u32 passIndex, DepthImageID imageID, AccessType accessType, PipelineType pipelineType);
        void Access(u32 passIndex, BufferID imageID, AccessType accessType, BufferPassUsage bufferPassUsage);

        void Use(u32 passIndex, DescriptorSetResource resource);

        void EnforceHasAccess(u32 passIndex, BufferResource resource, BufferPassUsage bufferPassUsage);
        void EnforceHasAccess(u32 passIndex, BufferMutableResource resource, BufferPassUsage bufferPassUsage);

        const DynamicArray<ImageMutableResource>& GetColorClears(u32 passIndex);
        const DynamicArray<DepthImageMutableResource>& GetDepthClears(u32 passIndex);

        const DynamicArray<TrackedImageAccess>& GetImageAccesses(u32 passIndex);
        const DynamicArray<TrackedDepthImageAccess>& GetDepthImageAccesses(u32 passIndex);

        const TrackedBufferBitSets& GetBufferPermissions(u32 passIndex);

        const DynamicArray<DescriptorSetID>& GetUsedDescriptorSetIDs(u32 passIndex);

        const DynamicArray<TrackedImagePassAccess>& GetPassAccesses(ImageID imageID);
        const DynamicArray<TrackedImagePassAccess>& GetPassAccesses(DepthImageID imageID);

        u32 GetLastBarrier(ImageID imageID);
        u32 GetLastBarrier(DepthImageID imageID);

        void SetLastBarrier(ImageID imageID, u32 passID);
        void SetLastBarrier(DepthImageID imageID, u32 passID);

        void SetPassName(std::string passName);

    private:
        Renderer* _renderer = nullptr;
        Memory::Allocator* _allocator = nullptr;

        IRenderGraphResourcesData* _data = nullptr;

        friend class RenderGraph;
        friend class RenderGraphBuilder;
        friend class CommandList;
        friend class DescriptorSetResource;
    };
}