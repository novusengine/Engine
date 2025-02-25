#include "AddSignalSemaphore.h"
#include "AddWaitSemaphore.h"
#include "BeginTrace.h"
#include "BindDescriptorSet.h"
#include "BufferBarrier.h"
#include "Clear.h"
#include "CopyBuffer.h"
#include "CopyImage.h"
#include "DepthImageBarrier.h"
#include "Dispatch.h"
#include "DispatchIndirect.h"
#include "Draw.h"
#include "DrawImgui.h"
#include "DrawIndexed.h"
#include "DrawIndexedIndirect.h"
#include "DrawIndexedIndirectCount.h"
#include "DrawIndirect.h"
#include "DrawIndirectCount.h"
#include "EndTrace.h"
#include "FillBuffer.h"
#include "ImageBarrier.h"
#include "MarkFrameStart.h"
#include "PopMarker.h"
#include "PushConstant.h"
#include "PushMarker.h"
#include "RenderPass.h"
#include "SetBuffer.h"
#include "SetDepthBias.h"
#include "SetIndexBuffer.h"
#include "SetPipeline.h"
#include "SetScissorRect.h"
#include "SetVertexBuffer.h"
#include "SetViewport.h"
#include "TimeQuery.h"
#include "UpdateBuffer.h"
#include "UploadBufferBarrier.h"

#include "Renderer/BackendDispatch.h"

namespace Renderer
{
    namespace Commands
    {
        const BackendDispatchFunction AddSignalSemaphore::DISPATCH_FUNCTION = &BackendDispatch::AddSignalSemaphore;
        const BackendDispatchFunction AddWaitSemaphore::DISPATCH_FUNCTION = &BackendDispatch::AddWaitSemaphore;
        const BackendDispatchFunction BeginRenderPass::DISPATCH_FUNCTION = &BackendDispatch::BeginRenderPass;
        const BackendDispatchFunction BeginComputePipeline::DISPATCH_FUNCTION = &BackendDispatch::BeginComputePipeline;
        const BackendDispatchFunction BeginGraphicsPipeline::DISPATCH_FUNCTION = &BackendDispatch::BeginGraphicsPipeline;
        const BackendDispatchFunction BeginTimeQuery::DISPATCH_FUNCTION = &BackendDispatch::BeginTimeQuery;
        const BackendDispatchFunction BeginTrace::DISPATCH_FUNCTION = &BackendDispatch::BeginTrace;
        const BackendDispatchFunction BindDescriptorSet::DISPATCH_FUNCTION = &BackendDispatch::BindDescriptorSet;
        const BackendDispatchFunction BufferBarrier::DISPATCH_FUNCTION = &BackendDispatch::BufferBarrier;
        const BackendDispatchFunction ClearDepthImage::DISPATCH_FUNCTION = &BackendDispatch::ClearDepthImage;
        const BackendDispatchFunction ClearImageColor::DISPATCH_FUNCTION = &BackendDispatch::ClearImageColor;
        const BackendDispatchFunction ClearImageInt::DISPATCH_FUNCTION = &BackendDispatch::ClearImageInt;
        const BackendDispatchFunction ClearImageUInt::DISPATCH_FUNCTION = &BackendDispatch::ClearImageUInt;
        const BackendDispatchFunction CopyBuffer::DISPATCH_FUNCTION = &BackendDispatch::CopyBuffer;
        const BackendDispatchFunction CopyDepthImage::DISPATCH_FUNCTION = &BackendDispatch::CopyDepthImage;
        const BackendDispatchFunction CopyImage::DISPATCH_FUNCTION = &BackendDispatch::CopyImage;
        const BackendDispatchFunction DepthImageBarrier::DISPATCH_FUNCTION = &BackendDispatch::DepthImageBarrier;
        const BackendDispatchFunction Dispatch::DISPATCH_FUNCTION = &BackendDispatch::Dispatch;
        const BackendDispatchFunction DispatchIndirect::DISPATCH_FUNCTION = &BackendDispatch::DispatchIndirect;
        const BackendDispatchFunction Draw::DISPATCH_FUNCTION = &BackendDispatch::Draw;
        const BackendDispatchFunction DrawImgui::DISPATCH_FUNCTION = &BackendDispatch::DrawImgui;
        const BackendDispatchFunction DrawIndexed::DISPATCH_FUNCTION = &BackendDispatch::DrawIndexed;
        const BackendDispatchFunction DrawIndexedIndirect::DISPATCH_FUNCTION = &BackendDispatch::DrawIndexedIndirect;
        const BackendDispatchFunction DrawIndexedIndirectCount::DISPATCH_FUNCTION = &BackendDispatch::DrawIndexedIndirectCount;
        const BackendDispatchFunction DrawIndirect::DISPATCH_FUNCTION = &BackendDispatch::DrawIndirect;
        const BackendDispatchFunction DrawIndirectCount::DISPATCH_FUNCTION = &BackendDispatch::DrawIndirectCount;
        const BackendDispatchFunction EndRenderPass::DISPATCH_FUNCTION = &BackendDispatch::EndRenderPass;
        const BackendDispatchFunction EndComputePipeline::DISPATCH_FUNCTION = &BackendDispatch::EndComputePipeline;
        const BackendDispatchFunction EndGraphicsPipeline::DISPATCH_FUNCTION = &BackendDispatch::EndGraphicsPipeline;
        const BackendDispatchFunction EndTimeQuery::DISPATCH_FUNCTION = &BackendDispatch::EndTimeQuery;
        const BackendDispatchFunction EndTrace::DISPATCH_FUNCTION = &BackendDispatch::EndTrace;
        const BackendDispatchFunction FillBuffer::DISPATCH_FUNCTION = &BackendDispatch::FillBuffer;
        const BackendDispatchFunction ImageBarrier::DISPATCH_FUNCTION = &BackendDispatch::ImageBarrier;
        const BackendDispatchFunction MarkFrameStart::DISPATCH_FUNCTION = &BackendDispatch::MarkFrameStart;
        const BackendDispatchFunction PopMarker::DISPATCH_FUNCTION = &BackendDispatch::PopMarker;
        const BackendDispatchFunction PushConstant::DISPATCH_FUNCTION = &BackendDispatch::PushConstant;
        const BackendDispatchFunction PushMarker::DISPATCH_FUNCTION = &BackendDispatch::PushMarker;
        const BackendDispatchFunction SetBuffer::DISPATCH_FUNCTION = &BackendDispatch::SetBuffer;
        const BackendDispatchFunction SetDepthBias::DISPATCH_FUNCTION = &BackendDispatch::SetDepthBias;
        const BackendDispatchFunction SetIndexBuffer::DISPATCH_FUNCTION = &BackendDispatch::SetIndexBuffer;
        const BackendDispatchFunction SetScissorRect::DISPATCH_FUNCTION = &BackendDispatch::SetScissorRect;
        const BackendDispatchFunction SetVertexBuffer::DISPATCH_FUNCTION = &BackendDispatch::SetVertexBuffer;
        const BackendDispatchFunction SetViewport::DISPATCH_FUNCTION = &BackendDispatch::SetViewport;
        const BackendDispatchFunction UpdateBuffer::DISPATCH_FUNCTION = &BackendDispatch::UpdateBuffer;
        const BackendDispatchFunction UploadBufferBarrier::DISPATCH_FUNCTION = &BackendDispatch::UploadBufferBarrier;
    }
}
