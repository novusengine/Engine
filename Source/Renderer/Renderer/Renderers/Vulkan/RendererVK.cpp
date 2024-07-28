#include "RendererVK.h"
#include "Renderer/Window.h"

#include "Backend/RenderDeviceVK.h"
#include "Backend/BufferHandlerVK.h"
#include "Backend/ImageHandlerVK.h"
#include "Backend/TextureHandlerVK.h"
#include "Backend/ShaderHandlerVK.h"
#include "Backend/PipelineHandlerVK.h"
#include "Backend/CommandListHandlerVK.h"
#include "Backend/SamplerHandlerVK.h"
#include "Backend/SemaphoreHandlerVK.h"
#include "Backend/UploadBufferHandlerVK.h"
#include "Backend/SwapChainVK.h"
#include "Backend/DebugMarkerUtilVK.h"
#include "Backend/DescriptorSetBuilderVK.h"
#include "Backend/FormatConverterVK.h"
#include "Backend/DebugMarkerUtilVK.h"
#include "Backend/TimeQueryHandlerVK.h"
#include "Backend/ffx_vk.h"

#include <Base/Container/SafeVector.h>
#include <Base/Util/StringUtils.h>
#include <Base/Util/DebugHandler.h>
#include <Base/Util/Timer.h>

#include <tracy/Tracy.hpp>
#include <tracy/TracyVulkan.hpp>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <FidelityFX/host/ffx_cacao.h>
#include <FidelityFX/host/ffx_interface.h>

namespace Renderer
{
    RendererVK::RendererVK(Novus::Window* window)
        : _device(new Backend::RenderDeviceVK(window))
    {
        _frameAllocator.Init(4 * 1024 * 1024, "Renderer Allocator");

        // Create handlers
        _bufferHandler = new Backend::BufferHandlerVK();
        _imageHandler = new Backend::ImageHandlerVK();
        _textureHandler = new Backend::TextureHandlerVK();
        _shaderHandler = new Backend::ShaderHandlerVK();
        _pipelineHandler = new Backend::PipelineHandlerVK();
        _commandListHandler = new Backend::CommandListHandlerVK();
        _samplerHandler = new Backend::SamplerHandlerVK();
        _semaphoreHandler = new Backend::SemaphoreHandlerVK();
        _uploadBufferHandler = new Backend::UploadBufferHandlerVK();
        _timeQueryHandler = new Backend::TimeQueryHandlerVK();
        _fidelityFXHandler = new Backend::FidelityFXHandlerVK();

        // Init
        _device->Init();
        _bufferHandler->Init(_device);
        _imageHandler->Init(_device, _samplerHandler);
        _textureHandler->Init(_device, _bufferHandler, _uploadBufferHandler, _samplerHandler);
        _shaderHandler->Init(_device);
        _timeQueryHandler->Init(_device);
        _fidelityFXHandler->Init(_device, _imageHandler);
        _pipelineHandler->Init(&_frameAllocator, _device, _shaderHandler, _imageHandler, _bufferHandler);
        _commandListHandler->Init(_device);
        _samplerHandler->Init(_device);
        _semaphoreHandler->Init(_device);
        _uploadBufferHandler->Init(this, _device, _bufferHandler, _textureHandler, _semaphoreHandler, _commandListHandler);
    }

    void RendererVK::InitDebug()
    {
        _imageHandler->PostInit();
        _textureHandler->InitDebugTexture();

        CreateDummyPipeline();
    }

    void RendererVK::InitWindow(Novus::Window* window)
    {
        _device->InitWindow(_imageHandler, _semaphoreHandler, window);
    }

    void RendererVK::Deinit()
    {
        _device->FlushGPU(); // Make sure it has finished rendering

        delete(_device);
        delete(_bufferHandler);
        delete(_imageHandler);
        delete(_textureHandler);
        delete(_shaderHandler);
        delete(_pipelineHandler);
        delete(_commandListHandler);
        delete(_samplerHandler);
        delete(_semaphoreHandler);
        delete(_timeQueryHandler);
        delete(_fidelityFXHandler);
    }

    void RendererVK::SetShaderSourceDirectory(const std::string& path)
    {
        _shaderHandler->SetShaderSourceDirectory(path);
    }

    void RendererVK::ReloadShaders(bool forceRecompileAll)
    {
        // Make sure the device is not rendering
        vkDeviceWaitIdle(_device->_device);

        _shaderHandler->ReloadShaders(forceRecompileAll);
        _pipelineHandler->DiscardPipelines();

        CreateDummyPipeline();
    }

    void RendererVK::ClearUploadBuffers()
    {
        _uploadBufferHandler->Clear();
    }

    void RendererVK::SetRenderSize(vec2 renderSize)
    {
        _renderSize = renderSize;
        _renderSizeChanged = true;
    }

    vec2 RendererVK::GetRenderSize()
    {
        return _renderSize;
    }

    vec2 RendererVK::GetWindowSize()
    {
        return _device->GetMainWindowSize();
    }

    BufferID RendererVK::CreateBuffer(BufferDesc& desc)
    {
        return _bufferHandler->CreateBuffer(desc);
    }

    BufferID RendererVK::CreateTemporaryBuffer(BufferDesc& desc, u32 framesLifetime)
    {
        return _bufferHandler->CreateTemporaryBuffer(desc, framesLifetime);
    }

    void RendererVK::QueueDestroyBuffer(BufferID buffer)
    {
        _uploadBufferHandler->QueueDestroyBuffer(buffer);
    }

    void RendererVK::DestroyBuffer(BufferID buffer)
    {
        std::scoped_lock lock(_destroyListMutex);
        _destroyLists[_destroyListIndex].buffers.push_back(buffer);
    }

    ImageID RendererVK::CreateImage(ImageDesc& desc)
    {
        return _imageHandler->CreateImage(desc);
    }

    DepthImageID RendererVK::CreateDepthImage(DepthImageDesc& desc)
    {
        return _imageHandler->CreateDepthImage(desc);
    }

    SamplerID RendererVK::CreateSampler(SamplerDesc& desc)
    {
        return _samplerHandler->CreateSampler(desc);
    }

    SemaphoreID RendererVK::CreateNSemaphore()
    {
        return _semaphoreHandler->CreateNSemaphore();
    }

    GraphicsPipelineID RendererVK::CreatePipeline(GraphicsPipelineDesc& desc)
    {
#if _DEBUG
        if (!_isExecutingCommandlist)
        {
            NC_LOG_CRITICAL("Please only create pipelines from inside a Commandlist");
        }
#endif // _DEBUG

        return _pipelineHandler->CreatePipeline(desc);
    }

    ComputePipelineID RendererVK::CreatePipeline(ComputePipelineDesc& desc)
    {
#if _DEBUG
        if (!_isExecutingCommandlist)
        {
            NC_LOG_CRITICAL("Please only create pipelines from inside a Commandlist");
        }
#endif // _DEBUG

        return _pipelineHandler->CreatePipeline(desc);
    }

    TextureArrayID RendererVK::CreateTextureArray(TextureArrayDesc& desc)
    {
        return _textureHandler->CreateTextureArray(desc);
    }

    TextureID RendererVK::CreateDataTexture(DataTextureDesc& desc)
    {
        return _textureHandler->CreateDataTexture(desc);
    }

    TextureID RendererVK::CreateDataTextureIntoArray(DataTextureDesc& desc, TextureArrayID textureArray, u32& arrayIndex)
    {
        return _textureHandler->CreateDataTextureIntoArray(desc, textureArray, arrayIndex);
    }

    TimeQueryID RendererVK::CreateTimeQuery(TimeQueryDesc& desc)
    {
        return _timeQueryHandler->CreateTimeQuery(desc);
    }

    TextureID RendererVK::LoadTexture(TextureDesc& desc)
    {
        return _textureHandler->LoadTexture(desc);
    }

    TextureID RendererVK::LoadTextureIntoArray(TextureDesc& desc, TextureArrayID textureArray, u32& arrayIndex, bool allowDuplicates)
    {
        return _textureHandler->LoadTextureIntoArray(desc, textureArray, arrayIndex, allowDuplicates);
    }

    VertexShaderID RendererVK::LoadShader(VertexShaderDesc& desc)
    {
        return _shaderHandler->LoadShader(desc);
    }

    PixelShaderID RendererVK::LoadShader(PixelShaderDesc& desc)
    {
        return _shaderHandler->LoadShader(desc);
    }

    ComputeShaderID RendererVK::LoadShader(ComputeShaderDesc& desc)
    {
        return _shaderHandler->LoadShader(desc);
    }

    void RendererVK::UnloadTexture(TextureID textureID)
    {
        _device->FlushGPU(); // Make sure we have finished rendering

        _textureHandler->UnloadTexture(textureID);
    }

    void RendererVK::UnloadTexturesInArray(TextureArrayID textureArrayID, u32 unloadStartIndex)
    {
        _device->FlushGPU(); // Make sure we have finished rendering

        _textureHandler->UnloadTexturesInArray(textureArrayID, unloadStartIndex);
    }

    u32 RendererVK::AddTextureToArray(TextureID textureID, TextureArrayID textureArrayID)
    {
        return _textureHandler->AddTextureToArray(textureID, textureArrayID);
    }

    static VmaBudget sBudgets[16] = { { 0 } };

    void RendererVK::FlushGPU()
    {
        _device->FlushGPU();
    }

    f32 RendererVK::FlipFrame(u32 frameIndex)
    {
        ZoneScopedC(tracy::Color::Red3);

        // Reset old commandbuffers
        _commandListHandler->FlipFrame();

        // Wait on frame fence
        f32 timeWaited = 0.0f;
        {
            Timer timer;

            ZoneScopedNC("Wait For Fence", tracy::Color::Red3);

            VkFence frameFence = _commandListHandler->GetCurrentFence();

            u64 timeout = 5000000000; // 5 seconds in nanoseconds
            VkResult result = vkWaitForFences(_device->_device, 1, &frameFence, true, timeout);

            if (result == VK_TIMEOUT)
            {
                NC_LOG_CRITICAL("Waiting for frame fence took longer than 5 seconds, something is wrong!");
            }

            vkResetFences(_device->_device, 1, &frameFence);
            timeWaited = timer.GetLifeTime();
        }

        if (_renderSizeChanged)
        {
            _device->FlushGPU();

            _device->SetRenderSize(_renderSize);

            _pipelineHandler->DiscardPipelines();
            CreateDummyPipeline();

            _imageHandler->OnResize(false);

            _renderSizeChanged = false;
        }

        _textureHandler->FlipFrame(frameIndex);
        _timeQueryHandler->FlipFrame(frameIndex);
        _commandListHandler->ResetCommandBuffers();
        _uploadBufferHandler->ExecuteUploadTasks();
        _bufferHandler->OnFrameStart();

        vmaSetCurrentFrameIndex(_device->_allocator, frameIndex);
        vmaGetBudget(_device->_allocator, sBudgets);

        _frameAllocator.Reset();

        return timeWaited;
    }

    void RendererVK::ResetTimeQueries(u32 frameIndex)
    {
        _frameTimeQueries.clear();
    }

    TextureID RendererVK::GetTextureID(TextureArrayID textureArrayID, u32 index)
    {
        return _textureHandler->GetTextureIDInArray(textureArrayID, index);
    }

    i32 RendererVK::GetTextureHeight(TextureID textureID)
    {
        return _textureHandler->GetTextureHeight(textureID);
    }

    i32 RendererVK::GetTextureWidth(TextureID textureID)
    {
        return _textureHandler->GetTextureWidth(textureID);
    }

    const ImageDesc& RendererVK::GetImageDesc(ImageID ID)
    {
        return _imageHandler->GetImageDesc(ID);
    }

    const DepthImageDesc& RendererVK::GetImageDesc(DepthImageID ID)
    {
        return _imageHandler->GetImageDesc(ID);
    }

    uvec2 RendererVK::GetImageDimensions(const ImageID id, u32 mipLevel)
    {
        return _imageHandler->GetDimensions(id, mipLevel);
    }

    uvec2 RendererVK::GetImageDimensions(const DepthImageID id)
    {
        return _imageHandler->GetDimensions(id);
    }

    std::string RendererVK::GetBufferName(BufferID id)
    {
        return _bufferHandler->GetBufferName(id);
    }

    CommandListID RendererVK::BeginCommandList()
    {
        return _commandListHandler->BeginCommandList(Backend::QueueType::Graphics);
    }

    void RendererVK::EndCommandList(CommandListID commandListID)
    {
        _commandListHandler->EndCommandList(commandListID, VK_NULL_HANDLE);
    }

    void RendererVK::Clear(CommandListID commandListID, ImageID imageID, Color color)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkImage image = _imageHandler->GetImage(imageID);

        ImageDesc desc = _imageHandler->GetImageDesc(imageID);
        
        VkClearColorValue clearColorValue = {};
        clearColorValue.float32[0] = color.r;
        clearColorValue.float32[1] = color.g;
        clearColorValue.float32[2] = color.b;
        clearColorValue.float32[3] = color.a;

        VkImageSubresourceRange imageSubresourceRange;
        imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageSubresourceRange.baseMipLevel = 0;
        imageSubresourceRange.levelCount = 1;
        imageSubresourceRange.baseArrayLayer = 0;
        imageSubresourceRange.layerCount = desc.depth;

        // Transition image to TRANSFER_DST_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, desc.depth, 1);

        vkCmdClearColorImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColorValue, 1, &imageSubresourceRange);

        // Transition image back to GENERAL
        _device->TransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, desc.depth, 1);
    }

    void RendererVK::Clear(CommandListID commandListID, ImageID imageID, uvec4 values)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkImage image = _imageHandler->GetImage(imageID);

        ImageDesc desc = _imageHandler->GetImageDesc(imageID);

        VkClearColorValue clearColorValue = {};
        clearColorValue.uint32[0] = values.x;
        clearColorValue.uint32[1] = values.y;
        clearColorValue.uint32[2] = values.z;
        clearColorValue.uint32[3] = values.w;

        VkImageSubresourceRange imageSubresourceRange;
        imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageSubresourceRange.baseMipLevel = 0;
        imageSubresourceRange.levelCount = 1;
        imageSubresourceRange.baseArrayLayer = 0;
        imageSubresourceRange.layerCount = desc.depth;

        // Transition image to TRANSFER_DST_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, desc.depth, 1);

        vkCmdClearColorImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColorValue, 1, &imageSubresourceRange);

        // Transition image back to GENERAL
        _device->TransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, desc.depth, 1);
    }

    void RendererVK::Clear(CommandListID commandListID, ImageID imageID, ivec4 values)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkImage image = _imageHandler->GetImage(imageID);

        ImageDesc desc = _imageHandler->GetImageDesc(imageID);

        VkClearColorValue clearColorValue = {};
        clearColorValue.int32[0] = values.x;
        clearColorValue.int32[1] = values.y;
        clearColorValue.int32[2] = values.z;
        clearColorValue.int32[3] = values.w;

        VkImageSubresourceRange imageSubresourceRange;
        imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageSubresourceRange.baseMipLevel = 0;
        imageSubresourceRange.levelCount = 1;
        imageSubresourceRange.baseArrayLayer = 0;
        imageSubresourceRange.layerCount = desc.depth;

        // Transition image to TRANSFER_DST_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, desc.depth, 1);

        vkCmdClearColorImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColorValue, 1, &imageSubresourceRange);

        // Transition image back to GENERAL
        _device->TransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, desc.depth, 1);
    }

    void RendererVK::Clear(CommandListID commandListID, DepthImageID imageID, DepthClearFlags clearFlags, f32 depth, u8 stencil)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkImage image = _imageHandler->GetImage(imageID);

        VkClearDepthStencilValue clearDepthValue = {};
        VkImageSubresourceRange range = {};

        if (clearFlags == DepthClearFlags::DEPTH || clearFlags == DepthClearFlags::BOTH)
        {
            clearDepthValue.depth = depth;
            range.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }

        if (clearFlags == DepthClearFlags::STENCIL || clearFlags == DepthClearFlags::BOTH)
        {
            clearDepthValue.stencil = stencil;
            range.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        range.layerCount = 1;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.baseMipLevel = 0;

        // Transition image to TRANSFER_DST_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, image, range.aspectMask, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1);

        vkCmdClearDepthStencilImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearDepthValue, 1, &range);

        // Transition image back to DEPTH_STENCIL_READ_ONLY_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, image, range.aspectMask, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 1, 1);
    }

    void RendererVK::Draw(CommandListID commandListID, u32 numVertices, u32 numInstances, u32 vertexOffset, u32 instanceOffset)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_commandListHandler->GetRenderPassOpenCount(commandListID) <= 0)
        {
            NC_LOG_CRITICAL("You tried to draw without first calling BeginPipeline!");
        }

        vkCmdDraw(commandBuffer, numVertices, numInstances, vertexOffset, instanceOffset);
    }

    void RendererVK::DrawIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, u32 drawCount)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_commandListHandler->GetRenderPassOpenCount(commandListID) <= 0)
        {
            NC_LOG_CRITICAL("You tried to draw without first calling BeginPipeline!");
        }

        VkBuffer vkArgumentBuffer = _bufferHandler->GetBuffer(argumentBuffer);

        vkCmdDrawIndirect(commandBuffer, vkArgumentBuffer, argumentBufferOffset, drawCount, sizeof(VkDrawIndirectCommand));
    }

    void RendererVK::DrawIndirectCount(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, BufferID drawCountBuffer, u32 drawCountBufferOffset, u32 maxDrawCount)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_commandListHandler->GetRenderPassOpenCount(commandListID) <= 0)
        {
            NC_LOG_CRITICAL("You tried to draw without first calling BeginPipeline!");
        }

        VkBuffer vkArgumentBuffer = _bufferHandler->GetBuffer(argumentBuffer);
        VkBuffer vkDrawCountBuffer = _bufferHandler->GetBuffer(drawCountBuffer);

        Backend::RenderDeviceVK::fnVkCmdDrawIndirectCountKHR(commandBuffer, vkArgumentBuffer, argumentBufferOffset, vkDrawCountBuffer, drawCountBufferOffset, maxDrawCount, sizeof(VkDrawIndirectCommand));
    }

    void RendererVK::DrawIndexed(CommandListID commandListID, u32 numIndices, u32 numInstances, u32 indexOffset, u32 vertexOffset, u32 instanceOffset)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_commandListHandler->GetRenderPassOpenCount(commandListID) <= 0)
        {
            NC_LOG_CRITICAL("You tried to draw without first calling BeginPipeline!");
        }

        vkCmdDrawIndexed(commandBuffer, numIndices, numInstances, indexOffset, vertexOffset, instanceOffset);
    }

    void RendererVK::DrawIndexedIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, u32 drawCount)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_commandListHandler->GetRenderPassOpenCount(commandListID) <= 0)
        {
            NC_LOG_CRITICAL("You tried to draw without first calling BeginPipeline!");
        }

        VkBuffer vkArgumentBuffer = _bufferHandler->GetBuffer(argumentBuffer);

        vkCmdDrawIndexedIndirect(commandBuffer, vkArgumentBuffer, argumentBufferOffset, drawCount, sizeof(VkDrawIndexedIndirectCommand));
    }

    void RendererVK::DrawIndexedIndirectCount(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, BufferID drawCountBuffer, u32 drawCountBufferOffset, u32 maxDrawCount)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_commandListHandler->GetRenderPassOpenCount(commandListID) <= 0)
        {
            NC_LOG_CRITICAL("You tried to draw without first calling BeginPipeline!");
        }

        VkBuffer vkArgumentBuffer = _bufferHandler->GetBuffer(argumentBuffer);
        VkBuffer vkDrawCountBuffer = _bufferHandler->GetBuffer(drawCountBuffer);

        Backend::RenderDeviceVK::fnVkCmdDrawIndexedIndirectCountKHR(commandBuffer, vkArgumentBuffer, argumentBufferOffset, vkDrawCountBuffer, drawCountBufferOffset, maxDrawCount, sizeof(VkDrawIndexedIndirectCommand));
    }

    void RendererVK::Dispatch(CommandListID commandListID, u32 threadGroupCountX, u32 threadGroupCountY, u32 threadGroupCountZ)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        vkCmdDispatch(commandBuffer, threadGroupCountX, threadGroupCountY, threadGroupCountZ);
    }

    void RendererVK::DispatchIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkBuffer vkArgumentBuffer = _bufferHandler->GetBuffer(argumentBuffer);

        vkCmdDispatchIndirect(commandBuffer, vkArgumentBuffer, argumentBufferOffset);
    }

    void RendererVK::PopMarker(CommandListID commandListID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        Backend::DebugMarkerUtilVK::PopMarker(commandBuffer);
    }

    void RendererVK::PushMarker(CommandListID commandListID, Color color, std::string name)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        Backend::DebugMarkerUtilVK::PushMarker(commandBuffer, color, name);
    }

    void RendererVK::BeginPipeline(CommandListID commandListID, GraphicsPipelineID pipelineID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        const GraphicsPipelineDesc& pipelineDesc = _pipelineHandler->GetDescriptor(pipelineID);
        VkPipeline pipeline = _pipelineHandler->GetPipeline(pipelineID);
        VkRenderPass renderPass = _pipelineHandler->GetRenderPass(pipelineID);
        VkFramebuffer frameBuffer = _pipelineHandler->GetFramebuffer(pipelineID);

        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount != 0)
        {
            NC_LOG_CRITICAL("You need to match your BeginPipeline calls with a EndPipeline call before beginning another pipeline!");
        }
        renderPassOpenCount++;
        _commandListHandler->SetRenderPassOpenCount(commandListID, renderPassOpenCount);

        uvec2 renderSize = _pipelineHandler->GetRenderPassResolution(pipelineID);

        // Transition depth stencil to DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        if (pipelineDesc.depthStencil != DepthImageMutableResource::Invalid())
        {
            DepthImageID depthImageID = pipelineDesc.MutableResourceToDepthImageID(pipelineDesc.depthStencil);
            const DepthImageDesc& depthImageDesc = _imageHandler->GetImageDesc(depthImageID);

            u32 imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
            // TODO: If we add stencil support we need to selectively add VK_IMAGE_ASPECT_STENCIL_BIT to imageAspect if the depthStencil has a stencil

            VkImage depthImage = _imageHandler->GetImage(depthImageID);
            _device->TransitionImageLayout(commandBuffer, depthImage, imageAspect, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1);
        }
        
        // Set up renderpass
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = frameBuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { renderSize.x, renderSize.y };

        // Start renderpass
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        _commandListHandler->SetBoundGraphicsPipeline(commandListID, pipelineID);
    }

    void RendererVK::EndPipeline(CommandListID commandListID, GraphicsPipelineID pipelineID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount <= 0)
        {
            NC_LOG_CRITICAL("You tried to call EndPipeline without first calling BeginPipeline!");
        }
        renderPassOpenCount--;
        _commandListHandler->SetRenderPassOpenCount(commandListID, renderPassOpenCount);

        vkCmdEndRenderPass(commandBuffer);
        _commandListHandler->SetBoundGraphicsPipeline(commandListID, GraphicsPipelineID::Invalid());

        // Transition depth stencil to DEPTH_STENCIL_READ_ONLY_OPTIMAL
        const GraphicsPipelineDesc& pipelineDesc = _pipelineHandler->GetDescriptor(pipelineID);
        if (pipelineDesc.depthStencil != DepthImageMutableResource::Invalid())
        {
            DepthImageID depthImageID = pipelineDesc.MutableResourceToDepthImageID(pipelineDesc.depthStencil);
            const DepthImageDesc& depthImageDesc = _imageHandler->GetImageDesc(depthImageID);

            u32 imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
            // TODO: If we add stencil support we need to selectively add VK_IMAGE_ASPECT_STENCIL_BIT to imageAspect if the depthStencil has a stencil

            VkImage depthImage = _imageHandler->GetImage(depthImageID);
            _device->TransitionImageLayout(commandBuffer, depthImage, imageAspect, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 1, 1);
        }
    }

    void RendererVK::BeginPipeline(CommandListID commandListID, ComputePipelineID pipelineID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkPipeline pipeline = _pipelineHandler->GetPipeline(pipelineID);

        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount != 0)
        {
            NC_LOG_CRITICAL("You need to match your BeginPipeline calls with a EndPipeline call before beginning another pipeline!");
        }
        renderPassOpenCount++;
        _commandListHandler->SetRenderPassOpenCount(commandListID, renderPassOpenCount);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

        _commandListHandler->SetBoundComputePipeline(commandListID, pipelineID);
    }

    void RendererVK::EndPipeline(CommandListID commandListID, ComputePipelineID pipelineID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount <= 0)
        {
            NC_LOG_CRITICAL("You tried to call EndPipeline without first calling BeginPipeline!");
        }
        renderPassOpenCount--;
        _commandListHandler->SetRenderPassOpenCount(commandListID, renderPassOpenCount);

        VkPipeline pipeline = _pipelineHandler->GetPipeline(pipelineID);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

        _commandListHandler->SetBoundComputePipeline(commandListID, ComputePipelineID::Invalid());
    }

    void RendererVK::BeginTimeQuery(CommandListID commandListID, TimeQueryID timeQueryID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        _timeQueryHandler->Begin(commandBuffer, timeQueryID);
        
        _frameTimeQueries.push_back(timeQueryID);
    }

    void RendererVK::EndTimeQuery(CommandListID commandListID, TimeQueryID timeQueryID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        _timeQueryHandler->End(commandBuffer, timeQueryID);
    }

    void RendererVK::SetDepthBias(CommandListID commandListID, DepthBias depthBias)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        vkCmdSetDepthBias(commandBuffer, depthBias.constantFactor, depthBias.clamp, depthBias.slopeFactor);
    }

    void RendererVK::SetScissorRect(CommandListID commandListID, ScissorRect scissorRect)
    {
        _lastScissorRect = scissorRect;

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkRect2D vkScissorRect = {};
        vkScissorRect.offset = { scissorRect.left, scissorRect.top };
        vkScissorRect.extent = { static_cast<u32>(scissorRect.right - scissorRect.left), static_cast<u32>(scissorRect.bottom - scissorRect.top) };

        vkCmdSetScissor(commandBuffer, 0, 1, &vkScissorRect);
    }

    void RendererVK::SetViewport(CommandListID commandListID, Viewport viewport)
    {
        _lastViewport = viewport;

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkViewport vkViewport = {};
        vkViewport.x = viewport.topLeftX;
        vkViewport.y = viewport.height - viewport.topLeftY;
        vkViewport.width = viewport.width;
        vkViewport.height = -viewport.height;
        vkViewport.minDepth = viewport.minDepth;
        vkViewport.maxDepth = viewport.maxDepth;

        vkCmdSetViewport(commandBuffer, 0, 1, &vkViewport);
    }

    void RendererVK::SetVertexBuffer(CommandListID commandListID, u32 slot, BufferID bufferID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        // Bind vertex buffer
        VkBuffer vertexBuffer = _bufferHandler->GetBuffer(bufferID);
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, slot, 1, &vertexBuffer, offsets);
    }

    void RendererVK::SetIndexBuffer(CommandListID commandListID, BufferID bufferID, IndexFormat indexFormat)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        // Bind index buffer
        VkBuffer indexBuffer = _bufferHandler->GetBuffer(bufferID);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, Backend::FormatConverterVK::ToVkIndexType(indexFormat));
    }

    void RendererVK::SetBuffer(CommandListID commandListID, u32 slot, BufferID buffer)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        // Bind buffer
        VkBuffer vkBuffer = _bufferHandler->GetBuffer(buffer);
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, slot, 1, &vkBuffer, offsets);
    }

    bool RendererVK::ReflectDescriptorSet(const std::string& name, u32 nameHash, u32 type, i32& set, const std::vector<Backend::BindInfo>& bindInfos, u32& outBindInfoIndex, VkDescriptorSetLayoutBinding* outDescriptorLayoutBinding)
    {
        // Try to find a BindInfo with a matching name
        for(u32 i = 0; i < bindInfos.size(); i++)
        {
            auto& bindInfo = bindInfos[i];

            // If the name and type matches
            if (nameHash == bindInfo.nameHash && type == bindInfo.descriptorType)
            {
                // If we have a set, make sure it's the correct one
                if (set != -1)
                {
                    if (set != bindInfo.set)
                    {
                        NC_LOG_ERROR("While creating DescriptorSet, we found BindInfo with matching name ({}) and type ({}), but it didn't match the location ({} != {})", bindInfo.name, static_cast<std::underlying_type<DescriptorType>::type>(bindInfo.descriptorType), bindInfo.set, set);
                    }
                }
                else
                {
                    set = bindInfo.set;
                }

                // Fill out descriptor set layout
                outDescriptorLayoutBinding->binding = bindInfo.binding;
                outDescriptorLayoutBinding->descriptorType = bindInfo.descriptorType;
                outDescriptorLayoutBinding->descriptorCount = bindInfo.count;
                outDescriptorLayoutBinding->stageFlags = bindInfo.stageFlags;
                outDescriptorLayoutBinding->pImmutableSamplers = NULL;

                outBindInfoIndex = i;

                return true;
            }
        }

        NC_LOG_ERROR("While creating DescriptorSet we encountered binding (%s) of type (%u) which did not have a matching BindInfo in the bound shaders", name.c_str(), type);
        return false;
    }

    void RendererVK::BindDescriptor(Backend::DescriptorSetBuilderVK& builder, Descriptor& descriptor)
    {
        if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_BUFFER)
        {
            ZoneScopedN("Buffer");
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = _bufferHandler->GetBuffer(descriptor.bufferID);
            bufferInfo.range = _bufferHandler->GetBufferSize(descriptor.bufferID);

            builder.BindBuffer(descriptor.nameHash, bufferInfo, descriptor.bufferID);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_BUFFER_ARRAY)
        {
            ZoneScopedN("BufferArray");
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = _bufferHandler->GetBuffer(descriptor.bufferID);
            bufferInfo.range = _bufferHandler->GetBufferSize(descriptor.bufferID);

            builder.BindBufferArrayIndex(descriptor.nameHash, bufferInfo, descriptor.arrayIndex, descriptor.bufferID);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_SAMPLER)
        {
            ZoneScopedN("Sampler");
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.sampler = _samplerHandler->GetSampler(descriptor.samplerID);

            builder.BindSampler(descriptor.nameHash, imageInfo);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_SAMPLER_ARRAY)
        {
            ZoneScopedN("SamplerArray");
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.sampler = _samplerHandler->GetSampler(descriptor.samplerID);

            builder.BindSamplerArrayIndex(descriptor.nameHash, imageInfo, descriptor.arrayIndex);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_TEXTURE)
        {
            ZoneScopedN("Texture");
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = _textureHandler->GetImageView(descriptor.textureID);

            builder.BindImage(descriptor.nameHash, imageInfo);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_TEXTURE_ARRAY)
        {
            ZoneScopedN("TextureArray");
            const SafeVector<TextureID>& textureIDs = _textureHandler->GetTextureIDsInArray(descriptor.textureArrayID);

            u32 textureArraySize = _textureHandler->GetTextureArraySize(descriptor.textureArrayID);
            
            VkDescriptorImageInfo* imageInfos = Memory::Allocator::NewArray<VkDescriptorImageInfo>(&_frameAllocator, textureArraySize);
            u32 numImageInfos = 0;
            
            u32 numTextures = static_cast<u32>(textureIDs.Size());

            // From 0 to numTextures, add our actual textures
            bool texturesAreOnionTextures = false;

            textureIDs.ReadLock(
                [&](const std::vector<TextureID> textures)
                {
                    for (auto textureID : textures)
                    {
                        VkDescriptorImageInfo& imageInfo = imageInfos[numImageInfos++];
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageInfo.imageView = _textureHandler->GetImageView(textureID);
                        imageInfo.sampler = VK_NULL_HANDLE;

                        texturesAreOnionTextures = _textureHandler->IsOnionTexture(textureID);
                    }
                });

            // from numTextures to textureArraySize, add debug texture
            VkDescriptorImageInfo imageInfoDebugTexture;
            imageInfoDebugTexture.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            if (texturesAreOnionTextures)
            {
                imageInfoDebugTexture.imageView = _textureHandler->GetDebugOnionTextureImageView();
            }
            else
            {
                imageInfoDebugTexture.imageView = _textureHandler->GetDebugTextureImageView();
            }
            
            imageInfoDebugTexture.sampler = VK_NULL_HANDLE;

            for (u32 i = numTextures; i < textureArraySize; i++)
            {
                VkDescriptorImageInfo& imageInfo = imageInfos[numImageInfos++];
                imageInfo = imageInfoDebugTexture;
            }
            
            builder.BindImageArray(descriptor.nameHash, imageInfos, textureArraySize);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_IMAGE)
        {
            ZoneScopedN("Image");
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo.imageView = _imageHandler->GetColorView(descriptor.imageID,descriptor.imageMipLevel);

            builder.BindImage(descriptor.nameHash, imageInfo);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_IMAGE_ARRAY)
        {
            ZoneScopedN("ImageArray");
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo.imageView = _imageHandler->GetColorView(descriptor.imageID, descriptor.imageMipLevel);

            builder.BindImageArrayIndex(descriptor.nameHash, imageInfo, descriptor.arrayIndex);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_DEPTH_IMAGE)
        {
            ZoneScopedN("DepthImage");
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            imageInfo.imageView = _imageHandler->GetDepthView(descriptor.depthImageID);

            builder.BindImage(descriptor.nameHash, imageInfo);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_DEPTH_IMAGE_ARRAY)
        {
            ZoneScopedN("DepthImageArray");
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            imageInfo.imageView = _imageHandler->GetDepthView(descriptor.depthImageID);

            builder.BindImageArrayIndex(descriptor.nameHash, imageInfo, descriptor.arrayIndex);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_STORAGE_IMAGE)
        {
            ZoneScopedN("StorageImage");
            VkDescriptorImageInfo* imageInfos = Memory::Allocator::NewArray<VkDescriptorImageInfo>(&_frameAllocator, descriptor.count);
            u32 numImageInfos = 0;

            const ImageDesc& imageDesc = _imageHandler->GetImageDesc(descriptor.imageID);

            for (u32 i = 0; i < descriptor.count; i++)
            {
                u32 mipLevel = descriptor.imageMipLevel + i;

                if (mipLevel < imageDesc.mipLevels)
                {
                    VkDescriptorImageInfo& imageInfo = imageInfos[numImageInfos++];
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                    imageInfo.imageView = _imageHandler->GetColorView(descriptor.imageID, mipLevel);
                    imageInfo.sampler = VK_NULL_HANDLE;
                }
                else
                {
                    // Any imageInfos pointing to mips we don't have should point to the last mip
                    VkDescriptorImageInfo& imageInfo = imageInfos[numImageInfos++];
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                    imageInfo.imageView = _imageHandler->GetColorView(descriptor.imageID, imageDesc.mipLevels - 1);
                    imageInfo.sampler = VK_NULL_HANDLE;
                }
            }

            builder.BindStorageImage(descriptor.nameHash, imageInfos, static_cast<i32>(descriptor.count));
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_STORAGE_IMAGE_ARRAY)
        {
            ZoneScopedN("StorageImageArray");
            VkDescriptorImageInfo* imageInfos = Memory::Allocator::NewArray<VkDescriptorImageInfo>(&_frameAllocator, descriptor.count);
            u32 numImageInfos = 0;

            const ImageDesc& imageDesc = _imageHandler->GetImageDesc(descriptor.imageID);

            for (u32 i = 0; i < descriptor.count; i++)
            {
                u32 mipLevel = descriptor.imageMipLevel + i;

                if (mipLevel < imageDesc.mipLevels)
                {
                    VkDescriptorImageInfo& imageInfo = imageInfos[numImageInfos++];
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                    imageInfo.imageView = _imageHandler->GetColorArrayView(descriptor.imageID, mipLevel);
                    imageInfo.sampler = VK_NULL_HANDLE;
                }
                else
                {
                    // Any imageInfos pointing to mips we don't have should point to the last mip
                    VkDescriptorImageInfo& imageInfo = imageInfos[numImageInfos++];
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                    imageInfo.imageView = _imageHandler->GetColorArrayView(descriptor.imageID, imageDesc.mipLevels - 1);
                    imageInfo.sampler = VK_NULL_HANDLE;
                }
            }

            builder.BindStorageImageArray(descriptor.nameHash, imageInfos, static_cast<i32>(descriptor.count));
        }
    }

    void RendererVK::RecreateSwapChain(Backend::SwapChainVK* swapChain)
    {
        _device->RecreateSwapChain(_imageHandler, _semaphoreHandler, swapChain);
        
        _pipelineHandler->DiscardPipelines();
        CreateDummyPipeline();
        
        _imageHandler->OnResize(true);
    }

    void RendererVK::CreateDummyPipeline()
    {
        // Load dummy pipeline containing our global descriptorset
        VertexShaderDesc dummyShaderDesc;
        dummyShaderDesc.path = "globalDataDummy.vs.hlsl";

        GraphicsPipelineDesc dummyPipelineDesc;
        dummyPipelineDesc.states.vertexShader = _shaderHandler->LoadShader(dummyShaderDesc);

        _globalDummyPipeline = _pipelineHandler->CreatePipeline(dummyPipelineDesc);
    }

    void RendererVK::DestroyObjects(ObjectDestroyList& destroyList)
    {
        for (const BufferID buffer : destroyList.buffers)
        {
            _bufferHandler->DestroyBuffer(buffer);
        }

        destroyList.buffers.clear();
    }

    void RendererVK::BindDescriptorSet(CommandListID commandListID, DescriptorSetSlot slot, Descriptor* descriptors, u32 numDescriptors, const TrackedBufferBitSets* bufferPermissions)
    {
        ZoneScopedNC("RendererVK::BindDescriptorSet", tracy::Color::Red3);

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        GraphicsPipelineID graphicsPipelineID = _commandListHandler->GetBoundGraphicsPipeline(commandListID);
        ComputePipelineID computePipelineID = _commandListHandler->GetBoundComputePipeline(commandListID);

        if (graphicsPipelineID != GraphicsPipelineID::Invalid())
        {
            if (_pipelineHandler->GetNumDescriptorSetLayouts(graphicsPipelineID) <= static_cast<u32>(slot))
            {
                return;
            }

            Backend::DescriptorSetBuilderVK& builder = _pipelineHandler->GetDescriptorSetBuilder(graphicsPipelineID);
            builder.SetBufferPermissions(bufferPermissions);

            for (u32 i = 0; i < numDescriptors; i++)
            {
                ZoneScopedNC("BindDescriptor", tracy::Color::Red3);
                Descriptor& descriptor = descriptors[i];
                BindDescriptor(builder, descriptor);
            }

            VkDescriptorSet descriptorSet = builder.BuildDescriptorSet(static_cast<i32>(slot), Backend::DescriptorLifetime::PerFrame);
            Backend::DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)descriptorSet, VK_OBJECT_TYPE_DESCRIPTOR_SET, DescriptorSetToName(slot));

            VkPipelineLayout pipelineLayout = _pipelineHandler->GetPipelineLayout(graphicsPipelineID);

            // Bind descriptor set
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, slot, 1, &descriptorSet, 0, nullptr);
        } 
        else if (computePipelineID != ComputePipelineID::Invalid())
        {
            if (_pipelineHandler->GetNumDescriptorSetLayouts(computePipelineID) <= static_cast<u32>(slot))
            {
                return;
            }

            Backend::DescriptorSetBuilderVK& builder = _pipelineHandler->GetDescriptorSetBuilder(computePipelineID);
            builder.SetBufferPermissions(bufferPermissions);

            for (u32 i = 0; i < numDescriptors; i++)
            {
                ZoneScopedNC("BindDescriptor", tracy::Color::Red3);
                Descriptor& descriptor = descriptors[i];
                BindDescriptor(builder, descriptor);
            }

            VkDescriptorSet descriptorSet = builder.BuildDescriptorSet(static_cast<i32>(slot), Backend::DescriptorLifetime::PerFrame);
            Backend::DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)descriptorSet, VK_OBJECT_TYPE_DESCRIPTOR_SET, DescriptorSetToName(slot));

            VkPipelineLayout pipelineLayout = _pipelineHandler->GetPipelineLayout(computePipelineID);

            // Bind descriptor set
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, slot, 1, &descriptorSet, 0, nullptr);
        }
    }

    void RendererVK::MarkFrameStart(CommandListID commandListID, u32 frameIndex)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        // Collect tracy timings
        TracyVkCollect(_device->_tracyContext, commandBuffer);

        // Add a marker specifying the frameIndex
        Backend::DebugMarkerUtilVK::PushMarker(commandBuffer, Color(1,1,1,1), std::to_string(frameIndex));
        Backend::DebugMarkerUtilVK::PopMarker(commandBuffer);

        // Free up any old descriptors
        _device->_descriptorMegaPool->SetFrame(frameIndex);
    }

#if !TRACY_ENABLE
    void RendererVK::BeginTrace(CommandListID /*commandListID*/, const tracy::SourceLocationData* /*sourceLocation*/)
    {
#else
    void RendererVK::BeginTrace(CommandListID commandListID, const tracy::SourceLocationData* sourceLocation)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        tracy::VkCtxManualScope*& tracyScope = _commandListHandler->GetTracyScope(commandListID);

        if (tracyScope != nullptr)
        {
            NC_LOG_CRITICAL("Tried to begin GPU trace on a commandlist that already had a begun GPU trace");
        }

        tracyScope = new tracy::VkCtxManualScope(_device->_tracyContext, sourceLocation, true);
        tracyScope->Start(commandBuffer);
#endif
    }

#if !TRACY_ENABLE
    void RendererVK::EndTrace(CommandListID /*commandListID*/)
    {
#else
    void RendererVK::EndTrace(CommandListID commandListID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        tracy::VkCtxManualScope*& tracyScope = _commandListHandler->GetTracyScope(commandListID);

        if (tracyScope == nullptr)
        {
            NC_LOG_CRITICAL("Tried to end GPU trace on a commandlist that didn't have a running trace");
        }

        tracyScope->End();
        delete tracyScope;
        tracyScope = nullptr;
#endif
    }

    void RendererVK::AddSignalSemaphore(CommandListID commandListID, SemaphoreID semaphoreID)
    {
        VkSemaphore semaphore = _semaphoreHandler->GetVkSemaphore(semaphoreID);
        _commandListHandler->AddSignalSemaphore(commandListID, semaphore);
    }

    void RendererVK::AddWaitSemaphore(CommandListID commandListID, SemaphoreID semaphoreID)
    {
        VkSemaphore semaphore = _semaphoreHandler->GetVkSemaphore(semaphoreID);
        _commandListHandler->AddWaitSemaphore(commandListID, semaphore);
    }

    void RendererVK::CopyImage(CommandListID commandListID, ImageID dstImageID, uvec2 dstPos, u32 dstMipLevel, ImageID srcImageID, uvec2 srcPos, u32 srcMipLevel, uvec2 size)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkImage dstImage = _imageHandler->GetImage(dstImageID);
        VkImage srcImage = _imageHandler->GetImage(srcImageID);

        VkImageCopy imageCopy = {};
        imageCopy.dstOffset.x = dstPos.x;
        imageCopy.dstOffset.y = dstPos.y;
        imageCopy.dstSubresource.mipLevel = dstMipLevel;
        imageCopy.dstSubresource.layerCount = 1;
        imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        imageCopy.srcOffset.x = srcPos.x;
        imageCopy.srcOffset.y = srcPos.y;
        imageCopy.srcSubresource.mipLevel = srcMipLevel;
        imageCopy.srcSubresource.layerCount = 1;
        imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        imageCopy.extent.width = size.x;
        imageCopy.extent.height = size.y;
        imageCopy.extent.depth = 1;
        
        vkCmdCopyImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_GENERAL, dstImage, VK_IMAGE_LAYOUT_GENERAL, 1, &imageCopy);
    }

    void RendererVK::CopyImage(CommandListID commandListID, DepthImageID dstImageID, uvec2 dstPos, DepthImageID srcImageID, uvec2 srcPos, uvec2 size)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkImage dstImage = _imageHandler->GetImage(dstImageID);
        VkImage srcImage = _imageHandler->GetImage(srcImageID);

        VkImageCopy imageCopy = {};
        imageCopy.dstOffset.x = dstPos.x;
        imageCopy.dstOffset.y = dstPos.y;
        imageCopy.dstSubresource.mipLevel = 0;
        imageCopy.dstSubresource.layerCount = 1;
        imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        imageCopy.srcOffset.x = srcPos.x;
        imageCopy.srcOffset.y = srcPos.y;
        imageCopy.srcSubresource.mipLevel = 0;
        imageCopy.srcSubresource.layerCount = 1;
        imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        imageCopy.extent.width = size.x;
        imageCopy.extent.height = size.y;
        imageCopy.extent.depth = 1;

        // Transition images to TRANSFER_(DST/SRC)_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, dstImage, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1);
        _device->TransitionImageLayout(commandBuffer, srcImage, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1, 1);

        vkCmdCopyImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

        // Transition images back to GENERAL
        _device->TransitionImageLayout(commandBuffer, dstImage, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 1, 1);
        _device->TransitionImageLayout(commandBuffer, srcImage, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 1, 1);
    }

    void RendererVK::CopyBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkBuffer vkDstBuffer = _bufferHandler->GetBuffer(dstBuffer);
        VkBuffer vkSrcBuffer = _bufferHandler->GetBuffer(srcBuffer);

        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = srcOffset;
        copyRegion.dstOffset = dstOffset;
        copyRegion.size = range;

        size_t srcBufferSize = _bufferHandler->GetBufferSize(srcBuffer);
        size_t dstBufferSize = _bufferHandler->GetBufferSize(dstBuffer);

        if (srcOffset + range > srcBufferSize)
        {
            NC_LOG_CRITICAL("[RendererVK::CopyBuffer] Source Buffer out of bounds!");
        }
        if (dstOffset + range > dstBufferSize)
        {
            NC_LOG_CRITICAL("[RendererVK::CopyBuffer] Destination Buffer out of bounds!");
        }

        vkCmdCopyBuffer(commandBuffer, vkSrcBuffer, vkDstBuffer, 1, &copyRegion);
    }

    void RendererVK::ImageBarrier(CommandListID commandListID, ImageID image)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        const VkImage& vkImage = _imageHandler->GetImage(image);
        const ImageDesc& imageDesc = _imageHandler->GetImageDesc(image);

        _device->TransitionImageLayout(commandBuffer, vkImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, imageDesc.depth, imageDesc.mipLevels);
    }

    void RendererVK::ImageBarrier(CommandListID commandListID, DepthImageID image)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        const VkImage& vkImage = _imageHandler->GetImage(image);
        const DepthImageDesc& imageDesc = _imageHandler->GetImageDesc(image);

        u32 imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
        // TODO: If we add stencil support we need to selectively add VK_IMAGE_ASPECT_STENCIL_BIT to imageAspect if the depthStencil has a stencil

        _device->TransitionImageLayout(commandBuffer, vkImage, imageAspect, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 1, 1);
    }

    void RendererVK::BufferBarrier(CommandListID commandListID, BufferID bufferID, BufferPassUsage from)
    {
        NC_ASSERT(commandListID != CommandListID::Invalid(), "RendererVK : BufferBarrier got invalid commandListID");
        NC_ASSERT(bufferID != BufferID::Invalid(), "RendererVK : BufferBarrier got invalid bufferID");
        NC_ASSERT(from != BufferPassUsage::NONE, "RendererVK : BufferBarrier from BufferPassUsage was NONE");

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkPipelineStageFlags srcStageMask = 0;
        VkPipelineStageFlags dstStageMask = 0;

        VkBufferMemoryBarrier bufferBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
        bufferBarrier.buffer = _bufferHandler->GetBuffer(bufferID);
        bufferBarrier.size = VK_WHOLE_SIZE;
        bufferBarrier.srcAccessMask = 0;
        bufferBarrier.dstAccessMask = 0;

        if ((from & BufferPassUsage::TRANSFER) == BufferPassUsage::TRANSFER)
        {
            srcStageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
            bufferBarrier.srcAccessMask |= VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        if ((from & BufferPassUsage::GRAPHICS) == BufferPassUsage::GRAPHICS)
        {
            srcStageMask |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            bufferBarrier.srcAccessMask |= VK_ACCESS_SHADER_WRITE_BIT;
        }
        if ((from & BufferPassUsage::COMPUTE) == BufferPassUsage::COMPUTE)
        {
            srcStageMask |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            bufferBarrier.srcAccessMask |= VK_ACCESS_SHADER_WRITE_BIT;
        }

        // Transfer
        dstStageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        bufferBarrier.dstAccessMask |= VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        
        // Graphics
        dstStageMask |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
        bufferBarrier.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        
        // Compute
        dstStageMask |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
        bufferBarrier.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        

        vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 1, &bufferBarrier, 0, nullptr);
    }

    void RendererVK::UploadBufferBarrier(CommandListID commandListID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        _uploadBufferHandler->SyncBarrier(commandBuffer);
    }

    void RendererVK::PushConstant(CommandListID commandListID, void* data, u32 offset, u32 size)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        GraphicsPipelineID graphicsPipelineID = _commandListHandler->GetBoundGraphicsPipeline(commandListID);

        if (graphicsPipelineID != GraphicsPipelineID::Invalid())
        {
            VkShaderStageFlags shaderStageFlags = 0;

            u32 numPushConstantRanges = _pipelineHandler->GetNumPushConstantRanges(graphicsPipelineID);
            for (u32 i = 0; i < numPushConstantRanges; i++)
            {
                const VkPushConstantRange& constantRange = _pipelineHandler->GetPushConstantRange(graphicsPipelineID, i);

                if (offset >= constantRange.offset && offset < constantRange.offset + constantRange.size)
                {
                    shaderStageFlags |= constantRange.stageFlags;
                }
            }

            if (shaderStageFlags == 0)
            {
                NC_LOG_CRITICAL("RendererVK::PushConstant : shaderStageFlags was 0, did we try to push a constant that doesn't exist?");
            }

            VkPipelineLayout layout = _pipelineHandler->GetPipelineLayout(graphicsPipelineID);
            vkCmdPushConstants(commandBuffer, layout, shaderStageFlags, offset, size, data);
        }

        ComputePipelineID computePipelineID = _commandListHandler->GetBoundComputePipeline(commandListID);
        if (computePipelineID != ComputePipelineID::Invalid())
        {
            VkPipelineLayout layout = _pipelineHandler->GetPipelineLayout(computePipelineID);
            vkCmdPushConstants(commandBuffer, layout, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, offset, size, data);
        }
    }

    void RendererVK::Present(Novus::Window* window, ImageID imageID, SemaphoreID semaphoreID)
    {
        CommandListID commandListID = _commandListHandler->BeginCommandList(Backend::QueueType::Graphics);
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        Backend::DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)commandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER,"Present CommandList");
        
        // Tracy profiling
        PushMarker(commandListID, Color::PastelRed, "Present Blitting");
        tracy::VkCtxManualScope*& tracyScope = _commandListHandler->GetTracyScope(commandListID);

        {
            ZoneScopedNC("Present::TracyScope", tracy::Color::Red);
            if (tracyScope != nullptr)
            {
                NC_LOG_CRITICAL("Tried to begin GPU trace on a commandlist that already had a begun GPU trace");
            }

#if TRACY_ENABLE
            TracySourceLocation(presentBlitting, "PresentBlitting", tracy::Color::Yellow2);
            tracyScope = new tracy::VkCtxManualScope(_device->_tracyContext, &presentBlitting, true);
            tracyScope->Start(commandBuffer);
#endif
        }

        Backend::SwapChainVK* swapChain = static_cast<Backend::SwapChainVK*>(window->GetSwapChain());
        u32 semaphoreIndex = swapChain->frameIndex;

        VkFence frameFence = _commandListHandler->GetCurrentFence();
        Backend::DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)frameFence, VK_OBJECT_TYPE_FENCE, "Present Frame Fence");

        VkSemaphore imageAvailableSemaphore = _semaphoreHandler->GetVkSemaphore(swapChain->imageAvailableSemaphores.Get(semaphoreIndex));
        Backend::DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)imageAvailableSemaphore, VK_OBJECT_TYPE_SEMAPHORE, "Present Image Available Semaphore");

        // Acquire next swapchain image
        VkResult result;
        u32 frameIndex;
        {
            ZoneScopedNC("Present::AcquireNextImage", tracy::Color::Red);
            result = vkAcquireNextImageKHR(_device->_device, swapChain->swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &frameIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                RecreateSwapChain(swapChain);
                return;
            }
            else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            {
                NC_LOG_CRITICAL("Failed to acquire swap chain image!");
            }
        }
        
        {
            ZoneScopedNC("Present::AddSemaphores", tracy::Color::Red);

            if (semaphoreID != SemaphoreID::Invalid())
            {
                VkSemaphore semaphore = _semaphoreHandler->GetVkSemaphore(semaphoreID);
                Backend::DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)semaphore, VK_OBJECT_TYPE_SEMAPHORE, "Present Wait Semaphore");

                _commandListHandler->AddWaitSemaphore(commandListID, semaphore); // Wait for the provided semaphore to finish
            }

            VkSemaphore blitFinishedSemaphore = _semaphoreHandler->GetVkSemaphore(swapChain->blitFinishedSemaphores.Get(semaphoreIndex));
            Backend::DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)imageAvailableSemaphore, VK_OBJECT_TYPE_SEMAPHORE, "Present Blit Finished Semaphore");

            _commandListHandler->AddWaitSemaphore(commandListID, imageAvailableSemaphore); // Wait for swapchain image to be available
            _commandListHandler->AddSignalSemaphore(commandListID, blitFinishedSemaphore); // Signal that blitting is done
        }
        
        // Create sampler
        SamplerDesc samplerDesc;
        samplerDesc.enabled = true;
        samplerDesc.filter = SamplerFilter::MIN_MAG_MIP_LINEAR;
        samplerDesc.addressU = TextureAddressMode::WRAP;
        samplerDesc.addressV = TextureAddressMode::WRAP;
        samplerDesc.addressW = TextureAddressMode::CLAMP;
        samplerDesc.shaderVisibility = ShaderVisibility::PIXEL;

        SamplerID samplerID = _samplerHandler->CreateSampler(samplerDesc);
        
        {
            ZoneScopedNC("Present::Blit", tracy::Color::Red);

            // Load shaders
            VertexShaderDesc vertexShaderDesc;
            vertexShaderDesc.path = "Blitting/blit.vs.hlsl";

            ImageDesc imageDesc = _imageHandler->GetImageDesc(imageID);
            ImageComponentType componentType = ToImageComponentType(imageDesc.format);
            std::string componentTypeName = "";

            switch (componentType)
            {
                case ImageComponentType::FLOAT:
                    componentTypeName = "float";
                    break;
                case ImageComponentType::SINT:
                case ImageComponentType::SNORM:
                    componentTypeName = "int";
                    break;
                case ImageComponentType::UINT:
                case ImageComponentType::UNORM:
                    componentTypeName = "uint";
                    break;

                default:
                    break;
            }

            u8 componentCount = ToImageComponentCount(imageDesc.format);
            if (componentCount > 1)
            {
                componentTypeName += std::to_string(componentCount);
            }

            PixelShaderDesc pixelShaderDesc;
            pixelShaderDesc.path = "Blitting/blit.ps.hlsl";
            pixelShaderDesc.AddPermutationField("TEX_TYPE", componentTypeName);

            GraphicsPipelineDesc pipelineDesc;

            // We define simple passthrough functions here because we don't have a rendergraph that keeps track of resources while presenting
            pipelineDesc.ResourceToImageID = [](ImageResource resource)
            {
                return ImageID(static_cast<ImageResource::type>(resource));
            };

            pipelineDesc.ResourceToDepthImageID = [](DepthImageResource resource)
            {
                return DepthImageID(static_cast<DepthImageResource::type>(resource));
            };

            pipelineDesc.MutableResourceToImageID = [](ImageMutableResource resource)
            {
                return ImageID(static_cast<ImageMutableResource::type>(resource));
            };

            pipelineDesc.MutableResourceToDepthImageID = [](DepthImageMutableResource resource)
            {
                return DepthImageID(static_cast<DepthImageMutableResource::type>(resource));
            };

            pipelineDesc.states.vertexShader = _shaderHandler->LoadShader(vertexShaderDesc);
            pipelineDesc.states.pixelShader = _shaderHandler->LoadShader(pixelShaderDesc);

            pipelineDesc.states.rasterizerState.cullMode = CullMode::BACK;
            pipelineDesc.states.rasterizerState.frontFaceMode = FrontFaceState::COUNTERCLOCKWISE;

            pipelineDesc.renderTargets[0] = ImageMutableResource(static_cast<ImageID::type>(swapChain->imageIDs.Get(frameIndex)));

            GraphicsPipelineID pipelineID = _pipelineHandler->CreatePipeline(pipelineDesc);
            VkPipelineLayout pipelineLayout = _pipelineHandler->GetPipelineLayout(pipelineID);
            Backend::DescriptorSetBuilderVK& builder = _pipelineHandler->GetDescriptorSetBuilder(pipelineID);
            
            ImageBarrier(commandListID, imageID);
            BeginPipeline(commandListID, pipelineID);

            SetViewport(commandListID, _lastViewport);
            SetScissorRect(commandListID, _lastScissorRect);

            // Set descriptors
            VkDescriptorImageInfo samplerInfo = {};
            samplerInfo.sampler = _samplerHandler->GetSampler(samplerID);
            builder.BindSampler("_sampler"_h, samplerInfo);

            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo.imageView = _imageHandler->GetColorView(imageID);
            builder.BindImage("_texture"_h, imageInfo);

            VkDescriptorSet descriptorSet = builder.BuildDescriptorSet(DescriptorSetSlot::GLOBAL, Backend::DescriptorLifetime::PerFrame);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, DescriptorSetSlot::GLOBAL, 1, &descriptorSet, 0, nullptr);

            struct BlitConstant
            {
                vec4 colorMultiplier;
                vec4 additiveColor;
                u32 channelRedirectors;
            } blitConstant;
            blitConstant.colorMultiplier = vec4(1, 1, 1, 1);
            blitConstant.additiveColor = vec4(0, 0, 0, 0);
            blitConstant.channelRedirectors = 0;
            blitConstant.channelRedirectors |= (1 << 8);
            blitConstant.channelRedirectors |= (2 << 16);
            blitConstant.channelRedirectors |= (3 << 24);

            vkCmdPushConstants(commandBuffer, pipelineLayout, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(BlitConstant), &blitConstant);

            vkCmdDraw(commandBuffer, 3, 1, 0, 0);

            EndPipeline(commandListID, pipelineID);

#if TRACY_ENABLE
            tracyScope->End();
            tracyScope = nullptr;
#endif

            _commandListHandler->EndCommandList(commandListID, frameFence);
        }

        // Present
        {
            //ImageID swapchainImageID = swapChain->imageIDs.Get(frameIndex);
            //VkImage image = _imageHandler->GetImage(swapchainImageID);

            VkSemaphore blitFinishedSemaphore = _semaphoreHandler->GetVkSemaphore(swapChain->blitFinishedSemaphores.Get(semaphoreIndex));

            ZoneScopedNC("Present::Present", tracy::Color::Red);
            VkPresentInfoKHR presentInfo = {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &blitFinishedSemaphore; // Wait for blitting to finish

            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &swapChain->swapChain;
            presentInfo.pImageIndices = &frameIndex;
            presentInfo.pResults = nullptr; // Optional

            {
                ZoneScopedNC("Present::vkQueuePresentKHR", tracy::Color::Red);
                result = vkQueuePresentKHR(_device->_presentQueue, &presentInfo);
            }
            

            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            {
                RecreateSwapChain(swapChain);
                return;
            }
            else if (result != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to present swap chain image!");
            }
        }

        {
            ZoneScopedNC("Present::Cleanup", tracy::Color::Red);
            // Flip frameIndex between 0 and 1
            swapChain->frameIndex = !swapChain->frameIndex;

            _destroyListIndex = (_destroyListIndex + 1) % _destroyLists.size();

            std::scoped_lock lock(_destroyListMutex);
            DestroyObjects(_destroyLists[_destroyListIndex]);
        }
    }

    void RendererVK::Present(Novus::Window* /*window*/, DepthImageID /*image*/, SemaphoreID /*semaphoreID*/)
    {
        
    }

    std::shared_ptr<UploadBuffer> RendererVK::CreateUploadBuffer(BufferID targetBuffer, size_t targetOffset, size_t size)
    {
        return _uploadBufferHandler->CreateUploadBuffer(targetBuffer, targetOffset, size);
    }

    bool RendererVK::ShouldWaitForUpload()
    {
        return _uploadBufferHandler->ShouldWaitForUpload();
    }

    void RendererVK::SetHasWaitedForUpload()
    {
        return _uploadBufferHandler->SetHasWaitedForUpload();
    }

    SemaphoreID RendererVK::GetUploadFinishedSemaphore()
    {
        return _uploadBufferHandler->GetUploadFinishedSemaphore();
    }

    void RendererVK::CopyBuffer(BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range)
    {
        _uploadBufferHandler->CopyBufferToBuffer(dstBuffer, dstOffset, srcBuffer, srcOffset, range);
    }

    void RendererVK::CopyBufferImmediate(BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range)
    {
        std::string dstName = _bufferHandler->GetBufferName(dstBuffer);
        std::string srcName = _bufferHandler->GetBufferName(srcBuffer);
        NC_LOG_WARNING("Immediately copying buffer {} into buffer {}, this is bad for performance and should only be used for debugging", srcName, dstName);

        VkCommandBuffer commandList = _device->BeginSingleTimeCommands();
        
        VkBufferCopy copyRegion = { srcOffset, dstOffset, range };
        vkCmdCopyBuffer(commandList, _bufferHandler->GetBuffer(srcBuffer), _bufferHandler->GetBuffer(dstBuffer), 1, &copyRegion);

        _device->EndSingleTimeCommands(commandList);
    }

    void RendererVK::FillBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, u64 size, u32 data)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkBuffer vkDstBuffer = _bufferHandler->GetBuffer(dstBuffer);

        vkCmdFillBuffer(commandBuffer, vkDstBuffer, dstOffset, size, data);
    }

    void RendererVK::UpdateBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, u64 size, void* data)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkBuffer vkDstBuffer = _bufferHandler->GetBuffer(dstBuffer);

        vkCmdUpdateBuffer(commandBuffer, vkDstBuffer, dstOffset, size, data);
    }

    void RendererVK::DispatchCacao(CommandListID commandListID, FfxCacaoContext* context, DepthImageID depthImage, ImageID normalImage, ImageID outputImage, mat4x4* proj, mat4x4* normalsToView, f32 normalUnpackMul, f32 normalUnpackAdd)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        FfxCacaoDispatchDescription dispatchDescription = {};
        dispatchDescription.commandList = commandBuffer;
        dispatchDescription.depthBuffer = _fidelityFXHandler->ffxGetResource(depthImage, FFX_RESOURCE_STATE_PIXEL_COMPUTE_READ);
        wcsncpy(dispatchDescription.depthBuffer.name, L"DepthBuffer", 63);
        if (normalImage != ImageID::Invalid())
        {
            dispatchDescription.normalBuffer = _fidelityFXHandler->ffxGetResource(normalImage, FFX_RESOURCE_STATE_PIXEL_COMPUTE_READ);
            wcsncpy(dispatchDescription.normalBuffer.name, L"NormalBuffer", 63);
        }
        dispatchDescription.outputBuffer = _fidelityFXHandler->ffxGetResource(outputImage, FFX_RESOURCE_STATE_PIXEL_COMPUTE_READ);
        wcsncpy(dispatchDescription.outputBuffer.name, L"SSAOBuffer", 63);
        dispatchDescription.proj = reinterpret_cast<FfxCacaoMat4x4*>(proj); // FidelityFX expects row major, GLM is column major so this needs to have been transposed already
        dispatchDescription.normalsToView = reinterpret_cast<FfxCacaoMat4x4*>(normalsToView); // FidelityFX expects row major, GLM is column major so this needs to have been transposed already
        dispatchDescription.normalUnpackMul = normalUnpackMul;
        dispatchDescription.normalUnpackAdd = normalUnpackAdd;

        // Transition depth image from VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, (VkImage)dispatchDescription.depthBuffer.resource, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1);
        // Transition image from VK_IMAGE_LAYOUT_GENERAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        if (normalImage != ImageID::Invalid())
        {
            const ImageDesc& normalImageDesc = _imageHandler->GetImageDesc(normalImage);
            _device->TransitionImageLayout(commandBuffer, (VkImage)dispatchDescription.normalBuffer.resource, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, normalImageDesc.depth, normalImageDesc.mipLevels);
        }
        // Transition image from VK_IMAGE_LAYOUT_GENERAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        const ImageDesc& outputImageDesc = _imageHandler->GetImageDesc(outputImage);
        _device->TransitionImageLayout(commandBuffer, (VkImage)dispatchDescription.outputBuffer.resource, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, outputImageDesc.depth, outputImageDesc.mipLevels);

        FfxErrorCode errorCode = ffxCacaoContextDispatch(context, &dispatchDescription);
        if (errorCode != FfxErrorCodes::FFX_OK)
        {
            NC_LOG_CRITICAL("Failed to dispatch CACAO");
            return;
        }

        // Transition depth image from VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL to VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, (VkImage)dispatchDescription.depthBuffer.resource, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 1, 1);
        // Transition image from VK_IMAGE_LAYOUT_GENERAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        if (normalImage != ImageID::Invalid())
        {
            const ImageDesc& normalImageDesc = _imageHandler->GetImageDesc(normalImage);
            _device->TransitionImageLayout(commandBuffer, (VkImage)dispatchDescription.normalBuffer.resource, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, normalImageDesc.depth, normalImageDesc.mipLevels);
        }
        // Transition image from VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL to VK_IMAGE_LAYOUT_GENERAL
        _device->TransitionImageLayout(commandBuffer, (VkImage)dispatchDescription.outputBuffer.resource, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, outputImageDesc.depth, outputImageDesc.mipLevels);
    }

    void* RendererVK::MapBuffer(BufferID buffer)
    {
        void* mappedMemory;

        VkResult result = vmaMapMemory(_device->_allocator, _bufferHandler->GetBufferAllocation(buffer), &mappedMemory);
        if (result != VK_SUCCESS)
        {
            NC_LOG_ERROR("vmaMapMemory failed!\n");
            return nullptr;
        }
        return mappedMemory;
    }
    
    void RendererVK::UnmapBuffer(BufferID buffer)
    {
        vmaUnmapMemory(_device->_allocator, _bufferHandler->GetBufferAllocation(buffer));
    }

    const std::string& RendererVK::GetTimeQueryName(TimeQueryID id)
    {
        return _timeQueryHandler->GetName(id);
    }

    f32 RendererVK::GetLastTimeQueryDuration(TimeQueryID id)
    {
        return _timeQueryHandler->GetLastTime(id);
    }

    i32 RendererVK::ffxGetInterface(FfxInterface* backendInterface, void* scratchBuffer, size_t scratchBufferSize, size_t maxContexts)
    {
        /*
        FFX_RETURN_ON_ERROR(
            !s_BackendRefCount,
            FFX_ERROR_BACKEND_API_ERROR);
        FFX_RETURN_ON_ERROR(
            backendInterface,
            FFX_ERROR_INVALID_POINTER);
        FFX_RETURN_ON_ERROR(
            scratchBuffer,
            FFX_ERROR_INVALID_POINTER);
        FFX_RETURN_ON_ERROR(
            scratchBufferSize >= ffxGetScratchMemorySizeVK(((VkDeviceContext*)device)->vkPhysicalDevice, maxContexts),
            FFX_ERROR_INSUFFICIENT_MEMORY);*/

        backendInterface->fpGetSDKVersion = Backend::GetSDKVersionVK;
        backendInterface->fpCreateBackendContext = Backend::CreateBackendContextVK;
        backendInterface->fpGetDeviceCapabilities = Backend::GetDeviceCapabilitiesVK;
        backendInterface->fpDestroyBackendContext = Backend::DestroyBackendContextVK;
        backendInterface->fpCreateResource = Backend::CreateResourceVK;
        backendInterface->fpDestroyResource = Backend::DestroyResourceVK;
        backendInterface->fpRegisterResource = Backend::RegisterResourceVK;
        backendInterface->fpGetResource = Backend::GetResourceVK;
        backendInterface->fpUnregisterResources = Backend::UnregisterResourcesVK;
        backendInterface->fpGetResourceDescription = Backend::GetResourceDescriptionVK;
        backendInterface->fpCreatePipeline = Backend::CreatePipelineVK;
        backendInterface->fpDestroyPipeline = Backend::DestroyPipelineVK;
        backendInterface->fpScheduleGpuJob = Backend::ScheduleGpuJobVK;
        backendInterface->fpExecuteGpuJobs = Backend::ExecuteGpuJobsVK;

        // Memory assignments
        backendInterface->scratchBuffer = scratchBuffer;
        backendInterface->scratchBufferSize = scratchBufferSize;

        // Map the device
        backendInterface->device = _fidelityFXHandler;

        return FFX_OK;
    }

    size_t RendererVK::ffxGetScratchMemorySize(size_t maxContexts)
    {
        return _fidelityFXHandler->ffxGetScratchMemorySize(maxContexts);
    }

    const std::string& RendererVK::GetGPUName()
    {
        return _device->GetGPUName();
    }

    const std::string& RendererVK::GetDebugName(const TextureID textureID)
    {
        return _textureHandler->GetDebugName(textureID);
    }

    size_t RendererVK::GetVRAMUsage()
    {
        size_t usage = sBudgets[0].usage;

        return usage;
    }

    size_t RendererVK::GetVRAMBudget()
    {
        size_t budget = sBudgets[0].budget;

        return budget;
    }

    void RendererVK::InitImgui()
    {
        _device->InitializeImguiVulkan();
    }

    void RendererVK::DrawImgui(CommandListID commandListID)
    {
        VkCommandBuffer cmd = _commandListHandler->GetCommandBuffer(commandListID);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    }

    void* RendererVK::GetImguiImageHandle(TextureID textureID)
    {
        return static_cast<void*>(_textureHandler->GetImguiImageHandle(textureID));
    }

    void* RendererVK::GetImguiImageHandle(ImageID imageID)
    {
        return static_cast<void*>(_imageHandler->GetImguiImageHandle(imageID));
    }

    u32 RendererVK::GetNumImages()
    {
        return _imageHandler->GetNumImages();
    }

    u32 RendererVK::GetNumDepthImages()
    {
        return _imageHandler->GetNumDepthImages();
    }

    u32 RendererVK::GetNumBuffers()
    {
        return _bufferHandler->GetNumBuffers();
    }

    bool RendererVK::HasExtendedTextureSupport()
    {
        return _device->HasExtendedTextureSupport();
    }
}
