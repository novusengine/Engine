#pragma once
#include <Base/Types.h>

#include "DescriptorSet.h"

// Descriptors
#include "Descriptors/BufferDesc.h"
#include "Descriptors/CommandListDesc.h"
#include "Descriptors/VertexShaderDesc.h"
#include "Descriptors/PixelShaderDesc.h"
#include "Descriptors/ComputeShaderDesc.h"
#include "Descriptors/GraphicsPipelineDesc.h"
#include "Descriptors/ComputePipelineDesc.h"
#include "Descriptors/ImageDesc.h"
#include "Descriptors/DepthImageDesc.h"
#include "Descriptors/TextureDesc.h"
#include "Descriptors/TextureArrayDesc.h"
#include "Descriptors/SamplerDesc.h"
#include "Descriptors/SemaphoreDesc.h"
#include "Descriptors/UploadBuffer.h"
#include "Descriptors/TimeQueryDesc.h"

namespace Novus
{
    class Window;
}

namespace tracy
{
    struct SourceLocationData;
}

namespace Memory
{
    class Allocator;
}

namespace Renderer
{
    class RenderGraph;
    struct RenderGraphDesc;
    class TrackedBufferBitSets;

    class Renderer
    {
    public:
        virtual void InitDebug() = 0;
        virtual void InitWindow(Novus::Window* window) = 0;
        virtual void Deinit() = 0;

        virtual void SetShaderSourceDirectory(const std::string& path) = 0;
        virtual void ReloadShaders(bool forceRecompileAll) = 0;
        virtual void ClearUploadBuffers() = 0;

        virtual void SetRenderSize(vec2 renderSize) = 0;
        virtual vec2 GetRenderSize() = 0;

        virtual vec2 GetWindowSize() = 0;

        virtual ~Renderer();

        [[nodiscard]] RenderGraph& CreateRenderGraph(RenderGraphDesc& desc);

        // Creation
        virtual BufferID CreateBuffer(BufferDesc& desc) = 0;
        virtual BufferID CreateTemporaryBuffer(BufferDesc& desc, u32 framesLifetime) = 0;
        virtual void QueueDestroyBuffer(BufferID buffer) = 0;
        virtual void DestroyBuffer(BufferID buffer) = 0;

        virtual ImageID CreateImage(ImageDesc& desc) = 0;
        virtual DepthImageID CreateDepthImage(DepthImageDesc& desc) = 0;

        virtual SamplerID CreateSampler(SamplerDesc& sampler) = 0;
        virtual SemaphoreID CreateNSemaphore() = 0;

        virtual GraphicsPipelineID CreatePipeline(GraphicsPipelineDesc& desc) = 0;
        virtual ComputePipelineID CreatePipeline(ComputePipelineDesc& desc) = 0;

        virtual TextureArrayID CreateTextureArray(TextureArrayDesc& desc) = 0;

        virtual TextureID CreateDataTexture(DataTextureDesc& desc) = 0;
        virtual TextureID CreateDataTextureIntoArray(DataTextureDesc& desc, TextureArrayID textureArray, u32& arrayIndex) = 0;

        virtual TimeQueryID CreateTimeQuery(TimeQueryDesc& desc) = 0;

        // Loading
        virtual TextureID LoadTexture(TextureDesc& desc) = 0;
        virtual TextureID LoadTextureIntoArray(TextureDesc& desc, TextureArrayID textureArray, u32& arrayIndex, bool allowDuplicates = false) = 0;

        virtual VertexShaderID LoadShader(VertexShaderDesc& desc) = 0;
        virtual PixelShaderID LoadShader(PixelShaderDesc& desc) = 0;
        virtual ComputeShaderID LoadShader(ComputeShaderDesc& desc) = 0;

        // Unloading
        virtual void UnloadTexture(TextureID textureID) = 0;
        virtual void UnloadTexturesInArray(TextureArrayID textureArrayID, u32 unloadStartIndex) = 0;

        // Misc
        virtual u32 AddTextureToArray(TextureID textureID, TextureArrayID textureArrayID) = 0;

        // Command List Functions
        virtual CommandListID BeginCommandList() = 0;
        virtual void EndCommandList(CommandListID commandListID) = 0;

        virtual void Clear(CommandListID commandListID, ImageID image, Color color) = 0;
        virtual void Clear(CommandListID commandListID, ImageID image, uvec4 values) = 0;
        virtual void Clear(CommandListID commandListID, ImageID image, ivec4 values) = 0;
        virtual void Clear(CommandListID commandListID, DepthImageID image, DepthClearFlags clearFlags, f32 depth, u8 stencil) = 0;

        virtual void Draw(CommandListID commandListID, u32 numVertices, u32 numInstances, u32 vertexOffset, u32 instanceOffset) = 0;
        virtual void DrawIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, u32 drawCount) = 0;
        virtual void DrawIndirectCount(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, BufferID drawCountBuffer, u32 drawCountBufferOffset, u32 maxDrawCount) = 0;
        virtual void DrawIndexed(CommandListID commandListID, u32 numIndices, u32 numInstances, u32 indexOffset, u32 vertexOffset, u32 instanceOffset) = 0;
        virtual void DrawIndexedIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, u32 drawCount) = 0;
        virtual void DrawIndexedIndirectCount(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, BufferID drawCountBuffer, u32 drawCountBufferOffset, u32 maxDrawCount) = 0;

        virtual void Dispatch(CommandListID commandListID, u32 threadGroupCountX, u32 threadGroupCountY, u32 threadGroupCountZ) = 0;
        virtual void DispatchIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset) = 0;

        virtual void PopMarker(CommandListID commandListID) = 0;
        virtual void PushMarker(CommandListID commandListID, Color color, std::string name) = 0;

        virtual void BeginPipeline(CommandListID commandListID, GraphicsPipelineID pipeline) = 0;
        virtual void EndPipeline(CommandListID commandListID, GraphicsPipelineID pipeline) = 0;
        virtual void BeginPipeline(CommandListID commandListID, ComputePipelineID pipeline) = 0;
        virtual void EndPipeline(CommandListID commandListID, ComputePipelineID pipeline) = 0;

        virtual void BeginTimeQuery(CommandListID commandListID, TimeQueryID timeQueryID) = 0;
        virtual void EndTimeQuery(CommandListID commandListID, TimeQueryID timeQueryID) = 0;

        virtual void SetDepthBias(CommandListID commandListID, DepthBias depthBias) = 0;
        virtual void SetScissorRect(CommandListID commandListID, ScissorRect scissorRect) = 0;
        virtual void SetViewport(CommandListID commandListID, Viewport viewport) = 0;
        virtual void SetVertexBuffer(CommandListID commandListID, u32 slot, BufferID bufferID) = 0;
        virtual void SetIndexBuffer(CommandListID commandListID, BufferID bufferID, IndexFormat indexFormat) = 0;
        virtual void SetBuffer(CommandListID commandListID, u32 slot, BufferID buffer) = 0;

        virtual void BindDescriptorSet(CommandListID commandListID, DescriptorSetSlot slot, Descriptor* descriptors, u32 numDescriptors, const TrackedBufferBitSets* bufferPermissions) = 0;

        virtual void MarkFrameStart(CommandListID commandListID, u32 frameIndex) = 0;
        virtual void BeginTrace(CommandListID commandListID, const tracy::SourceLocationData* sourceLocation) = 0;
        virtual void EndTrace(CommandListID commandListID) = 0;

        virtual void AddSignalSemaphore(CommandListID commandListID, SemaphoreID semaphoreID) = 0;
        virtual void AddWaitSemaphore(CommandListID commandListID, SemaphoreID semaphoreID) = 0;

        virtual void CopyImage(CommandListID commandListID, ImageID dstImageID, uvec2 dstPos, u32 dstMipLevel, ImageID srcImageID, uvec2 srcPos, u32 srcMipLevel, uvec2 size) = 0;
        virtual void CopyImage(CommandListID commandListID, DepthImageID dstImageID, uvec2 dstPos, DepthImageID srcImageID, uvec2 srcPos, uvec2 size) = 0;
        virtual void CopyBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range) = 0;

        virtual void ImageBarrier(CommandListID commandListID, ImageID imageID) = 0;
        virtual void ImageBarrier(CommandListID commandListID, DepthImageID imageID) = 0;
        virtual void BufferBarrier(CommandListID commandListID, BufferID bufferID, BufferPassUsage from) = 0;

        virtual void PushConstant(CommandListID commandListID, void* data, u32 offset, u32 size) = 0;
        virtual void FillBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, u64 size, u32 data) = 0;
        virtual void UpdateBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, u64 size, void* data) = 0;

        // Present functions
        virtual void Present(Novus::Window* window, ImageID image, SemaphoreID semaphoreID = SemaphoreID::Invalid()) = 0;
        virtual void Present(Novus::Window* window, DepthImageID image, SemaphoreID semaphoreID = SemaphoreID::Invalid()) = 0;

        // Staging and memory
        virtual std::shared_ptr<UploadBuffer> CreateUploadBuffer(BufferID targetBuffer, size_t targetOffset, size_t size) = 0;
        virtual bool ShouldWaitForUpload() = 0;
        virtual void SetHasWaitedForUpload() = 0;
        virtual SemaphoreID GetUploadFinishedSemaphore() = 0;

        [[nodiscard]] BufferID CreateBuffer(BufferID bufferID, BufferDesc& desc);
        [[nodiscard]] BufferID CreateAndFillBuffer(BufferID bufferID, BufferDesc desc, void* data, size_t dataSize); // Deletes the current BufferID if it's not invalid
        [[nodiscard]] BufferID CreateAndFillBuffer(BufferDesc desc, void* data, size_t dataSize);
        [[nodiscard]] BufferID CreateAndFillBuffer(BufferID bufferID, BufferDesc desc, const std::function<void(void*, size_t)>& callback); // Deletes the current BufferID if it's not invalid
        [[nodiscard]] BufferID CreateAndFillBuffer(BufferDesc desc, const std::function<void(void*, size_t)>& callback);

        // Uses the upload handler to schedule it for next frames command list
        virtual void CopyBuffer(BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range) = 0;
        // Immediately copies using an immediate command list
        virtual void CopyBufferImmediate(BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range) = 0;
        void UploadToBuffer(BufferID dstBuffer, u64 dstOffset, void* srcData, u64 srcOffset, u64 srcSize);

        virtual void* MapBuffer(BufferID buffer) = 0;
        virtual void UnmapBuffer(BufferID buffer) = 0;

        // Time Queries
        virtual const std::string& GetTimeQueryName(TimeQueryID id) = 0;
        virtual f32 GetLastTimeQueryDuration(TimeQueryID id) = 0;

        const std::vector<TimeQueryID>& GetFrameTimeQueries() { return _frameTimeQueries; }

        // Utils
        virtual f32 FlipFrame(u32 frameIndex) = 0; // Returns time waited in seconds
        virtual void ResetTimeQueries(u32 frameIndex) = 0;

        virtual TextureID GetTextureID(TextureArrayID textureArrayID, u32 index) = 0;
        virtual i32 GetTextureHeight(TextureID textureID) = 0;
        virtual i32 GetTextureWidth(TextureID textureID) = 0;

        virtual const ImageDesc& GetImageDesc(ImageID ID) = 0;
        virtual const DepthImageDesc& GetImageDesc(DepthImageID ID) = 0;

        virtual uvec2 GetImageDimensions(const ImageID id, u32 mipLevel = 0) = 0;
        virtual uvec2 GetImageDimensions(const DepthImageID id) = 0;

        virtual std::string GetBufferName(BufferID id) = 0;

        virtual const std::string& GetGPUName() = 0;
        virtual const std::string& GetDebugName(const TextureID textureID) = 0;

        virtual size_t GetVRAMUsage() = 0;
        virtual size_t GetVRAMBudget() = 0;

        virtual u32 GetNumImages() = 0;
        virtual u32 GetNumDepthImages() = 0;
        virtual u32 GetNumBuffers() = 0;

        virtual void InitImgui() = 0;
        virtual void DrawImgui(CommandListID commandListID) = 0;
        virtual void* GetImguiImageHandle(TextureID textureID) = 0;
        virtual void* GetImguiImageHandle(ImageID imageID) = 0;

        virtual bool HasExtendedTextureSupport() = 0;

    protected:
        Renderer() {}; // Pure virtual class, disallow creation of it

        void BeginExecutingCommandlist() { _isExecutingCommandlist = true; };
        void EndExecutingCommandlist() { _isExecutingCommandlist = false; };

        bool _isExecutingCommandlist = false;

        std::vector<TimeQueryID> _frameTimeQueries;

        friend class RenderGraph;
    };
}
