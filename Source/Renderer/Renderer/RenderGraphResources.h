#pragma once
#include "RenderPassResources.h"
#include "DescriptorSetResource.h"
#include "Descriptors/TextureDesc.h"
#include "Descriptors/ImageDesc.h"
#include "Descriptors/DepthImageDesc.h"

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

    enum class AccessType : u8
    {
        READ,
        WRITE
    };

    enum class PipelineType : u8
    {
        GRAPHICS,
        COMPUTE,
        BOTH
    };

    enum class LoadMode : u8
    {
        LOAD, // Load the contents of the resource
        DISCARD, // We don't really care
        CLEAR // Clear the resource of existing data
    };

    struct TrackedPassAccess
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
        RenderGraphResources(Memory::Allocator* allocator, Renderer* renderer, size_t numPasses);

        ImageID GetImage(ImageResource resource);
        ImageID GetImage(ImageMutableResource resource);
        DepthImageID GetImage(DepthImageResource resource);
        DepthImageID GetImage(DepthImageMutableResource resource);
        DescriptorSet* GetDescriptorSet(DescriptorSetID resource);

        ImageResource GetResource(ImageID id);
        ImageResource GetResource(TextureID id);
        DepthImageResource GetResource(DepthImageID id);
        DescriptorSetResource GetResource(DescriptorSet& descriptorSet);

        ImageMutableResource GetMutableResource(ImageID id);
        DepthImageMutableResource GetMutableResource(DepthImageID id);

        void Clear(u32 passIndex, ImageMutableResource resource);
        void Clear(u32 passIndex, DepthImageMutableResource resource);

        void Access(u32 passIndex, ImageResource resource, AccessType accessType, PipelineType pipelineType);
        void Access(u32 passIndex, ImageMutableResource resource, AccessType accessType, PipelineType pipelineType);
        void Access(u32 passIndex, DepthImageResource resource, AccessType accessType, PipelineType pipelineType);
        void Access(u32 passIndex, DepthImageMutableResource resource, AccessType accessType, PipelineType pipelineType);

        void Use(u32 passIndex, DescriptorSetResource resource);

        bool NeedsPreExecute(u32 passIndex);
        bool NeedsPostExecute(u32 passIndex);

        const DynamicArray<ImageMutableResource>& GetColorClears(u32 passIndex);
        const DynamicArray<DepthImageMutableResource>& GetDepthClears(u32 passIndex);

        const DynamicArray<TrackedImageAccess>& GetImageAccesses(u32 passIndex);
        const DynamicArray<TrackedDepthImageAccess>& GetDepthImageAccesses(u32 passIndex);

        const DynamicArray<DescriptorSetID>& GetUsedDescriptorSetIDs(u32 passIndex);

        const DynamicArray<TrackedPassAccess>& GetPassAccesses(ImageID imageID);
        const DynamicArray<TrackedPassAccess>& GetPassAccesses(DepthImageID imageID);

        u32 GetLastBarrier(ImageID imageID);
        u32 GetLastBarrier(DepthImageID imageID);

        void SetLastBarrier(ImageID imageID, u32 passID);
        void SetLastBarrier(DepthImageID imageID, u32 passID);

    private:
        Renderer* _renderer = nullptr;
        Memory::Allocator* _allocator = nullptr;

        IRenderGraphResourcesData* _data = nullptr;

        friend class RenderGraphBuilder;
        friend class CommandList;
        friend class DescriptorSetResource;
    };
}