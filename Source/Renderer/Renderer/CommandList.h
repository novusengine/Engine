#pragma once
#include "DescriptorSet.h"
#include "BackendDispatch.h"
#include "BackendDispatchFn.h"
#include "Descriptors/BufferDesc.h"
#include "Descriptors/CommandListDesc.h"
#include "Descriptors/GraphicsPipelineDesc.h"
#include "Descriptors/ComputePipelineDesc.h"
#include "Descriptors/SemaphoreDesc.h"
#include "Descriptors/TimeQueryDesc.h"

#include <Base/Types.h>
#include <Base/Memory/StackAllocator.h>
#include <Base/Container/DynamicArray.h>

#include <tracy/Tracy.hpp>
//#include <tracy/TracyVulkan.hpp>

#define COMMANDLIST_DEBUG_IMMEDIATE_MODE 1 // This makes it easier to debug the renderer by providing better callstacks if it asserts or crashes inside of render-lib

// TODO: Fix this
#if 0//TRACY_ENABLE
#define GPU_SCOPED_PROFILER_ZONE(commandList, name) \
    TracySourceLocation(name, #name, tracy::Color::Yellow2); \
    Renderer::ScopedGPUProfilerZone nameZone(commandList, &name);
#else
namespace tracy
{
    struct SourceLocationData;
}
#define GPU_SCOPED_PROFILER_ZONE(commandList, name) ((void)0)
#endif

namespace Renderer
{
    inline u32 GetDispatchCount(u32 num, u32 threadGroupSize)
    {
        return (num + (threadGroupSize - 1)) / threadGroupSize;
    }

    class DescriptorSet;
    class CommandList;
    class RenderGraphResources;

    struct ScopedGPUProfilerZone
    {
        ScopedGPUProfilerZone(CommandList& commandList, const tracy::SourceLocationData* sourceLocation);
        ~ScopedGPUProfilerZone();

    private:
        CommandList& _commandList;
    };

    class CommandList
    {
    public:
        CommandList(Renderer* renderer, Memory::Allocator* allocator, RenderGraphResources* resources);

        void MarkFrameStart(u32 frameIndex);

        void BeginTrace(const tracy::SourceLocationData* sourceLocation);
        void EndTrace();

        void PushMarker(std::string marker, Color color);
        void PopMarker();

        void BeginPipeline(GraphicsPipelineID pipelineID);
        void EndPipeline(GraphicsPipelineID pipelineID);

        void BeginPipeline(ComputePipelineID pipelineID);
        void EndPipeline(ComputePipelineID pipelineID);

        void BeginTimeQuery(TimeQueryID timeQueryID);
        void EndTimeQuery(TimeQueryID timeQueryID);

        void BindDescriptorSet(DescriptorSetSlot slot, DescriptorSetResource resource, u32 frameIndex);

        void SetDepthBias(f32 constantFactor, f32 clamp, f32 slopeFactor);
        void SetScissorRect(u32 left, u32 right, u32 top, u32 bottom);
        void SetViewport(f32 topLeftX, f32 topLeftY, f32 width, f32 height, f32 minDepth, f32 maxDepth);

        void SetIndexBuffer(BufferID buffer, IndexFormat indexFormat);
        void SetBuffer(u32 slot, BufferID buffer);

        void Clear(ImageMutableResource resource, Color color);
        void Clear(ImageMutableResource resource, uvec4 values);
        void Clear(ImageMutableResource resource, ivec4 values);
        void Clear(DepthImageMutableResource resource, f32 depth, DepthClearFlags flags = DepthClearFlags::DEPTH, u8 stencil = 0);

        void Draw(u32 numVertices, u32 numInstances, u32 vertexOffset, u32 instanceOffset);
        void DrawIndirect(BufferResource argumentResource, u32 argumentBufferOffset, u32 drawCount);
        void DrawIndirect(BufferMutableResource argumentResource, u32 argumentBufferOffset, u32 drawCount);
        void DrawIndexed(u32 numIndices, u32 numInstances, u32 indexOffset, u32 vertexOffset, u32 instanceOffset);
        void DrawIndexedIndirect(BufferResource argumentResource, u32 argumentBufferOffset, u32 drawCount);
        void DrawIndexedIndirect(BufferMutableResource argumentResource, u32 argumentBufferOffset, u32 drawCount);
        void DrawIndexedIndirectCount(BufferResource argumentResource, u32 argumentBufferOffset, BufferResource drawCountResource, u32 drawCountBufferOffset, u32 maxDrawCount);
        void DrawIndexedIndirectCount(BufferResource argumentResource, u32 argumentBufferOffset, BufferMutableResource drawCountResource, u32 drawCountBufferOffset, u32 maxDrawCount);
        void DrawIndexedIndirectCount(BufferMutableResource argumentResource, u32 argumentBufferOffset, BufferResource drawCountResource, u32 drawCountBufferOffset, u32 maxDrawCount);
        void DrawIndexedIndirectCount(BufferMutableResource argumentResource, u32 argumentBufferOffset, BufferMutableResource drawCountResource, u32 drawCountBufferOffset, u32 maxDrawCount);

        void Dispatch(u32 numThreadGroupsX, u32 numThreadGroupsY, u32 numThreadGroupsZ);
        void DispatchIndirect(BufferResource argumentResource, u32 argumentBufferOffset);
        void DispatchIndirect(BufferMutableResource argumentResource, u32 argumentBufferOffset);

        void AddSignalSemaphore(SemaphoreID semaphoreID);
        void AddWaitSemaphore(SemaphoreID semaphoreID);

        void CopyImage(ImageID dstImage, uvec2 dstPos, u32 dstMipLevel, ImageID srcImage, uvec2 srcPos, u32 srcMipLevel, uvec2 size);
        void CopyDepthImage(DepthImageID dstImage, uvec2 dstPos, DepthImageID srcImage, uvec2 srcPos, uvec2 size);

        void CopyBuffer(BufferMutableResource dstBuffer, u64 dstBufferOffset, BufferResource srcBuffer, u64 srcBufferOffset, u64 region);
        void CopyBuffer(BufferMutableResource dstBuffer, u64 dstBufferOffset, BufferMutableResource srcBuffer, u64 srcBufferOffset, u64 region);
        void FillBuffer(BufferMutableResource dstBuffer, u64 dstBufferOffset, u64 size, u32 data);
        void UpdateBuffer(BufferMutableResource dstBuffer, u64 dstBufferOffset, u64 size, void* data);

        void ImageBarrier(ImageResource resource);
        void ImageBarrier(ImageMutableResource resource);
        void ImageBarrier(DepthImageResource resource);
        void ImageBarrier(DepthImageMutableResource resource);
        void BufferBarrier(BufferResource resource, BufferPassUsage from);
        void BufferBarrier(BufferMutableResource resource, BufferPassUsage from);
        
        void DrawImgui();

        void PushConstant(void* data, u32 offset, u32 size);

    private:
        // Execute gets friend-called from RenderGraph
        void Execute();
        // BufferBarrier taking BufferIDs gets friend-called from RenderGraphBuilder
        void BufferBarrier(BufferID bufferID, BufferPassUsage from);

        template<typename Command>
        Command* AddCommand()
        {
            Command* command = AllocateCommand<Command>();

            AddFunction(Command::DISPATCH_FUNCTION);
            AddData(command);

            return command;
        }

        template<typename Command>
        Command* AllocateCommand()
        {
            assert(_allocator != nullptr);

            return Memory::Allocator::New<Command>(_allocator);
        }

        void AddFunction(const BackendDispatchFunction& function)
        {
            _functions.Insert(function);
        }

        void AddData(void* data)
        {
            _data.Insert(data);
        }

        void SetCurrentPassIndex(u32 currentPassIndex)
        {
            _currentPassIndex = currentPassIndex;
        }

    private:
        Renderer* _renderer;
        Memory::Allocator* _allocator;
        RenderGraphResources* _resources;
        u32 _markerScope;

        u32 _currentPassIndex;

        DynamicArray<BackendDispatchFunction> _functions;
        DynamicArray<void*> _data;

        bool _isTracing = false;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        CommandListID _immediateCommandList = CommandListID::Invalid();
#endif

        friend class RenderGraph;
        friend class RenderGraphBuilder;
    };
}