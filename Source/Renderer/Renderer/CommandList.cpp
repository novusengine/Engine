#include "CommandList.h"
#include "Renderer.h"
#include "RenderGraphResources.h"
#include "DescriptorSetResource.h"

// Commands
#include "Commands/Clear.h"
#include "Commands/Draw.h"
#include "Commands/DrawIndirect.h"
#include "Commands/DrawIndexed.h"
#include "Commands/DrawIndexedIndirect.h"
#include "Commands/DrawIndexedIndirectCount.h"
#include "Commands/Dispatch.h"
#include "Commands/DispatchIndirect.h"
#include "Commands/PopMarker.h"
#include "Commands/PushMarker.h"
#include "Commands/SetPipeline.h"
#include "Commands/SetDepthBias.h"
#include "Commands/SetScissorRect.h"
#include "Commands/SetViewport.h"
#include "Commands/SetVertexBuffer.h"
#include "Commands/SetIndexBuffer.h"
#include "Commands/SetBuffer.h"
#include "Commands/BindDescriptorSet.h"
#include "Commands/MarkFrameStart.h"
#include "Commands/BeginTrace.h"
#include "Commands/EndTrace.h"
#include "Commands/AddSignalSemaphore.h"
#include "Commands/AddWaitSemaphore.h"
#include "Commands/CopyImage.h"
#include "Commands/CopyBuffer.h"
#include "Commands/FillBuffer.h"
#include "Commands/UpdateBuffer.h"
#include "Commands/ImageBarrier.h"
#include "Commands/DepthImageBarrier.h"
#include "Commands/BufferBarrier.h"
#include "Commands/DrawImgui.h"
#include "Commands/PushConstant.h"

namespace Renderer
{
    class ScopedMarker
    {
    public:
        ScopedMarker(CommandList& commandList, std::string marker, const Color& color)
            : _commandList(commandList)
        {
            _commandList.PushMarker(marker, color);
        }
        ~ScopedMarker()
        {
            _commandList.PopMarker();
        }

    private:
        CommandList& _commandList;
    };

    ScopedGPUProfilerZone::ScopedGPUProfilerZone(CommandList& commandList, const tracy::SourceLocationData* sourceLocation)
        : _commandList(commandList)
    {
        _commandList.BeginTrace(sourceLocation);
    }

    ScopedGPUProfilerZone::~ScopedGPUProfilerZone()
    {
        _commandList.EndTrace();
    }

    void CommandList::Execute()
    {
#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        _renderer->EndCommandList(_immediateCommandList);
#else
        assert(_markerScope == 0); // We need to pop all markers that we push

        CommandListID commandList = _renderer->BeginCommandList();

        {
            ZoneScopedNC("Record commandlist", tracy::Color::Red2)
            // Execute each command
            for (int i = 0; i < _functions.Count(); i++)
            {
                _functions[i](_renderer, commandList, _data[i]);
            }
        }
        _renderer->EndCommandList(commandList);
#endif
    }

    void CommandList::BufferBarrier(BufferID bufferID, BufferPassUsage from)
    {
        Commands::BufferBarrier* command = AddCommand<Commands::BufferBarrier>();
        command->bufferID = bufferID;
        command->from = from;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::BufferBarrier::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    CommandList::CommandList(Renderer* renderer, Memory::Allocator* allocator, RenderGraphResources* resources)
        : _renderer(renderer)
        , _allocator(allocator)
        , _resources(resources)
        , _markerScope(0)
        , _functions(allocator, 32)
        , _data(allocator, 32)
    {
#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        _immediateCommandList = _renderer->BeginCommandList();
#endif
    }

    void CommandList::MarkFrameStart(u32 frameIndex)
    {
        Commands::MarkFrameStart* command = AddCommand<Commands::MarkFrameStart>();
        command->frameIndex = frameIndex;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::MarkFrameStart::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::BeginTrace(const tracy::SourceLocationData* sourceLocation)
    {
        Commands::BeginTrace* command = AddCommand<Commands::BeginTrace>();
        command->sourceLocation = sourceLocation;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::BeginTrace::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::EndTrace()
    {
        Commands::EndTrace* command = AddCommand<Commands::EndTrace>();

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::EndTrace::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::PushMarker(std::string marker, Color color)
    {
        Commands::PushMarker* command = AddCommand<Commands::PushMarker>();
        assert(marker.length() < Commands::PushMarker::MARKER_MAX_LENGTH); // Max length of marker names is enforced because we have to store the string internally
        strcpy_s(command->marker, marker.c_str());
        command->color = color;

        _markerScope++;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::PushMarker::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::PopMarker()
    {
        Commands::PopMarker* command = AddCommand<Commands::PopMarker>();

        assert(_markerScope > 0); // We tried to pop a marker we never pushed
        _markerScope--;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::PopMarker::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::BeginPipeline(GraphicsPipelineID pipelineID)
    {
        Commands::BeginGraphicsPipeline* command = AddCommand<Commands::BeginGraphicsPipeline>();
        command->pipeline = pipelineID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::BeginGraphicsPipeline::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::EndPipeline(GraphicsPipelineID pipelineID)
    {
        Commands::EndGraphicsPipeline* command = AddCommand<Commands::EndGraphicsPipeline>();
        command->pipeline = pipelineID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::EndGraphicsPipeline::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::BeginPipeline(ComputePipelineID pipelineID)
    {
        Commands::BeginComputePipeline* command = AddCommand<Commands::BeginComputePipeline>();
        command->pipeline = pipelineID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::BeginComputePipeline::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::EndPipeline(ComputePipelineID pipelineID)
    {
        Commands::EndComputePipeline* command = AddCommand<Commands::EndComputePipeline>();
        command->pipeline = pipelineID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::EndComputePipeline::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::BeginTimeQuery(TimeQueryID timeQueryID)
    {
        Commands::BeginTimeQuery* command = AddCommand<Commands::BeginTimeQuery>();
        command->timeQueryID = timeQueryID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::BeginTimeQuery::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::EndTimeQuery(TimeQueryID timeQueryID)
    {
        Commands::EndTimeQuery* command = AddCommand<Commands::EndTimeQuery>();
        command->timeQueryID = timeQueryID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::EndTimeQuery::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }
    
    void CommandList::BindDescriptorSet(DescriptorSetSlot slot, DescriptorSetResource resource, u32 frameIndex)
    {
        DescriptorSet* descriptorSet = _resources->GetDescriptorSet(resource.GetID());

        const std::vector<Descriptor>& descriptors = descriptorSet->GetDescriptors();
        size_t numDescriptors = descriptors.size();

        Commands::BindDescriptorSet* command = AddCommand<Commands::BindDescriptorSet>();
        command->slot = slot;

        // Make a copy of the current state of this DescriptorSets descriptors, this uses our per-frame stack allocator so it's gonna be fast and not leak
        command->descriptors = Memory::Allocator::NewArray<Descriptor>(_allocator, numDescriptors);
        memcpy(command->descriptors, descriptors.data(), sizeof(Descriptor) * numDescriptors);

        command->numDescriptors = static_cast<u32>(numDescriptors);

        command->bufferPermissions = &_resources->GetBufferPermissions(_currentPassIndex);

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::BindDescriptorSet::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::SetDepthBias(f32 constantFactor, f32 clamp, f32 slopeFactor)
    {
        Commands::SetDepthBias* command = AddCommand<Commands::SetDepthBias>();
        command->depthBias.constantFactor = constantFactor;
        command->depthBias.clamp = clamp;
        command->depthBias.slopeFactor = slopeFactor;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::SetDepthBias::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::SetScissorRect(u32 left, u32 right, u32 top, u32 bottom)
    {
        Commands::SetScissorRect* command = AddCommand<Commands::SetScissorRect>();
        command->scissorRect.left = left;
        command->scissorRect.right = right;
        command->scissorRect.top = top;
        command->scissorRect.bottom = bottom;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::SetScissorRect::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::SetViewport(f32 topLeftX, f32 topLeftY, f32 width, f32 height, f32 minDepth, f32 maxDepth)
    {
        Commands::SetViewport* command = AddCommand<Commands::SetViewport>();
        command->viewport.topLeftX = topLeftX;
        command->viewport.topLeftY = topLeftY;
        command->viewport.width = width;
        command->viewport.height = height;
        command->viewport.minDepth = minDepth;
        command->viewport.maxDepth = maxDepth;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::SetViewport::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::SetIndexBuffer(BufferID buffer, IndexFormat indexFormat)
    {
        Commands::SetIndexBuffer* command = AddCommand<Commands::SetIndexBuffer>();
        command->bufferID = buffer;
        command->indexFormat = indexFormat;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::SetIndexBuffer::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::SetBuffer(u32 slot, BufferID buffer)
    {
        Commands::SetBuffer* command = AddCommand<Commands::SetBuffer>();
        command->slot = slot;
        command->buffer = buffer;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::SetBuffer::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::Clear(ImageMutableResource resource, Color color)
    {
        ImageID imageID = _resources->GetImage(resource);

        Commands::ClearImageColor* command = AddCommand<Commands::ClearImageColor>();
        command->image = imageID;
        command->color = color;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::ClearImageColor::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::Clear(ImageMutableResource resource, uvec4 values)
    {
        ImageID imageID = _resources->GetImage(resource);

        Commands::ClearImageUInt* command = AddCommand<Commands::ClearImageUInt>();
        command->image = imageID;
        command->values = values;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::ClearImageUInt::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::Clear(ImageMutableResource resource, ivec4 values)
    {
        ImageID imageID = _resources->GetImage(resource);

        Commands::ClearImageInt* command = AddCommand<Commands::ClearImageInt>();
        command->image = imageID;
        command->values = values;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::ClearImageInt::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::Clear(DepthImageMutableResource resource, f32 depth, DepthClearFlags flags, u8 stencil)
    {
        DepthImageID depthImageID = _resources->GetImage(resource);

        Commands::ClearDepthImage* command = AddCommand<Commands::ClearDepthImage>();                                                                                                      
        command->image = depthImageID;
        command->depth = depth;
        command->flags = flags;
        command->stencil = stencil;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::ClearDepthImage::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::Draw(u32 numVertices, u32 numInstances, u32 vertexOffset, u32 instanceOffset)
    {
        Commands::Draw* command = AddCommand<Commands::Draw>();
        command->vertexCount = numVertices;
        command->instanceCount = numInstances;
        command->vertexOffset = vertexOffset;
        command->instanceOffset = instanceOffset;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::Draw::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DrawIndirect(BufferResource argumentResource, u32 argumentBufferOffset, u32 drawCount)
    {
        DebugHandler::Assert(argumentResource != BufferResource::Invalid(), "CommandList : DrawIndirect got invalid argumentResource");
        _resources->EnforceHasAccess(_currentPassIndex, argumentResource, BufferPassUsage::GRAPHICS);

        BufferID argumentBuffer = _resources->GetBuffer(argumentResource);

        Commands::DrawIndirect* command = AddCommand<Commands::DrawIndirect>();
        command->argumentBuffer = argumentBuffer;
        command->argumentBufferOffset = argumentBufferOffset;
        command->drawCount = drawCount;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DrawIndirect::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DrawIndirect(BufferMutableResource argumentResource, u32 argumentBufferOffset, u32 drawCount)
    {
        DebugHandler::Assert(argumentResource != BufferMutableResource::Invalid(), "CommandList : DrawIndirect got invalid argumentResource");
        _resources->EnforceHasAccess(_currentPassIndex, argumentResource, BufferPassUsage::GRAPHICS);

        BufferID argumentBuffer = _resources->GetBuffer(argumentResource);

        Commands::DrawIndirect* command = AddCommand<Commands::DrawIndirect>();
        command->argumentBuffer = argumentBuffer;
        command->argumentBufferOffset = argumentBufferOffset;
        command->drawCount = drawCount;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DrawIndirect::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DrawIndexed(u32 numIndices, u32 numInstances, u32 indexOffset, u32 vertexOffset, u32 instanceOffset)
    {
        Commands::DrawIndexed* command = AddCommand<Commands::DrawIndexed>();
        command->indexCount = numIndices;
        command->instanceCount = numInstances;
        command->indexOffset = indexOffset;
        command->vertexOffset = vertexOffset;
        command->instanceOffset = instanceOffset;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DrawIndexed::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DrawIndexedIndirect(BufferResource argumentResource, u32 argumentBufferOffset, u32 drawCount)
    {
        DebugHandler::Assert(argumentResource != BufferResource::Invalid(), "CommandList : DrawIndexedIndirect got invalid argumentResource");
        _resources->EnforceHasAccess(_currentPassIndex, argumentResource, BufferPassUsage::GRAPHICS);

        BufferID argumentBuffer = _resources->GetBuffer(argumentResource);

        Commands::DrawIndexedIndirect* command = AddCommand<Commands::DrawIndexedIndirect>();
        command->argumentBuffer = argumentBuffer;
        command->argumentBufferOffset = argumentBufferOffset;
        command->drawCount = drawCount;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DrawIndexedIndirect::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DrawIndexedIndirect(BufferMutableResource argumentResource, u32 argumentBufferOffset, u32 drawCount)
    {
        DebugHandler::Assert(argumentResource != BufferMutableResource::Invalid(), "CommandList : DrawIndexedIndirect got invalid argumentResource");
        _resources->EnforceHasAccess(_currentPassIndex, argumentResource, BufferPassUsage::GRAPHICS);

        BufferID argumentBuffer = _resources->GetBuffer(argumentResource);

        Commands::DrawIndexedIndirect* command = AddCommand<Commands::DrawIndexedIndirect>();
        command->argumentBuffer = argumentBuffer;
        command->argumentBufferOffset = argumentBufferOffset;
        command->drawCount = drawCount;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DrawIndexedIndirect::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DrawIndexedIndirectCount(BufferResource argumentResource, u32 argumentBufferOffset, BufferResource drawCountResource, u32 drawCountBufferOffset, u32 maxDrawCount)
    {
        DebugHandler::Assert(argumentResource != BufferResource::Invalid(), "CommandList : DrawIndexedIndirectCount got invalid argumentResource");
        DebugHandler::Assert(drawCountResource != BufferResource::Invalid(), "CommandList : DrawIndexedIndirectCount got invalid drawCountResource");
        _resources->EnforceHasAccess(_currentPassIndex, argumentResource, BufferPassUsage::GRAPHICS);
        _resources->EnforceHasAccess(_currentPassIndex, drawCountResource, BufferPassUsage::GRAPHICS);

        BufferID argumentBuffer = _resources->GetBuffer(argumentResource);
        BufferID drawCountBuffer = _resources->GetBuffer(drawCountResource);

        Commands::DrawIndexedIndirectCount* command = AddCommand<Commands::DrawIndexedIndirectCount>();
        command->argumentBuffer = argumentBuffer;
        command->argumentBufferOffset = argumentBufferOffset;
        command->drawCountBuffer = drawCountBuffer;
        command->drawCountBufferOffset = drawCountBufferOffset;
        command->maxDrawCount = maxDrawCount;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DrawIndexedIndirectCount::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DrawIndexedIndirectCount(BufferResource argumentResource, u32 argumentBufferOffset, BufferMutableResource drawCountResource, u32 drawCountBufferOffset, u32 maxDrawCount)
    {
        DebugHandler::Assert(argumentResource != BufferResource::Invalid(), "CommandList : DrawIndexedIndirectCount got invalid argumentResource");
        DebugHandler::Assert(drawCountResource != BufferMutableResource::Invalid(), "CommandList : DrawIndexedIndirectCount got invalid drawCountResource");
        _resources->EnforceHasAccess(_currentPassIndex, argumentResource, BufferPassUsage::GRAPHICS);
        _resources->EnforceHasAccess(_currentPassIndex, drawCountResource, BufferPassUsage::GRAPHICS);

        BufferID argumentBuffer = _resources->GetBuffer(argumentResource);
        BufferID drawCountBuffer = _resources->GetBuffer(drawCountResource);

        Commands::DrawIndexedIndirectCount* command = AddCommand<Commands::DrawIndexedIndirectCount>();
        command->argumentBuffer = argumentBuffer;
        command->argumentBufferOffset = argumentBufferOffset;
        command->drawCountBuffer = drawCountBuffer;
        command->drawCountBufferOffset = drawCountBufferOffset;
        command->maxDrawCount = maxDrawCount;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DrawIndexedIndirectCount::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DrawIndexedIndirectCount(BufferMutableResource argumentResource, u32 argumentBufferOffset, BufferResource drawCountResource, u32 drawCountBufferOffset, u32 maxDrawCount)
    {
        DebugHandler::Assert(argumentResource != BufferMutableResource::Invalid(), "CommandList : DrawIndexedIndirectCount got invalid argumentResource");
        DebugHandler::Assert(drawCountResource != BufferResource::Invalid(), "CommandList : DrawIndexedIndirectCount got invalid drawCountResource");
        _resources->EnforceHasAccess(_currentPassIndex, argumentResource, BufferPassUsage::GRAPHICS);
        _resources->EnforceHasAccess(_currentPassIndex, drawCountResource, BufferPassUsage::GRAPHICS);

        BufferID argumentBuffer = _resources->GetBuffer(argumentResource);
        BufferID drawCountBuffer = _resources->GetBuffer(drawCountResource);

        Commands::DrawIndexedIndirectCount* command = AddCommand<Commands::DrawIndexedIndirectCount>();
        command->argumentBuffer = argumentBuffer;
        command->argumentBufferOffset = argumentBufferOffset;
        command->drawCountBuffer = drawCountBuffer;
        command->drawCountBufferOffset = drawCountBufferOffset;
        command->maxDrawCount = maxDrawCount;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DrawIndexedIndirectCount::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DrawIndexedIndirectCount(BufferMutableResource argumentResource, u32 argumentBufferOffset, BufferMutableResource drawCountResource, u32 drawCountBufferOffset, u32 maxDrawCount)
    {
        DebugHandler::Assert(argumentResource != BufferMutableResource::Invalid(), "CommandList : DrawIndexedIndirectCount got invalid argumentResource");
        DebugHandler::Assert(drawCountResource != BufferMutableResource::Invalid(), "CommandList : DrawIndexedIndirectCount got invalid drawCountResource");
        _resources->EnforceHasAccess(_currentPassIndex, argumentResource, BufferPassUsage::GRAPHICS);
        _resources->EnforceHasAccess(_currentPassIndex, drawCountResource, BufferPassUsage::GRAPHICS);

        BufferID argumentBuffer = _resources->GetBuffer(argumentResource);
        BufferID drawCountBuffer = _resources->GetBuffer(drawCountResource);

        Commands::DrawIndexedIndirectCount* command = AddCommand<Commands::DrawIndexedIndirectCount>();
        command->argumentBuffer = argumentBuffer;
        command->argumentBufferOffset = argumentBufferOffset;
        command->drawCountBuffer = drawCountBuffer;
        command->drawCountBufferOffset = drawCountBufferOffset;
        command->maxDrawCount = maxDrawCount;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DrawIndexedIndirectCount::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::Dispatch(u32 numThreadGroupsX, u32 numThreadGroupsY, u32 numThreadGroupsZ)
    {
        assert(numThreadGroupsX > 0);
        assert(numThreadGroupsY > 0);
        assert(numThreadGroupsZ > 0);
        Commands::Dispatch* command = AddCommand<Commands::Dispatch>();
        command->threadGroupCountX = numThreadGroupsX;
        command->threadGroupCountY = numThreadGroupsY;
        command->threadGroupCountZ = numThreadGroupsZ;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::Dispatch::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DispatchIndirect(BufferResource argumentResource, u32 argumentBufferOffset)
    {
        DebugHandler::Assert(argumentResource != BufferResource::Invalid(), "CommandList : DispatchIndirect got invalid argumentResource");
        _resources->EnforceHasAccess(_currentPassIndex, argumentResource, BufferPassUsage::COMPUTE);

        BufferID argumentBuffer = _resources->GetBuffer(argumentResource);

        Commands::DispatchIndirect* command = AddCommand<Commands::DispatchIndirect>();
        command->argumentBuffer = argumentBuffer;
        command->argumentBufferOffset = argumentBufferOffset;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DispatchIndirect::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::DispatchIndirect(BufferMutableResource argumentResource, u32 argumentBufferOffset)
    {
        DebugHandler::Assert(argumentResource != BufferMutableResource::Invalid(), "CommandList : DispatchIndirect got invalid argumentResource");
        _resources->EnforceHasAccess(_currentPassIndex, argumentResource, BufferPassUsage::COMPUTE);

        BufferID argumentBuffer = _resources->GetBuffer(argumentResource);

        Commands::DispatchIndirect* command = AddCommand<Commands::DispatchIndirect>();
        command->argumentBuffer = argumentBuffer;
        command->argumentBufferOffset = argumentBufferOffset;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DispatchIndirect::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::AddSignalSemaphore(SemaphoreID semaphoreID)
    {
        Commands::AddSignalSemaphore* command = AddCommand<Commands::AddSignalSemaphore>();
        command->semaphore = semaphoreID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::AddSignalSemaphore::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::AddWaitSemaphore(SemaphoreID semaphoreID)
    {
        Commands::AddWaitSemaphore* command = AddCommand<Commands::AddWaitSemaphore>();
        command->semaphore = semaphoreID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::AddWaitSemaphore::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::CopyImage(ImageID dstImage, uvec2 dstPos, u32 dstMipLevel, ImageID srcImage, uvec2 srcPos, u32 srcMipLevel, uvec2 size)
    {
        assert(dstImage != ImageID::Invalid());
        assert(srcImage != ImageID::Invalid());
        Commands::CopyImage* command = AddCommand<Commands::CopyImage>();
        command->dstImage = dstImage;
        command->dstPos = dstPos;
        command->dstMipLevel = dstMipLevel;
        command->srcImage = srcImage;
        command->srcPos = srcPos;
        command->srcMipLevel = srcMipLevel;
        command->size = size;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::CopyImage::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::CopyDepthImage(DepthImageID dstImage, uvec2 dstPos, DepthImageID srcImage, uvec2 srcPos, uvec2 size)
    {
        assert(dstImage != DepthImageID::Invalid());
        assert(srcImage != DepthImageID::Invalid());
        Commands::CopyDepthImage* command = AddCommand<Commands::CopyDepthImage>();
        command->dstImage = dstImage;
        command->dstPos = dstPos;
        command->srcImage = srcImage;
        command->srcPos = srcPos;
        command->size = size;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::CopyDepthImage::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::CopyBuffer(BufferMutableResource dstResource, u64 dstBufferOffset, BufferResource srcResource, u64 srcBufferOffset, u64 region)
    {
        DebugHandler::Assert(dstResource != BufferMutableResource::Invalid(), "CommandList : CopyBuffer got invalid dstResource");
        DebugHandler::Assert(srcResource != BufferResource::Invalid(), "CommandList : CopyBuffer got invalid srcResource");
        _resources->EnforceHasAccess(_currentPassIndex, dstResource, BufferPassUsage::TRANSFER);
        _resources->EnforceHasAccess(_currentPassIndex, srcResource, BufferPassUsage::TRANSFER);

        BufferID dstBuffer = _resources->GetBuffer(dstResource);
        BufferID srcBuffer = _resources->GetBuffer(srcResource);

        Commands::CopyBuffer* command = AddCommand<Commands::CopyBuffer>();
        command->dstBuffer = dstBuffer;
        command->dstBufferOffset = dstBufferOffset;
        command->srcBuffer = srcBuffer;
        command->srcBufferOffset = srcBufferOffset;
        command->region = region;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::CopyBuffer::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::CopyBuffer(BufferMutableResource dstResource, u64 dstBufferOffset, BufferMutableResource srcResource, u64 srcBufferOffset, u64 region)
    {
        DebugHandler::Assert(dstResource != BufferMutableResource::Invalid(), "CommandList : CopyBuffer got invalid dstResource");
        DebugHandler::Assert(srcResource != BufferMutableResource::Invalid(), "CommandList : CopyBuffer got invalid srcResource");
        _resources->EnforceHasAccess(_currentPassIndex, dstResource, BufferPassUsage::TRANSFER);
        _resources->EnforceHasAccess(_currentPassIndex, srcResource, BufferPassUsage::TRANSFER);

        BufferID dstBuffer = _resources->GetBuffer(dstResource);
        BufferID srcBuffer = _resources->GetBuffer(srcResource);

        Commands::CopyBuffer* command = AddCommand<Commands::CopyBuffer>();
        command->dstBuffer = dstBuffer;
        command->dstBufferOffset = dstBufferOffset;
        command->srcBuffer = srcBuffer;
        command->srcBufferOffset = srcBufferOffset;
        command->region = region;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::CopyBuffer::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::FillBuffer(BufferMutableResource dstResource, u64 dstBufferOffset, u64 size, u32 data)
    {
        DebugHandler::Assert(dstResource != BufferMutableResource::Invalid(), "CommandList : FillBuffer got invalid dstResource");
        _resources->EnforceHasAccess(_currentPassIndex, dstResource, BufferPassUsage::TRANSFER);

        BufferID dstBuffer = _resources->GetBuffer(dstResource);

        Commands::FillBuffer* command = AddCommand<Commands::FillBuffer>();
        command->dstBuffer = dstBuffer;
        command->dstBufferOffset = dstBufferOffset;
        command->size = size;
        command->data = data;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::FillBuffer::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::UpdateBuffer(BufferMutableResource dstResource, u64 dstBufferOffset, u64 size, void* data)
    {
        DebugHandler::Assert(dstResource != BufferMutableResource::Invalid(), "CommandList : UpdateBuffer got invalid dstResource");
        _resources->EnforceHasAccess(_currentPassIndex, dstResource, BufferPassUsage::TRANSFER);

        BufferID dstBuffer = _resources->GetBuffer(dstResource);

        Commands::UpdateBuffer* command = AddCommand<Commands::UpdateBuffer>();
        command->dstBuffer = dstBuffer;
        command->dstBufferOffset = dstBufferOffset;
        command->size = size;
        command->data = data;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::UpdateBuffer::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::ImageBarrier(ImageResource resource)
    {
        ImageID imageID = _resources->GetImage(resource);

        assert(imageID != ImageID::Invalid());
        Commands::ImageBarrier* command = AddCommand<Commands::ImageBarrier>();
        command->image = imageID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::ImageBarrier::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::ImageBarrier(ImageMutableResource resource)
    {
        ImageID imageID = _resources->GetImage(resource);

        assert(imageID != ImageID::Invalid());
        Commands::ImageBarrier* command = AddCommand<Commands::ImageBarrier>();
        command->image = imageID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::ImageBarrier::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::ImageBarrier(DepthImageResource resource)
    {
        DepthImageID imageID = _resources->GetImage(resource);

        assert(imageID != DepthImageID::Invalid());
        Commands::DepthImageBarrier* command = AddCommand<Commands::DepthImageBarrier>();
        command->image = imageID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DepthImageBarrier::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::ImageBarrier(DepthImageMutableResource resource)
    {
        DepthImageID imageID = _resources->GetImage(resource);

        assert(imageID != DepthImageID::Invalid());
        Commands::DepthImageBarrier* command = AddCommand<Commands::DepthImageBarrier>();
        command->image = imageID;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DepthImageBarrier::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }

    void CommandList::BufferBarrier(BufferResource resource, BufferPassUsage from)
    {
        BufferID bufferID = _resources->GetBuffer(resource);

        BufferBarrier(bufferID, from);
    }

    void CommandList::BufferBarrier(BufferMutableResource resource, BufferPassUsage from)
    {
        BufferID bufferID = _resources->GetBuffer(resource);

        BufferBarrier(bufferID, from);
    }

    void CommandList::DrawImgui()
    {
        Commands::DrawImgui* command = AddCommand<Commands::DrawImgui>();

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::DrawImgui::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }    
    
    void CommandList::PushConstant(void* data, u32 offset, u32 size)
    {
        assert(data != nullptr);
        Commands::PushConstant* command = AddCommand<Commands::PushConstant>();
        command->data = data;
        command->offset = offset;
        command->size = size;

#if COMMANDLIST_DEBUG_IMMEDIATE_MODE
        Commands::PushConstant::DISPATCH_FUNCTION(_renderer, _immediateCommandList, command);
#endif
    }
}