#include "Clear.h"
#include "Draw.h"
#include "DrawIndirect.h"
#include "DrawIndirectCount.h"
#include "DrawIndexed.h"
#include "DrawIndexedIndirect.h"
#include "DrawIndexedIndirectCount.h"
#include "Dispatch.h"
#include "DispatchIndirect.h"
#include "PopMarker.h"
#include "PushMarker.h"
#include "SetPipeline.h"
#include "SetDepthBias.h"
#include "SetScissorRect.h"
#include "SetViewport.h"
#include "SetVertexBuffer.h"
#include "SetIndexBuffer.h"
#include "SetBuffer.h"
#include "BindDescriptorSet.h"
#include "MarkFrameStart.h"
#include "BeginTrace.h"
#include "EndTrace.h"
#include "AddSignalSemaphore.h"
#include "AddWaitSemaphore.h"
#include "CopyImage.h"
#include "CopyBuffer.h"
#include "FillBuffer.h"
#include "UpdateBuffer.h"
#include "ImageBarrier.h"
#include "BufferBarrier.h"
#include "DepthImageBarrier.h"
#include "DrawImgui.h"
#include "PushConstant.h"
#include "TimeQuery.h"

#include "Renderer/BackendDispatch.h"

namespace Renderer
{
    namespace Commands
    {
        const BackendDispatchFunction ClearImageColor::DISPATCH_FUNCTION = &BackendDispatch::ClearImageColor;
        const BackendDispatchFunction ClearImageUInt::DISPATCH_FUNCTION = &BackendDispatch::ClearImageUInt;
        const BackendDispatchFunction ClearImageInt::DISPATCH_FUNCTION = &BackendDispatch::ClearImageInt;
        const BackendDispatchFunction ClearDepthImage::DISPATCH_FUNCTION = &BackendDispatch::ClearDepthImage;
        const BackendDispatchFunction Draw::DISPATCH_FUNCTION = &BackendDispatch::Draw;
        const BackendDispatchFunction DrawIndirect::DISPATCH_FUNCTION = &BackendDispatch::DrawIndirect;
        const BackendDispatchFunction DrawIndirectCount::DISPATCH_FUNCTION = &BackendDispatch::DrawIndirectCount;
        const BackendDispatchFunction DrawIndexed::DISPATCH_FUNCTION = &BackendDispatch::DrawIndexed;
        const BackendDispatchFunction DrawIndexedIndirect::DISPATCH_FUNCTION = &BackendDispatch::DrawIndexedIndirect;
        const BackendDispatchFunction DrawIndexedIndirectCount::DISPATCH_FUNCTION = &BackendDispatch::DrawIndexedIndirectCount;
        const BackendDispatchFunction Dispatch::DISPATCH_FUNCTION = &BackendDispatch::Dispatch;
        const BackendDispatchFunction DispatchIndirect::DISPATCH_FUNCTION = &BackendDispatch::DispatchIndirect;
        const BackendDispatchFunction PopMarker::DISPATCH_FUNCTION = &BackendDispatch::PopMarker;
        const BackendDispatchFunction PushMarker::DISPATCH_FUNCTION = &BackendDispatch::PushMarker;
        const BackendDispatchFunction BeginGraphicsPipeline::DISPATCH_FUNCTION = &BackendDispatch::BeginGraphicsPipeline;
        const BackendDispatchFunction EndGraphicsPipeline::DISPATCH_FUNCTION = &BackendDispatch::EndGraphicsPipeline;
        const BackendDispatchFunction BeginComputePipeline::DISPATCH_FUNCTION = &BackendDispatch::BeginComputePipeline;
        const BackendDispatchFunction EndComputePipeline::DISPATCH_FUNCTION = &BackendDispatch::EndComputePipeline;
        const BackendDispatchFunction BeginTimeQuery::DISPATCH_FUNCTION = &BackendDispatch::BeginTimeQuery;
        const BackendDispatchFunction EndTimeQuery::DISPATCH_FUNCTION = &BackendDispatch::EndTimeQuery;
        const BackendDispatchFunction SetDepthBias::DISPATCH_FUNCTION = &BackendDispatch::SetDepthBias;
        const BackendDispatchFunction SetScissorRect::DISPATCH_FUNCTION = &BackendDispatch::SetScissorRect;
        const BackendDispatchFunction SetViewport::DISPATCH_FUNCTION = &BackendDispatch::SetViewport;
        const BackendDispatchFunction SetVertexBuffer::DISPATCH_FUNCTION = &BackendDispatch::SetVertexBuffer;
        const BackendDispatchFunction SetIndexBuffer::DISPATCH_FUNCTION = &BackendDispatch::SetIndexBuffer;
        const BackendDispatchFunction SetBuffer::DISPATCH_FUNCTION = &BackendDispatch::SetBuffer;
        const BackendDispatchFunction BindDescriptorSet::DISPATCH_FUNCTION = &BackendDispatch::BindDescriptorSet;
        const BackendDispatchFunction MarkFrameStart::DISPATCH_FUNCTION = &BackendDispatch::MarkFrameStart;
        const BackendDispatchFunction BeginTrace::DISPATCH_FUNCTION = &BackendDispatch::BeginTrace;
        const BackendDispatchFunction EndTrace::DISPATCH_FUNCTION = &BackendDispatch::EndTrace;
        const BackendDispatchFunction AddSignalSemaphore::DISPATCH_FUNCTION = &BackendDispatch::AddSignalSemaphore;
        const BackendDispatchFunction AddWaitSemaphore::DISPATCH_FUNCTION = &BackendDispatch::AddWaitSemaphore;
        const BackendDispatchFunction CopyImage::DISPATCH_FUNCTION = &BackendDispatch::CopyImage;
        const BackendDispatchFunction CopyDepthImage::DISPATCH_FUNCTION = &BackendDispatch::CopyDepthImage;
        const BackendDispatchFunction CopyBuffer::DISPATCH_FUNCTION = &BackendDispatch::CopyBuffer;
        const BackendDispatchFunction FillBuffer::DISPATCH_FUNCTION = &BackendDispatch::FillBuffer;
        const BackendDispatchFunction UpdateBuffer::DISPATCH_FUNCTION = &BackendDispatch::UpdateBuffer;
        const BackendDispatchFunction ImageBarrier::DISPATCH_FUNCTION = &BackendDispatch::ImageBarrier;
        const BackendDispatchFunction DepthImageBarrier::DISPATCH_FUNCTION = &BackendDispatch::DepthImageBarrier;
        const BackendDispatchFunction BufferBarrier::DISPATCH_FUNCTION = &BackendDispatch::BufferBarrier;
        const BackendDispatchFunction DrawImgui::DISPATCH_FUNCTION = &BackendDispatch::DrawImgui;
        const BackendDispatchFunction PushConstant::DISPATCH_FUNCTION = &BackendDispatch::PushConstant;
    }
}
