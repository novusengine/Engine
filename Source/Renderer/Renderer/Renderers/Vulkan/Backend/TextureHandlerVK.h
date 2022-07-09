#pragma once
#include "Renderer/Descriptors/SamplerDesc.h"
#include "Renderer/Descriptors/TextureDesc.h"
#include "Renderer/Descriptors/TextureArrayDesc.h"

#include <Base/Types.h>

#include <vulkan/vulkan_core.h>

template<typename T>
class SafeVector;

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class BufferHandlerVK;
        class UploadBufferHandlerVK;
        class SamplerHandlerVK;
        struct Texture;

        struct ITextureHandlerVKData {};

        class TextureHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device, BufferHandlerVK* bufferHandler, UploadBufferHandlerVK* uploadBufferHandler, SamplerHandlerVK* samplerHandler);
            void InitDebugTexture();

            void FlipFrame(u32 frameIndex);

            TextureID LoadTexture(const TextureDesc& desc);
            TextureID LoadTextureIntoArray(const TextureDesc& desc, TextureArrayID textureArrayID, u32& arrayIndex, bool allowDuplicates);

            void UnloadTexture(const TextureID textureID);
            void UnloadTexturesInArray(const TextureArrayID textureArrayID, u32 unloadStartIndex);

            TextureArrayID CreateTextureArray(const TextureArrayDesc& desc);

            TextureID CreateDataTexture(const DataTextureDesc& desc);
            TextureID CreateDataTextureIntoArray(const DataTextureDesc& desc, TextureArrayID textureArrayID, u32& arrayIndex);

            void CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, size_t srcOffset, TextureID dstTextureID);
            void TransitionImageLayout(VkCommandBuffer commandBuffer, TextureID textureID, VkImageAspectFlags aspects, VkImageLayout oldLayout, VkImageLayout newLayout);

            TextureID GetTextureIDInArray(const TextureArrayID textureArrayID, u32 index);
            const SafeVector<TextureID>& GetTextureIDsInArray(const TextureArrayID textureArrayID);

            bool IsOnionTexture(const TextureID textureID);

            VkImageView GetImageView(const TextureID textureID);
            VkImageView GetDebugTextureImageView();
            VkImageView GetDebugOnionTextureImageView();

            VkDescriptorSet GetImguiImageHandle(const TextureID textureID);

            size_t GetTextureSize(const TextureID textureID);
            u32 GetTextureArraySize(const TextureArrayID textureArrayID);

            const std::string& GetDebugName(const TextureID textureID);

        private:
            u64 CalculateDescHash(const TextureDesc& desc);
            bool TryFindExistingTexture(u64 descHash, size_t& id);
            bool TryFindExistingTextureInArray(TextureArrayID textureArrayID, u64 descHash, size_t& arrayIndex, TextureID& textureID);

            void LoadFile(const std::string& filename, Texture& texture, TextureID textureID);
            void CreateTexture(Texture& texture);

        private:
            ITextureHandlerVKData* _data;

            RenderDeviceVK* _device;
            BufferHandlerVK* _bufferHandler;
            UploadBufferHandlerVK* _uploadBufferHandler;
            SamplerHandlerVK* _samplerHandler;

            SamplerID _imguiSampler;
            TextureID _debugTexture;
            TextureID _debugOnionTexture; // "TextureArrays" using texture layers rather than arrays of descriptors are now called Onion Textures to make it possible to differentiate between them...
        };
    }
}