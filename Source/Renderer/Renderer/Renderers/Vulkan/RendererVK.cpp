#include "RendererVK.h"
#include "Renderer/Window.h"

#include "Backend/BufferHandlerVK.h"
#include "Backend/CommandListHandlerVK.h"
#include "Backend/DebugMarkerUtilVK.h"
#include "Backend/DescriptorHandlerVK.h"
#include "Backend/FormatConverterVK.h"
#include "Backend/ImageHandlerVK.h"
#include "Backend/PipelineHandlerVK.h"
#include "Backend/RenderDeviceVK.h"
#include "Backend/SamplerHandlerVK.h"
#include "Backend/SemaphoreHandlerVK.h"
#include "Backend/ShaderHandlerVK.h"
#include "Backend/SwapChainVK.h"
#include "Backend/TextureHandlerVK.h"
#include "Backend/TimeQueryHandlerVK.h"
#include "Backend/UploadBufferHandlerVK.h"

#include <Base/Container/SafeVector.h>
#include <Base/Util/StringUtils.h>
#include <Base/Util/DebugHandler.h>
#include <Base/Util/Timer.h>

#include <tracy/Tracy.hpp>
#include <tracy/TracyVulkan.hpp>
#include <imgui/backends/imgui_impl_vulkan.h>

namespace Renderer
{
    RendererVK::RendererVK(Novus::Window* window)
        : _device(new Backend::RenderDeviceVK(window))
        , _presentDescriptorSet(DescriptorSetSlot::PER_PASS)
    {
        _frameAllocator.Init(64 * 1024 * 1024, "Renderer Allocator");

        // Create handlers
        _bufferHandler = new Backend::BufferHandlerVK();
        _commandListHandler = new Backend::CommandListHandlerVK();
        _descriptorHandler = new Backend::DescriptorHandlerVK();
        _imageHandler = new Backend::ImageHandlerVK();
        _pipelineHandler = new Backend::PipelineHandlerVK();
        _samplerHandler = new Backend::SamplerHandlerVK();
        _semaphoreHandler = new Backend::SemaphoreHandlerVK();
        _shaderHandler = new Backend::ShaderHandlerVK();
        _textureHandler = new Backend::TextureHandlerVK();
        _timeQueryHandler = new Backend::TimeQueryHandlerVK();
        _uploadBufferHandler = new Backend::UploadBufferHandlerVK();

        // Init
        _device->Init();

        _bufferHandler->Init(_device);
        _descriptorHandler->Init(_device, _textureHandler);
        _commandListHandler->Init(_device);
        _samplerHandler->Init(_device);
        _semaphoreHandler->Init(_device);
        _shaderHandler->Init(_device);
        _timeQueryHandler->Init(_device);
        _imageHandler->Init(_device, _samplerHandler);
        _pipelineHandler->Init(&_frameAllocator, _device, _shaderHandler, _imageHandler, _bufferHandler);
        _textureHandler->Init(_device, _bufferHandler, _descriptorHandler, _uploadBufferHandler, _samplerHandler);
        _uploadBufferHandler->Init(this, _device, _bufferHandler, _textureHandler, _semaphoreHandler, _commandListHandler);
    }

    void RendererVK::InitDebug()
    {
        _imageHandler->PostInit();
        _textureHandler->InitDebugTexture();

        //CreateDummyPipeline();
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
        _pipelineHandler->RecreatePipelines();

        //CreateDummyPipeline();
    }

    void RendererVK::ClearUploadBuffers()
    {
        _uploadBufferHandler->Clear();
    }

    void RendererVK::SetRenderSize(const vec2& renderSize)
    {
        _renderSize = renderSize;
        _renderSizeChanged = true;

        Renderer::SetRenderSize(renderSize);
    }

    const vec2& RendererVK::GetRenderSize()
    {
        return _renderSize;
    }

    vec2 RendererVK::GetWindowSize()
    {
        return _device->GetMainWindowSize();
    }

    ImageFormat RendererVK::GetSwapChainImageFormat()
    {
        return _device->GetSwapChainImageFormat();
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

    DescriptorSetID RendererVK::CreateDescriptorSet(const DescriptorSetDesc& desc)
    {
        return _descriptorHandler->CreateDescriptorSet(desc);
    }

    GraphicsPipelineID RendererVK::CreatePipeline(GraphicsPipelineDesc& desc)
    {
        return _pipelineHandler->CreatePipeline(desc);
    }

    ComputePipelineID RendererVK::CreatePipeline(ComputePipelineDesc& desc)
    {
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

    void RendererVK::BindDescriptor(DescriptorSetID descriptorSetID, u32 bindingIndex, BufferID bufferID, DescriptorType type, u32 frameIndex)
    {
        VkBuffer buffer = _bufferHandler->GetBuffer(bufferID);
        _descriptorHandler->BindDescriptor(descriptorSetID, bindingIndex, buffer, type, frameIndex);
    }

    void RendererVK::BindDescriptor(DescriptorSetID descriptorSetID, u32 bindingIndex, ImageID imageID, u32 mipLevel, DescriptorType type, u32 frameIndex)
    {
        VkImageView imageView = _imageHandler->GetColorView(imageID, mipLevel);
        _descriptorHandler->BindDescriptor(descriptorSetID, bindingIndex, imageView, type, true, frameIndex);
    }

    void RendererVK::BindDescriptorArray(DescriptorSetID descriptorSetID, u32 bindingIndex, ImageID imageID, u32 mipLevel, u32 mipCount, DescriptorType type, u32 frameIndex)
    {
        std::vector<VkImageView> imageViews;
        for (u32 i = 0; i < mipCount; i++)
        {
            u32 mip = mipLevel + i;
            VkImageView imageView = _imageHandler->GetColorView(imageID, mip);
            imageViews.push_back(imageView);
        }
        _descriptorHandler->BindDescriptorArray(descriptorSetID, bindingIndex, imageViews, 0, type, true, frameIndex);
    }

    void RendererVK::BindDescriptor(DescriptorSetID descriptorSetID, u32 bindingIndex, DepthImageID imageID, DescriptorType type, u32 frameIndex)
    {
        VkImageView imageView = _imageHandler->GetDepthView(imageID);
        _descriptorHandler->BindDescriptor(descriptorSetID, bindingIndex, imageView, type, true, frameIndex);
    }

    void RendererVK::BindDescriptorArray(DescriptorSetID descriptorSetID, u32 bindingIndex, DepthImageID imageID, u32 arrayIndex, DescriptorType type, u32 frameIndex)
    {
        VkImageView imageView = _imageHandler->GetDepthView(imageID);
        _descriptorHandler->BindDescriptorArray(descriptorSetID, bindingIndex, imageView, arrayIndex, type, true, frameIndex);
    }

    void RendererVK::BindDescriptor(DescriptorSetID descriptorSetID, u32 bindingIndex, SamplerID samplerID, u32 frameIndex)
    {
        VkSampler sampler = _samplerHandler->GetSampler(samplerID);
        _descriptorHandler->BindDescriptor(descriptorSetID, bindingIndex, sampler, frameIndex);
    }

    void RendererVK::BindDescriptorArray(DescriptorSetID descriptorSetID, u32 bindingIndex, SamplerID samplerID, u32 arrayIndex, u32 frameIndex)
    {
        VkSampler sampler = _samplerHandler->GetSampler(samplerID);
        _descriptorHandler->BindDescriptorArray(descriptorSetID, bindingIndex, sampler, arrayIndex, frameIndex);
    }

    void RendererVK::BindDescriptor(DescriptorSetID descriptorSetID, u32 bindingIndex, TextureID textureID, u32 mipLevel, DescriptorType type, u32 frameIndex)
    {
        VkImageView imageView = _textureHandler->GetImageView(textureID, mipLevel);
        _descriptorHandler->BindDescriptor(descriptorSetID, bindingIndex, imageView, type, false, frameIndex);
    }

    void RendererVK::BindDescriptorArray(DescriptorSetID descriptorSetID, u32 bindingIndex, TextureID textureID, u32 mipLevel, u32 mipCount, DescriptorType type, u32 frameIndex)
    {
        std::vector<VkImageView> imageViews;
        for (u32 i = 0; i < mipCount; i++)
        {
            u32 mip = mipLevel + i;
            VkImageView imageView = _textureHandler->GetImageView(textureID, mip);
            imageViews.push_back(imageView);
        }

        _descriptorHandler->BindDescriptorArray(descriptorSetID, bindingIndex, imageViews, 0, type, false, frameIndex);
    }

    void RendererVK::BindDescriptor(DescriptorSetID descriptorSetID, u32 bindingIndex, TextureArrayID textureArrayID)
    {
        _descriptorHandler->BindDescriptor(descriptorSetID, bindingIndex, textureArrayID);
    }

    u32 RendererVK::AddTextureToArray(TextureID textureID, TextureArrayID textureArrayID)
    {
        return _textureHandler->AddTextureToArray(textureID, textureArrayID);
    }

    void RendererVK::FlushTextureArrayDescriptors(TextureArrayID textureArrayID)
    {
        _textureHandler->FlushTextureArrayDescriptors(textureArrayID);
    }

    TextureBaseDesc RendererVK::GetDesc(TextureID textureID)
    {
        return _textureHandler->GetTextureDesc(textureID);
    }

    const ImageDesc& RendererVK::GetDesc(ImageID ID)
    {
        return _imageHandler->GetImageDesc(ID);
    }

    const DepthImageDesc& RendererVK::GetDesc(DepthImageID ID)
    {
        return _imageHandler->GetImageDesc(ID);
    }

    const ComputePipelineDesc& RendererVK::GetDesc(ComputePipelineID ID)
    {
        return _pipelineHandler->GetDesc(ID);
    }

    const GraphicsPipelineDesc& RendererVK::GetDesc(GraphicsPipelineID ID)
    {
        return _pipelineHandler->GetDesc(ID);
    }

    const ComputeShaderDesc& RendererVK::GetDesc(ComputeShaderID ID)
    {
        return _shaderHandler->GetDesc(ID);
    }

    const VertexShaderDesc& RendererVK::GetDesc(VertexShaderID ID)
    {
        return _shaderHandler->GetDesc(ID);
    }

    const PixelShaderDesc& RendererVK::GetDesc(PixelShaderID ID)
    {
        return _shaderHandler->GetDesc(ID);
    }

    static VmaBudget sBudgets[16] = { { 0 } };

    void RendererVK::FlushGPU()
    {
        _device->FlushGPU();
    }

    f32 RendererVK::FlipFrame(u32 frameIndex)
    {
        ZoneScopedC(tracy::Color::Red3);
        _frameIndex = frameIndex;

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

            //_pipelineHandler->DiscardPipelines();
            //CreateDummyPipeline();

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

    u32 RendererVK::GetCurrentFrameIndex()
    {
        return _frameIndex;
    }

    u32 RendererVK::GetFrameIndexCount()
    {
        return _device->FRAME_INDEX_COUNT;
    }

    void RendererVK::ResetTimeQueries(u32 frameIndex)
    {
        _frameTimeQueries.clear();
    }

    TextureID RendererVK::GetTextureID(TextureArrayID textureArrayID, u32 index)
    {
        return _textureHandler->GetTextureIDInArray(textureArrayID, index);
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

    void RendererVK::BeginRenderPass(CommandListID commandListID, const RenderPassDesc& desc)
    {
        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount != 0)
        {
            NC_LOG_CRITICAL("You need to match your BeginRenderPass calls with a EndRenderPass call before beginning another render pass!");
        }
        renderPassOpenCount++;
        _commandListHandler->SetRenderPassOpenCount(commandListID, renderPassOpenCount);

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        // -- Get number of render targets and attachments --
        u8 numAttachments = 0;
        for (int i = 0; i < MAX_RENDER_TARGETS; i++)
        {
            if (desc.renderTargets[i] == ImageMutableResource::Invalid())
                break;
            numAttachments++;
        }

        if (numAttachments > 0)
        {
            if (desc.ResourceToImageID == nullptr ||
                desc.ResourceToDepthImageID == nullptr ||
                desc.MutableResourceToImageID == nullptr ||
                desc.MutableResourceToDepthImageID == nullptr)
            {
                NC_LOG_CRITICAL("Tried to start a renderpass with uninitialized RenderPassDesc, try using RenderGraphResources::InitializeRenderPassDesc!");
            }
        }

        std::vector<VkRenderingAttachmentInfo> colorAttachmentInfos(numAttachments);
        for (int i = 0; i < numAttachments; i++)
        {
            VkRenderingAttachmentInfo& attachmentInfo = colorAttachmentInfos[i];

            ImageID imageID = desc.MutableResourceToImageID(desc.renderTargets[i]);
            bool isSwapchain = _imageHandler->IsSwapChainImage(imageID);
            attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            attachmentInfo.imageView = _imageHandler->GetColorView(imageID);
            attachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
            attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            // Clear values are not needed

            // Manual transition for color attachment
            VkImage image = _imageHandler->GetImage(imageID);
            VkImageLayout oldLayout = isSwapchain ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_GENERAL;
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = nullptr;
            // If the image is not a swapchain image, assume previous writes need to be finished.
            barrier.srcAccessMask = isSwapchain ? 0 : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);
        }

        uvec2 extent = desc.extent;
        if (extent == uvec2(0) && desc.renderTargets[0] != ImageMutableResource::Invalid())
        {
            extent = _imageHandler->GetDimensions(desc.MutableResourceToImageID(desc.renderTargets[0]), 0);
        }

        VkRenderingInfo renderInfo = {};
        renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderInfo.pNext = nullptr;
        renderInfo.renderArea = VkRect2D{ VkOffset2D{desc.offset.x, desc.offset.y}, VkExtent2D{extent.x, extent.y} };
        renderInfo.pColorAttachments = colorAttachmentInfos.data();
        renderInfo.colorAttachmentCount = numAttachments;
        renderInfo.layerCount = 1;
        renderInfo.flags = 0;
        renderInfo.viewMask = 0;

        VkRenderingAttachmentInfo depthAttachmentInfo = {};
        if (desc.depthStencil != DepthImageMutableResource::Invalid())
        {
            DepthImageID depthImageID = desc.MutableResourceToDepthImageID(desc.depthStencil);
            const DepthImageDesc& depthImageDesc = _imageHandler->GetImageDesc(depthImageID);
            depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            depthAttachmentInfo.imageView = _imageHandler->GetDepthView(depthImageID);
            depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
            depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            // Clear values are not needed

            renderInfo.pDepthAttachment = &depthAttachmentInfo;
            if (depthImageDesc.format == DepthImageFormat::D24_UNORM_S8_UINT || depthImageDesc.format == DepthImageFormat::D32_FLOAT_S8X24_UINT)
            {
                renderInfo.pStencilAttachment = &depthAttachmentInfo;
            }

            // Manual transition for depth attachment
            VkImage depthImage = _imageHandler->GetImage(depthImageID);
            VkImageMemoryBarrier depthBarrier = {};
            depthBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            depthBarrier.pNext = nullptr;
            // Wait for both read and write from previous usage.
            depthBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            // Allow the upcoming depth attachment usage to read and write.
            depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            depthBarrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            depthBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            depthBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            depthBarrier.image = depthImage;
            depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthBarrier.subresourceRange.baseMipLevel = 0;
            depthBarrier.subresourceRange.levelCount = 1;
            depthBarrier.subresourceRange.baseArrayLayer = 0;
            depthBarrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &depthBarrier);
        }

        vkCmdBeginRendering(commandBuffer, &renderInfo);
    }

    void RendererVK::EndRenderPass(CommandListID commandListID, const RenderPassDesc& desc)
    {
        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount <= 0)
        {
            NC_LOG_CRITICAL("You tried to call EndRenderPass without first calling BeginRenderPass!");
        }
        renderPassOpenCount--;
        _commandListHandler->SetRenderPassOpenCount(commandListID, renderPassOpenCount);

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        vkCmdEndRendering(commandBuffer);

        for (int i = 0; i < MAX_RENDER_TARGETS; i++)
        {
            if (desc.renderTargets[i] == ImageMutableResource::Invalid())
                break;

            ImageID imageID = desc.MutableResourceToImageID(desc.renderTargets[i]);
            bool isSwapchain = _imageHandler->IsSwapChainImage(imageID);

            // Manual transition for color attachment back to original layout
            VkImage image = _imageHandler->GetImage(imageID);
            VkImageLayout newLayout = isSwapchain ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_GENERAL;
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = nullptr;
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);
        }

        if (desc.depthStencil != DepthImageMutableResource::Invalid())
        {
            DepthImageID depthImageID = desc.MutableResourceToDepthImageID(desc.depthStencil);

            // Manual transition for depth attachment back to read-only layout
            VkImage depthImage = _imageHandler->GetImage(depthImageID);
            VkImageMemoryBarrier depthBarrier = {};
            depthBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            depthBarrier.pNext = nullptr;
            depthBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            depthBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            depthBarrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            depthBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            depthBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            depthBarrier.image = depthImage;
            depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthBarrier.subresourceRange.baseMipLevel = 0;
            depthBarrier.subresourceRange.levelCount = 1;
            depthBarrier.subresourceRange.baseArrayLayer = 0;
            depthBarrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &depthBarrier);
        }
    }

    void RendererVK::BeginRenderPass(CommandListID commandListID, const TextureRenderPassDesc& desc)
    {
        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount != 0)
        {
            NC_LOG_CRITICAL("You need to match your BeginRenderPass calls with a EndRenderPass call before beginning another render pass!");
        }
        renderPassOpenCount++;
        _commandListHandler->SetRenderPassOpenCount(commandListID, renderPassOpenCount);

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        std::vector<VkRenderingAttachmentInfo> colorAttachmentInfos;
        colorAttachmentInfos.reserve(8);
        for (int i = 0; i < MAX_RENDER_TARGETS; i++)
        {
            if (desc.renderTargets[i] == TextureID::Invalid())
                break;

            TextureBaseDesc textureDesc = _textureHandler->GetTextureDesc(desc.renderTargets[i]);

            VkRenderingAttachmentInfo& attachmentInfo = colorAttachmentInfos.emplace_back();

            attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            attachmentInfo.imageView = _textureHandler->GetImageView(desc.renderTargets[i]);
            attachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
            attachmentInfo.loadOp = (desc.clearRenderTargets[i]) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            // Manual transition for color attachment
            VkImage image = _textureHandler->GetImage(desc.renderTargets[i]);
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = nullptr;

            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = textureDesc.mipLevels;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);
        }

        uvec2 extent = desc.extent;
        if (extent == uvec2(0))
        {
            extent = _textureHandler->GetTextureDimensions(desc.renderTargets[0]);
        }

        VkRenderingInfo renderInfo = {};
        renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderInfo.pNext = nullptr;
        renderInfo.renderArea = VkRect2D{ VkOffset2D{desc.offset.x, desc.offset.y}, VkExtent2D{extent.x, extent.y} };
        renderInfo.pColorAttachments = colorAttachmentInfos.data();
        renderInfo.colorAttachmentCount = static_cast<u32>(colorAttachmentInfos.size());
        renderInfo.layerCount = 1;
        renderInfo.flags = 0;
        renderInfo.viewMask = 0;

        vkCmdBeginRendering(commandBuffer, &renderInfo);
    }

    void RendererVK::EndRenderPass(CommandListID commandListID, const TextureRenderPassDesc& desc)
    {
        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount <= 0)
        {
            NC_LOG_CRITICAL("You tried to call EndRenderPass without first calling BeginRenderPass!");
        }
        renderPassOpenCount--;
        _commandListHandler->SetRenderPassOpenCount(commandListID, renderPassOpenCount);

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        vkCmdEndRendering(commandBuffer);

        for (int i = 0; i < MAX_RENDER_TARGETS; i++)
        {
            if (desc.renderTargets[i] == TextureID::Invalid())
                break;

            TextureBaseDesc textureDesc = _textureHandler->GetTextureDesc(desc.renderTargets[i]);

            // Manual transition for color attachment back to original layout
            VkImage image = _textureHandler->GetImage(desc.renderTargets[i]);
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = nullptr;
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;

            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = textureDesc.mipLevels;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);
        }
    }

    void RendererVK::BeginTextureComputeWritePass(CommandListID commandListID, const TextureRenderPassDesc& desc)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        for (int i = 0; i < MAX_RENDER_TARGETS; i++)
        {
            if (desc.renderTargets[i] == TextureID::Invalid())
                break;

            TextureBaseDesc textureDesc = _textureHandler->GetTextureDesc(desc.renderTargets[i]);
            VkImage image = _textureHandler->GetImage(desc.renderTargets[i]);
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = nullptr;

            // Transition from shader-read to general layout.
            barrier.oldLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;

            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = textureDesc.mipLevels;  // Transition all mip levels.
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            // Wait for any previous color attachment writes and compute shader storage writes.
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            // Allow the compute shader to perform both sampled reads and storage writes.
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

            // Make sure to wait on both stages that might have written to the image.
            VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                srcStage,
                dstStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);
        }
    }

    void RendererVK::EndTextureComputeWritePass(CommandListID commandListID, const TextureRenderPassDesc& desc)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        for (int i = 0; i < MAX_RENDER_TARGETS; i++)
        {
            if (desc.renderTargets[i] == TextureID::Invalid())
                break;

            // Retrieve texture description to get the number of mip levels.
            TextureBaseDesc textureDesc = _textureHandler->GetTextureDesc(desc.renderTargets[i]);
            VkImage image = _textureHandler->GetImage(desc.renderTargets[i]);
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = nullptr;

            // Transition from general (compute writes) back to shader-read optimal
            barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;

            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = textureDesc.mipLevels;  // Transition all mip levels
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            // Ensure compute writes are visible before subsequent shader reads.
            barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                srcStage,
                dstStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);
        }
    }

    void RendererVK::BeginPipeline(CommandListID commandListID, GraphicsPipelineID pipelineID)
    {
        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount != 1)
        {
            NC_LOG_CRITICAL("You tried to call BeginPipeline without first calling BeginRenderPass!");
        }

        i8 pipelineOpenCount = _commandListHandler->GetPipelineOpenCount(commandListID);
        if (pipelineOpenCount != 0)
        {
            NC_LOG_CRITICAL("You need to match your BeginPipeline calls with a EndPipeline call before beginning another pipeline!");
        }
        pipelineOpenCount++;
        _commandListHandler->SetPipelineOpenCount(commandListID, pipelineOpenCount);

        VkPipeline pipeline = _pipelineHandler->GetPipeline(pipelineID);
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        // Bind pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        _commandListHandler->SetBoundGraphicsPipeline(commandListID, pipelineID);
    }

    void RendererVK::EndPipeline(CommandListID commandListID, GraphicsPipelineID pipelineID)
    {
        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount != 1)
        {
            NC_LOG_CRITICAL("You tried to call EndPipeline without first calling BeginRenderPass, or did you EndRenderPass before EndPipeline?");
        }

        i8 pipelineOpenCount = _commandListHandler->GetPipelineOpenCount(commandListID);
        if (pipelineOpenCount <= 0)
        {
            NC_LOG_CRITICAL("You tried to call EndPipeline without first calling BeginPipeline!");
        }
        pipelineOpenCount--;
        _commandListHandler->SetPipelineOpenCount(commandListID, pipelineOpenCount);

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        _commandListHandler->SetBoundGraphicsPipeline(commandListID, GraphicsPipelineID::Invalid());
    }

    void RendererVK::BeginPipeline(CommandListID commandListID, ComputePipelineID pipelineID)
    {
        i8 renderPassOpenCount = _commandListHandler->GetRenderPassOpenCount(commandListID);
        if (renderPassOpenCount != 0)
        {
            NC_LOG_CRITICAL("You tried to call BeginPipeline for a ComputePipeline without first calling EndRenderPass");
        }

        i8 pipelineOpenCount = _commandListHandler->GetPipelineOpenCount(commandListID);
        if (pipelineOpenCount != 0)
        {
            NC_LOG_CRITICAL("You need to match your BeginPipeline calls with a EndPipeline call before beginning another pipeline!");
        }
        pipelineOpenCount++;
        _commandListHandler->SetPipelineOpenCount(commandListID, pipelineOpenCount);

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkPipeline pipeline = _pipelineHandler->GetPipeline(pipelineID);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

        _commandListHandler->SetBoundComputePipeline(commandListID, pipelineID);
    }

    void RendererVK::EndPipeline(CommandListID commandListID, ComputePipelineID pipelineID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        i8 pipelineOpenCount = _commandListHandler->GetPipelineOpenCount(commandListID);
        if (pipelineOpenCount <= 0)
        {
            NC_LOG_CRITICAL("You tried to call EndPipeline without first calling BeginPipeline!");
        }
        pipelineOpenCount--;
        _commandListHandler->SetPipelineOpenCount(commandListID, pipelineOpenCount);

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
        vkViewport.y = viewport.topLeftY + viewport.height;
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

        NC_LOG_ERROR("While creating DescriptorSet we encountered binding ({0}) of type ({1}) which did not have a matching BindInfo in the bound shaders", name.c_str(), type);
        return false;
    }

    void RendererVK::RecreateSwapChain(Backend::SwapChainVK* swapChain)
    {
        _device->RecreateSwapChain(_imageHandler, _semaphoreHandler, swapChain);
        
        //_pipelineHandler->DiscardPipelines();
        //CreateDummyPipeline();
        
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

    void RendererVK::BindDescriptorSet(CommandListID commandListID, DescriptorSet* descriptorSet, const TrackedBufferBitSets* bufferPermissions)
    {
        ZoneScopedNC("RendererVK::BindDescriptorSet", tracy::Color::Red3);

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        GraphicsPipelineID graphicsPipelineID = _commandListHandler->GetBoundGraphicsPipeline(commandListID);
        ComputePipelineID computePipelineID = _commandListHandler->GetBoundComputePipeline(commandListID);

        DescriptorSetID descriptorSetID = descriptorSet->GetID();

        VkDescriptorSet vkDescriptorSet = _descriptorHandler->GetVkDescriptorSet(descriptorSetID, _frameIndex);
        u32 slot = static_cast<u32>(descriptorSet->GetSlot());

        if (graphicsPipelineID != GraphicsPipelineID::Invalid())
        {
            /*if (_pipelineHandler->GetNumDescriptorSetLayouts(graphicsPipelineID) <= slot)
            {
                return;
            }*/

            // TODO: Validate buffer permissions

            VkPipelineLayout pipelineLayout = _pipelineHandler->GetPipelineLayout(graphicsPipelineID);

            // Bind descriptor set
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, slot, 1, &vkDescriptorSet, 0, nullptr);
        }
        else if (computePipelineID != ComputePipelineID::Invalid())
        {
            /*if (_pipelineHandler->GetNumDescriptorSetLayouts(computePipelineID) <= slot)
            {
                return;
            }*/

            // TODO: Validate buffer permissions

            VkPipelineLayout pipelineLayout = _pipelineHandler->GetPipelineLayout(computePipelineID);

            // Bind descriptor set
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, slot, 1, &vkDescriptorSet, 0, nullptr);
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

        VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
        VkPipelineLayout layout;

        if (graphicsPipelineID != GraphicsPipelineID::Invalid())
        {
            layout = _pipelineHandler->GetPipelineLayout(graphicsPipelineID);
        }
        else
        {
            ComputePipelineID computePipelineID = _commandListHandler->GetBoundComputePipeline(commandListID);
            if (computePipelineID != ComputePipelineID::Invalid())
            {
                layout = _pipelineHandler->GetPipelineLayout(computePipelineID);
            }
            else
            {
                NC_LOG_CRITICAL("Tried to push constants without a bound graphics or compute pipeline");
                return;
            }
        }
        vkCmdPushConstants(commandBuffer, layout, shaderStageFlags, offset, size, data);
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
                PopMarker(commandListID);
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

            RenderPassDesc renderPassDesc;
            // We define simple passthrough functions here because we don't have a rendergraph that keeps track of resources while presenting
            renderPassDesc.ResourceToImageID = [](ImageResource resource)
            {
                return ImageID(static_cast<ImageResource::type>(resource));
            };

            renderPassDesc.ResourceToDepthImageID = [](DepthImageResource resource)
            {
                return DepthImageID(static_cast<DepthImageResource::type>(resource));
            };

            renderPassDesc.MutableResourceToImageID = [](ImageMutableResource resource)
            {
                return ImageID(static_cast<ImageMutableResource::type>(resource));
            };

            renderPassDesc.MutableResourceToDepthImageID = [](DepthImageMutableResource resource)
            {
                return DepthImageID(static_cast<DepthImageMutableResource::type>(resource));
            };

            // Render targets
            renderPassDesc.renderTargets[0] = ImageMutableResource(static_cast<ImageID::type>(swapChain->imageIDs.Get(frameIndex)));
            ImageBarrier(commandListID, imageID);
            BeginRenderPass(commandListID, renderPassDesc);

            ImageDesc imageDesc = _imageHandler->GetImageDesc(imageID);
            
            std::string componentTypeName = GetTextureTypeName(imageDesc.format);
            u32 componentTypeNameHash = StringUtils::fnv1a_32(componentTypeName.c_str(), componentTypeName.size());
            GraphicsPipelineID pipelineID = GetBlitPipeline(componentTypeNameHash);

            VkPipelineLayout pipelineLayout = _pipelineHandler->GetPipelineLayout(pipelineID);

            if (!_presentDescriptorSet.IsInitialized())
            {
                _presentDescriptorSet.RegisterPipeline(this, pipelineID);
                _presentDescriptorSet.Init(this);
            }

            BeginPipeline(commandListID, pipelineID);

            SetViewport(commandListID, _lastViewport);
            SetScissorRect(commandListID, _lastScissorRect);

            VkSampler vkSampler = _samplerHandler->GetSampler(samplerID);
            VkImageView vkImageView = _imageHandler->GetColorView(imageID);

            DescriptorSetID descriptorSetID = _presentDescriptorSet.GetID();
            _descriptorHandler->BindDescriptor(descriptorSetID, 0, vkSampler, frameIndex);
            _descriptorHandler->BindDescriptor(descriptorSetID, 1, vkImageView, DescriptorType::SampledImage, false, frameIndex);
            
            VkDescriptorSet descriptorSet = _descriptorHandler->GetVkDescriptorSet(descriptorSetID, _frameIndex);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, DescriptorSetSlot::PER_PASS, 1, &descriptorSet, 0, nullptr);

            struct BlitConstant
            {
                vec4 colorMultiplier;
                vec4 additiveColor;
                vec4 uvOffsetAndExtent;
                u32 channelRedirectors;
            } blitConstant;
            blitConstant.colorMultiplier = vec4(1, 1, 1, 1);
            blitConstant.additiveColor = vec4(0, 0, 0, 0);
            blitConstant.uvOffsetAndExtent = vec4(0.0f, 0.0f, 1.0f, 1.0f);
            blitConstant.channelRedirectors = 0;
            blitConstant.channelRedirectors |= (1 << 8);
            blitConstant.channelRedirectors |= (2 << 16);
            blitConstant.channelRedirectors |= (3 << 24);

            vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(BlitConstant), &blitConstant);

            vkCmdDraw(commandBuffer, 3, 1, 0, 0);

            EndPipeline(commandListID, pipelineID);
            EndRenderPass(commandListID, renderPassDesc);

#if TRACY_ENABLE
            tracyScope->End();
            tracyScope = nullptr;
#endif
            PopMarker(commandListID);
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

    u64 RendererVK::GetImguiTextureID(TextureID textureID)
    {
        return reinterpret_cast<u64>(_textureHandler->GetImguiTextureID(textureID));
    }

    u64 RendererVK::GetImguiTextureID(ImageID imageID)
    {
        return reinterpret_cast<u64>(_imageHandler->GetImguiTextureID(imageID));
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
