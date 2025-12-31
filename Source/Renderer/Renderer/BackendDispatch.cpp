#include "BackendDispatch.h"
#include "Renderer.h"

#include "Commands/AddSignalSemaphore.h"
#include "Commands/AddWaitSemaphore.h"
#include "Commands/BeginTrace.h"
#include "Commands/BindDescriptorSet.h"
#include "Commands/BufferBarrier.h"
#include "Commands/Clear.h"
#include "Commands/CopyBuffer.h"
#include "Commands/CopyImage.h"
#include "Commands/DepthImageBarrier.h"
#include "Commands/Dispatch.h"
#include "Commands/DispatchIndirect.h"
#include "Commands/Draw.h"
#include "Commands/DrawImgui.h"
#include "Commands/DrawIndexed.h"
#include "Commands/DrawIndexedIndirect.h"
#include "Commands/DrawIndexedIndirectCount.h"
#include "Commands/DrawIndirect.h"
#include "Commands/DrawIndirectCount.h"
#include "Commands/EndTrace.h"
#include "Commands/FillBuffer.h"
#include "Commands/ImageBarrier.h"
#include "Commands/MarkFrameStart.h"
#include "Commands/PopMarker.h"
#include "Commands/PushConstant.h"
#include "Commands/PushMarker.h"
#include "Commands/RenderPass.h"
#include "Commands/SetBuffer.h"
#include "Commands/SetDepthBias.h"
#include "Commands/SetIndexBuffer.h"
#include "Commands/SetPipeline.h"
#include "Commands/SetScissorRect.h"
#include "Commands/SetVertexBuffer.h"
#include "Commands/SetViewport.h"
#include "Commands/TimeQuery.h"
#include "Commands/UpdateBuffer.h"

#include <tracy/Tracy.hpp>

namespace Renderer
{
    void BackendDispatch::ClearImageColor(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::ClearImageColor* actualData = static_cast<const Commands::ClearImageColor*>(data);
        renderer->Clear(commandList, actualData->image, actualData->color);
    }

    void BackendDispatch::ClearImageUInt(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::ClearImageUInt* actualData = static_cast<const Commands::ClearImageUInt*>(data);
        renderer->Clear(commandList, actualData->image, actualData->values);
    }

    void BackendDispatch::ClearImageInt(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::ClearImageInt* actualData = static_cast<const Commands::ClearImageInt*>(data);
        renderer->Clear(commandList, actualData->image, actualData->values);
    }

    void BackendDispatch::ClearDepthImage(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::ClearDepthImage* actualData = static_cast<const Commands::ClearDepthImage*>(data);
        renderer->Clear(commandList, actualData->image, actualData->flags, actualData->depth, actualData->stencil);
    }

    void BackendDispatch::Draw(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::Draw* actualData = static_cast<const Commands::Draw*>(data);
        renderer->Draw(commandList, actualData->vertexCount, actualData->instanceCount, actualData->vertexOffset, actualData->instanceOffset);
    }

    void BackendDispatch::DrawIndirect(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::DrawIndirect* actualData = static_cast<const Commands::DrawIndirect*>(data);
        renderer->DrawIndirect(commandList, actualData->argumentBuffer, actualData->argumentBufferOffset, actualData->drawCount);
    }

    void BackendDispatch::DrawIndirectCount(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::DrawIndirectCount* actualData = static_cast<const Commands::DrawIndirectCount*>(data);
        renderer->DrawIndirectCount(commandList, actualData->argumentBuffer, actualData->argumentBufferOffset, actualData->drawCountBuffer, actualData->drawCountBufferOffset, actualData->maxDrawCount);
    }

    void BackendDispatch::DrawIndexed(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::DrawIndexed* actualData = static_cast<const Commands::DrawIndexed*>(data);
        renderer->DrawIndexed(commandList, actualData->indexCount, actualData->instanceCount, actualData->indexOffset, actualData->vertexOffset, actualData->instanceOffset);
    }

    void BackendDispatch::DrawIndexedIndirect(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::DrawIndexedIndirect* actualData = static_cast<const Commands::DrawIndexedIndirect*>(data);
        renderer->DrawIndexedIndirect(commandList, actualData->argumentBuffer, actualData->argumentBufferOffset, actualData->drawCount);
    }

    void BackendDispatch::DrawIndexedIndirectCount(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::DrawIndexedIndirectCount* actualData = static_cast<const Commands::DrawIndexedIndirectCount*>(data);
        renderer->DrawIndexedIndirectCount(commandList, actualData->argumentBuffer, actualData->argumentBufferOffset, actualData->drawCountBuffer, actualData->drawCountBufferOffset, actualData->maxDrawCount);
    }

    void BackendDispatch::Dispatch(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::Dispatch* actualData = static_cast<const Commands::Dispatch*>(data);
        renderer->Dispatch(commandList, actualData->threadGroupCountX, actualData->threadGroupCountY, actualData->threadGroupCountZ);
    }

    void BackendDispatch::DispatchIndirect(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::DispatchIndirect* actualData = static_cast<const Commands::DispatchIndirect*>(data);
        renderer->DispatchIndirect(commandList, actualData->argumentBuffer, actualData->argumentBufferOffset);
    }

    void BackendDispatch::MarkFrameStart(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::MarkFrameStart* actualData = static_cast<const Commands::MarkFrameStart*>(data);
        renderer->MarkFrameStart(commandList, actualData->frameIndex);
    }

    void BackendDispatch::BeginTrace(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::BeginTrace* actualData = static_cast<const Commands::BeginTrace*>(data);
        renderer->BeginTrace(commandList, actualData->sourceLocation);
    }

    void BackendDispatch::EndTrace(Renderer* renderer, CommandListID commandList, const void* /*data*/)
    {
        ZoneScopedC(tracy::Color::Red3);
        renderer->EndTrace(commandList);
    }

    void BackendDispatch::PopMarker(Renderer* renderer, CommandListID commandList, const void* /*data*/)
    {
        ZoneScopedC(tracy::Color::Red3);
        renderer->PopMarker(commandList);
    }

    void BackendDispatch::PushMarker(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::PushMarker* actualData = static_cast<const Commands::PushMarker*>(data);
        renderer->PushMarker(commandList, actualData->color, actualData->marker);
    }

    void BackendDispatch::BeginRenderPass(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::BeginRenderPass* actualData = static_cast<const Commands::BeginRenderPass*>(data);
        renderer->BeginRenderPass(commandList, actualData->desc);
    }

    void BackendDispatch::EndRenderPass(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::EndRenderPass* actualData = static_cast<const Commands::EndRenderPass*>(data);
        renderer->EndRenderPass(commandList, actualData->desc);
    }

    void BackendDispatch::BeginTextureRenderPass(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::BeginTextureRenderPass* actualData = static_cast<const Commands::BeginTextureRenderPass*>(data);
        renderer->BeginRenderPass(commandList, actualData->desc);
    }

    void BackendDispatch::EndTextureRenderPass(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::BeginTextureRenderPass* actualData = static_cast<const Commands::BeginTextureRenderPass*>(data);
        renderer->EndRenderPass(commandList, actualData->desc);
    }

    void BackendDispatch::BeginTextureComputeWritePass(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::BeginTextureComputeWritePass* actualData = static_cast<const Commands::BeginTextureComputeWritePass*>(data);
        renderer->BeginTextureComputeWritePass(commandList, actualData->desc);
    }

    void BackendDispatch::EndTextureComputeWritePass(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::EndTextureComputeWritePass* actualData = static_cast<const Commands::EndTextureComputeWritePass*>(data);
        renderer->EndTextureComputeWritePass(commandList, actualData->desc);
    }

    void BackendDispatch::BeginGraphicsPipeline(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::BeginGraphicsPipeline* actualData = static_cast<const Commands::BeginGraphicsPipeline*>(data);
        renderer->BeginPipeline(commandList, actualData->pipeline);
    }

    void BackendDispatch::EndGraphicsPipeline(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::EndGraphicsPipeline* actualData = static_cast<const Commands::EndGraphicsPipeline*>(data);
        renderer->EndPipeline(commandList, actualData->pipeline);
    }

    void BackendDispatch::BeginComputePipeline(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::BeginComputePipeline* actualData = static_cast<const Commands::BeginComputePipeline*>(data);
        renderer->BeginPipeline(commandList, actualData->pipeline);
    }

    void BackendDispatch::EndComputePipeline(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::EndComputePipeline* actualData = static_cast<const Commands::EndComputePipeline*>(data);
        renderer->EndPipeline(commandList, actualData->pipeline);
    }

    void BackendDispatch::BeginTimeQuery(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::BeginTimeQuery* actualData = static_cast<const Commands::BeginTimeQuery*>(data);
        renderer->BeginTimeQuery(commandList, actualData->timeQueryID);
    }

    void BackendDispatch::EndTimeQuery(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::EndTimeQuery* actualData = static_cast<const Commands::EndTimeQuery*>(data);
        renderer->EndTimeQuery(commandList, actualData->timeQueryID);
    }

    void BackendDispatch::BindDescriptorSet(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::BindDescriptorSet* actualData = static_cast<const Commands::BindDescriptorSet*>(data);
        renderer->BindDescriptorSet(commandList, actualData->set, actualData->bufferPermissions);
    }

    void BackendDispatch::SetDepthBias(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::SetDepthBias* actualData = static_cast<const Commands::SetDepthBias*>(data);
        renderer->SetDepthBias(commandList, actualData->depthBias);
    }

    void BackendDispatch::SetScissorRect(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::SetScissorRect* actualData = static_cast<const Commands::SetScissorRect*>(data);
        renderer->SetScissorRect(commandList, actualData->scissorRect);
    }

    void BackendDispatch::SetViewport(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::SetViewport* actualData = static_cast<const Commands::SetViewport*>(data);
        renderer->SetViewport(commandList, actualData->viewport);
    }

    void BackendDispatch::SetVertexBuffer(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::SetVertexBuffer* actualData = static_cast<const Commands::SetVertexBuffer*>(data);
        renderer->SetVertexBuffer(commandList, actualData->slot, actualData->bufferID);
    }

    void BackendDispatch::SetIndexBuffer(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::SetIndexBuffer* actualData = static_cast<const Commands::SetIndexBuffer*>(data);
        renderer->SetIndexBuffer(commandList, actualData->bufferID, actualData->indexFormat);
    }

    void BackendDispatch::SetBuffer(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::SetBuffer* actualData = static_cast<const Commands::SetBuffer*>(data);
        renderer->SetBuffer(commandList, actualData->slot, actualData->buffer);
    }

    void BackendDispatch::AddSignalSemaphore(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::AddSignalSemaphore* actualData = static_cast<const Commands::AddSignalSemaphore*>(data);
        renderer->AddSignalSemaphore(commandList, actualData->semaphore);
    }

    void BackendDispatch::AddWaitSemaphore(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::AddWaitSemaphore* actualData = static_cast<const Commands::AddWaitSemaphore*>(data);
        renderer->AddWaitSemaphore(commandList, actualData->semaphore);
    }

    void BackendDispatch::CopyImage(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::CopyImage* actualData = static_cast<const Commands::CopyImage*>(data);
        renderer->CopyImage(commandList, actualData->dstImage, actualData->dstPos, actualData->dstMipLevel, actualData->srcImage, actualData->srcPos, actualData->srcMipLevel, actualData->size);
    }

    void BackendDispatch::CopyDepthImage(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::CopyDepthImage* actualData = static_cast<const Commands::CopyDepthImage*>(data);
        renderer->CopyImage(commandList, actualData->dstImage, actualData->dstPos, actualData->srcImage, actualData->srcPos, actualData->size);
    }

    void BackendDispatch::CopyBuffer(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::CopyBuffer* actualData = static_cast<const Commands::CopyBuffer*>(data);
        renderer->CopyBuffer(commandList, actualData->dstBuffer, actualData->dstBufferOffset, actualData->srcBuffer, actualData->srcBufferOffset, actualData->region);
    }

    void BackendDispatch::FillBuffer(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::FillBuffer* actualData = static_cast<const Commands::FillBuffer*>(data);
        renderer->FillBuffer(commandList, actualData->dstBuffer, actualData->dstBufferOffset, actualData->size, actualData->data);
    }

    void BackendDispatch::UpdateBuffer(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::UpdateBuffer* actualData = static_cast<const Commands::UpdateBuffer*>(data);
        renderer->UpdateBuffer(commandList, actualData->dstBuffer, actualData->dstBufferOffset, actualData->size, actualData->data);
    }

    void BackendDispatch::ImageBarrier(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::ImageBarrier* actualData = static_cast<const Commands::ImageBarrier*>(data);
        renderer->ImageBarrier(commandList, actualData->image);
    }

    void BackendDispatch::DepthImageBarrier(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::DepthImageBarrier* actualData = static_cast<const Commands::DepthImageBarrier*>(data);
        renderer->ImageBarrier(commandList, actualData->image);
    }

    void BackendDispatch::BufferBarrier(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::BufferBarrier* actualData = static_cast<const Commands::BufferBarrier*>(data);
        renderer->BufferBarrier(commandList, actualData->bufferID, actualData->from);
    }

    void BackendDispatch::UploadBufferBarrier(Renderer* renderer, CommandListID commandList, const void*)
    {
        ZoneScopedC(tracy::Color::Red3);
        renderer->UploadBufferBarrier(commandList);
    }

    void BackendDispatch::DrawImgui(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);

        renderer->DrawImgui(commandList);
    }

    void BackendDispatch::PushConstant(Renderer* renderer, CommandListID commandList, const void* data)
    {
        ZoneScopedC(tracy::Color::Red3);
        const Commands::PushConstant* actualData = static_cast<const Commands::PushConstant*>(data);
        renderer->PushConstant(commandList, actualData->data, actualData->offset, actualData->size);
    }
}