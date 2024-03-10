#pragma once
#include <Renderer/Renderer.h>

#include <Base/Memory/StackAllocator.h>

#include <array>
#include <mutex>

struct VkDescriptorSetLayoutBinding;

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class BufferHandlerVK;
        class ImageHandlerVK;
        class TextureHandlerVK;
        class ShaderHandlerVK;
        class PipelineHandlerVK;
        class CommandListHandlerVK;
        class SamplerHandlerVK;
        class SemaphoreHandlerVK;
        class UploadBufferHandlerVK;
        class TimeQueryHandlerVK;
        struct BindInfo;
        class DescriptorSetBuilderVK;
        struct SwapChainVK;
        class DescriptorSetBuilderVK;
    }
    
    class RendererVK : public Renderer
    {
    public:
        RendererVK(Novus::Window* window);

        void InitDebug() override;
        void InitWindow(Novus::Window* window) override;
        void Deinit() override;

        void SetShaderSourceDirectory(const std::string& path) override;
        void ReloadShaders(bool forceRecompileAll) override;
        void ClearUploadBuffers() override;

        void SetRenderSize(vec2 renderSize) override;
        vec2 GetRenderSize() override;

        vec2 GetWindowSize() override;

        // Creation
        [[nodiscard]] BufferID CreateBuffer(BufferDesc& desc) override;
        [[nodiscard]] BufferID CreateTemporaryBuffer(BufferDesc& desc, u32 framesLifetime) override;
        void QueueDestroyBuffer(BufferID buffer) override;
        void DestroyBuffer(BufferID buffer) override;

        [[nodiscard]] ImageID CreateImage(ImageDesc& desc) override;
        [[nodiscard]] DepthImageID CreateDepthImage(DepthImageDesc& desc) override;

        [[nodiscard]] SamplerID CreateSampler(SamplerDesc& desc) override;
        [[nodiscard]] SemaphoreID CreateNSemaphore() override;

        [[nodiscard]] GraphicsPipelineID CreatePipeline(GraphicsPipelineDesc& desc) override;
        [[nodiscard]] ComputePipelineID CreatePipeline(ComputePipelineDesc& desc) override;

        [[nodiscard]] TextureArrayID CreateTextureArray(TextureArrayDesc& desc) override;

        [[nodiscard]] TextureID CreateDataTexture(DataTextureDesc& desc) override;
        [[nodiscard]] TextureID CreateDataTextureIntoArray(DataTextureDesc& desc, TextureArrayID textureArray, u32& arrayIndex) override;

        [[nodiscard]] TimeQueryID CreateTimeQuery(TimeQueryDesc& desc) override;

        // Loading
        [[nodiscard]] TextureID LoadTexture(TextureDesc& desc) override;
        [[nodiscard]] TextureID LoadTextureIntoArray(TextureDesc& desc, TextureArrayID textureArray, u32& arrayIndex, bool allowDuplicates = false) override;

        [[nodiscard]] VertexShaderID LoadShader(VertexShaderDesc& desc) override;
        [[nodiscard]] PixelShaderID LoadShader(PixelShaderDesc& desc) override;
        [[nodiscard]] ComputeShaderID LoadShader(ComputeShaderDesc& desc) override;

        // Unloading
        void UnloadTexture(TextureID textureID) override;
        void UnloadTexturesInArray(TextureArrayID textureArrayID, u32 unloadStartIndex) override;
        
        // Misc
        u32 AddTextureToArray(TextureID textureID, TextureArrayID textureArrayID) override;

        // Command List Functions
        [[nodiscard]] CommandListID BeginCommandList() override;
        void EndCommandList(CommandListID commandListID) override;

        void Clear(CommandListID commandListID, ImageID image, Color color) override;
        void Clear(CommandListID commandListID, ImageID image, uvec4 values) override;
        void Clear(CommandListID commandListID, ImageID image, ivec4 values) override;
        void Clear(CommandListID commandListID, DepthImageID image, DepthClearFlags clearFlags, f32 depth, u8 stencil) override;

        void Draw(CommandListID commandListID, u32 numVertices, u32 numInstances, u32 vertexOffset, u32 instanceOffset) override;
        void DrawIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, u32 drawCount) override;
        void DrawIndirectCount(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, BufferID drawCountBuffer, u32 drawCountBufferOffset, u32 maxDrawCount) override;
        void DrawIndexed(CommandListID commandListID, u32 numIndices, u32 numInstances, u32 indexOffset, u32 vertexOffset, u32 instanceOffset) override;
        void DrawIndexedIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, u32 drawCount) override;
        void DrawIndexedIndirectCount(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, BufferID drawCountBuffer, u32 drawCountBufferOffset, u32 maxDrawCount) override;
        void Dispatch(CommandListID commandListID, u32 threadGroupCountX, u32 threadGroupCountY, u32 threadGroupCountZ) override;
        void DispatchIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset) override;

        void PopMarker(CommandListID commandListID) override;
        void PushMarker(CommandListID commandListID, Color color, std::string name) override;

        void BeginPipeline(CommandListID commandListID, GraphicsPipelineID pipeline) override;
        void EndPipeline(CommandListID commandListID, GraphicsPipelineID pipeline) override;
        void BeginPipeline(CommandListID commandListID, ComputePipelineID pipeline) override;
        void EndPipeline(CommandListID commandListID, ComputePipelineID pipeline) override;

        void BeginTimeQuery(CommandListID commandListID, TimeQueryID timeQueryID) override;
        void EndTimeQuery(CommandListID commandListID, TimeQueryID timeQueryID) override;

        void SetDepthBias(CommandListID commandListID, DepthBias depthBias) override;
        void SetScissorRect(CommandListID commandListID, ScissorRect scissorRect) override;
        void SetViewport(CommandListID commandListID, Viewport viewport) override;
        void SetVertexBuffer(CommandListID commandListID, u32 slot, BufferID bufferID) override;
        void SetIndexBuffer(CommandListID commandListID, BufferID bufferID, IndexFormat indexFormat) override;
        void SetBuffer(CommandListID commandListID, u32 slot, BufferID buffer) override;

        void BindDescriptorSet(CommandListID commandListID, DescriptorSetSlot slot, Descriptor* descriptors, u32 numDescriptors, const TrackedBufferBitSets* bufferPermissions) override;

        void MarkFrameStart(CommandListID commandListID, u32 frameIndex) override;
        void BeginTrace(CommandListID commandListID, const tracy::SourceLocationData* sourceLocation) override;
        void EndTrace(CommandListID commandListID) override;

        void AddSignalSemaphore(CommandListID commandListID, SemaphoreID semaphoreID) override;
        void AddWaitSemaphore(CommandListID commandListID, SemaphoreID semaphoreID) override;

        void CopyImage(CommandListID commandListID, ImageID dstImageID, uvec2 dstPos, u32 dstMipLevel, ImageID srcImageID, uvec2 srcPos, u32 srcMipLevel, uvec2 size) override;
        void CopyImage(CommandListID commandListID, DepthImageID dstImageID, uvec2 dstPos, DepthImageID srcImageID, uvec2 srcPos, uvec2 size) override;
        void CopyBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range) override;

        void ImageBarrier(CommandListID commandListID, ImageID imageID) override;
        void ImageBarrier(CommandListID commandListID, DepthImageID imageID) override;
        void BufferBarrier(CommandListID commandListID, BufferID bufferID, BufferPassUsage from) override;

        void PushConstant(CommandListID commandListID, void* data, u32 offset, u32 size) override;
        void FillBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, u64 size, u32 data) override;
        void UpdateBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, u64 size, void* data) override;

        // Present functions
        void Present(Novus::Window* window, ImageID image, SemaphoreID semaphoreID = SemaphoreID::Invalid()) override;
        void Present(Novus::Window* window, DepthImageID image, SemaphoreID semaphoreID = SemaphoreID::Invalid()) override;

        // Staging and memory
        [[nodiscard]] std::shared_ptr<UploadBuffer> CreateUploadBuffer(BufferID targetBuffer, size_t targetOffset, size_t size) override;
        [[nodiscard]] bool ShouldWaitForUpload() override;
        void SetHasWaitedForUpload() override;
        [[nodiscard]] SemaphoreID GetUploadFinishedSemaphore() override;

        // Uses the upload handler to schedule it for next frames command list
        void CopyBuffer(BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range) override;
        // Immediately copies using an immediate command list
        void CopyBufferImmediate(BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range) override;

        [[nodiscard]] void* MapBuffer(BufferID buffer) override;
        void UnmapBuffer(BufferID buffer) override;

        // Time Queries
        [[nodiscard]] const std::string& GetTimeQueryName(TimeQueryID id) override;
        [[nodiscard]] f32 GetLastTimeQueryDuration(TimeQueryID id) override;

        // Utils
        f32 FlipFrame(u32 frameIndex) override;
        void ResetTimeQueries(u32 frameIndex) override;

        [[nodiscard]] TextureID GetTextureID(TextureArrayID textureArrayID, u32 index) override;
        [[nodiscard]] i32 GetTextureHeight(TextureID textureID) override;
        [[nodiscard]] i32 GetTextureWidth(TextureID textureID) override;

        [[nodiscard]] const ImageDesc& GetImageDesc(ImageID ID) override;
        [[nodiscard]] const DepthImageDesc& GetImageDesc(DepthImageID ID) override;

        [[nodiscard]] uvec2 GetImageDimensions(const ImageID id, u32 mipLevel = 0) override;
        [[nodiscard]] uvec2 GetImageDimensions(const DepthImageID id) override;

        [[nodiscard]] std::string GetBufferName(BufferID id) override;

        [[nodiscard]] const std::string& GetGPUName() override;
        [[nodiscard]] const std::string& GetDebugName(const TextureID textureID) override;

        [[nodiscard]] size_t GetVRAMUsage() override;
        [[nodiscard]] size_t GetVRAMBudget() override;

        [[nodiscard]] u32 GetNumImages() override;
        [[nodiscard]] u32 GetNumDepthImages() override;
        [[nodiscard]] u32 GetNumBuffers() override;

        void InitImgui() override;
        void DrawImgui(CommandListID commandListID) override;
        void* GetImguiImageHandle(TextureID textureID) override;
        void* GetImguiImageHandle(ImageID imageID) override;

        bool HasExtendedTextureSupport() override;

    private:
        [[nodiscard]] bool ReflectDescriptorSet(const std::string& name, u32 nameHash, u32 type, i32& set, const std::vector<Backend::BindInfo>& bindInfos, u32& outBindInfoIndex, VkDescriptorSetLayoutBinding* outDescriptorLayoutBinding);
        void BindDescriptor(Backend::DescriptorSetBuilderVK& builder, Descriptor& descriptor);

        void RecreateSwapChain(Backend::SwapChainVK* swapChain);
        void CreateDummyPipeline();

    private:
        Backend::RenderDeviceVK* _device = nullptr;
        Backend::BufferHandlerVK* _bufferHandler = nullptr;
        Backend::ImageHandlerVK* _imageHandler = nullptr;
        Backend::TextureHandlerVK* _textureHandler = nullptr;
        Backend::ShaderHandlerVK* _shaderHandler = nullptr;
        Backend::PipelineHandlerVK* _pipelineHandler = nullptr;
        Backend::CommandListHandlerVK* _commandListHandler = nullptr;
        Backend::SamplerHandlerVK* _samplerHandler = nullptr;
        Backend::SemaphoreHandlerVK* _semaphoreHandler = nullptr;
        Backend::UploadBufferHandlerVK* _uploadBufferHandler = nullptr;
        Backend::TimeQueryHandlerVK* _timeQueryHandler = nullptr;

        Memory::StackAllocator _frameAllocator;

        GraphicsPipelineID _globalDummyPipeline = GraphicsPipelineID::Invalid();
        //Backend::DescriptorSetBuilderVK* _descriptorSetBuilder = nullptr;

        Viewport _lastViewport;
        ScissorRect _lastScissorRect;

        bool _renderSizeChanged = false;
        vec2 _renderSize = vec2(1, 1);

        std::mutex _destroyListMutex;

        struct ObjectDestroyList
        {
            std::vector<BufferID> buffers;
        };

        std::array<ObjectDestroyList, 4> _destroyLists;
        size_t _destroyListIndex = 0;

        void DestroyObjects(ObjectDestroyList& destroyList);
    };
}
