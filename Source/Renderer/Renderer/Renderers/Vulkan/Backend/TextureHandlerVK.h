#pragma once
#include "Renderer/Descriptors/DescriptorSetDesc.h"
#include "Renderer/Descriptors/SamplerDesc.h"
#include "Renderer/Descriptors/TextureDesc.h"
#include "Renderer/Descriptors/TextureArrayDesc.h"
#include "Renderer/RenderStates.h"

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
        class DescriptorHandlerVK;
        struct Texture;

        struct ITextureHandlerVKData {};

        class TextureHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device, BufferHandlerVK* bufferHandler, DescriptorHandlerVK* descriptorHandler, UploadBufferHandlerVK* uploadBufferHandler, SamplerHandlerVK* samplerHandler);
            void InitDebugTexture();

            void FlipFrame(u32 frameIndex);

            void RegisterTextureArrayBinding(TextureArrayID textureArrayID, DescriptorSetID descriptorSetID, u32 bindingIndex);
            void FlushTextureArrayDescriptors(TextureArrayID textureArrayID);

            TextureID LoadTexture(const TextureDesc& desc);
            TextureID LoadTextureIntoArray(const TextureDesc& desc, TextureArrayID textureArrayID, u32& arrayIndex, bool allowDuplicates);

            void UnloadTexture(const TextureID textureID);
            void UnloadTexturesInArray(const TextureArrayID textureArrayID, u32 unloadStartIndex);

            TextureArrayID CreateTextureArray(const TextureArrayDesc& desc);

            TextureID CreateDataTexture(const DataTextureDesc& desc);
            TextureID CreateDataTextureIntoArray(const DataTextureDesc& desc, TextureArrayID textureArrayID, u32& arrayIndex);

            u32 AddTextureToArray(const TextureID textureID, const TextureArrayID textureArrayID);

            void CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, size_t srcOffset, TextureID dstTextureID);
            void TransitionImageLayout(VkCommandBuffer commandBuffer, TextureID textureID, VkImageAspectFlags aspects, VkImageLayout oldLayout, VkImageLayout newLayout);

            TextureID GetTextureIDInArray(const TextureArrayID textureArrayID, u32 index);
            TextureBaseDesc GetTextureDesc(const TextureID textureID);

            const SafeVector<TextureID>& GetTextureIDsInArray(const TextureArrayID textureArrayID);

            bool IsOnionTexture(const TextureID textureID);

            VkImage GetImage(const TextureID textureID);
            VkImageView GetImageView(const TextureID textureID);
            VkImageView GetImageView(const TextureID textureID, u32 mipLevel);
            VkImageView GetDebugTextureImageView();
            VkImageView GetDebugOnionTextureImageView();

            VkDescriptorSet GetImguiTextureID(const TextureID textureID);

            size_t GetTextureUploadSize(const TextureID textureID);
            size_t GetTextureTotalSize(const TextureID textureID);
            u32 GetTextureArraySize(const TextureArrayID textureArrayID);

            ivec2 GetTextureDimensions(const TextureID textureID);

            const std::string& GetDebugName(const TextureID textureID);

        private:
            u64 CalculateDescHash(const TextureDesc& desc);
            bool TryFindExistingTexture(u64 descHash, size_t& id);
            bool TryFindExistingTextureInArray(TextureArrayID textureArrayID, u64 descHash, size_t& arrayIndex, TextureID& textureID);

            void LoadFile(const std::string& filename, Texture& texture, TextureID textureID);
            void CreateTexture(Texture& texture);

            u32 AddTextureToArrayInternal(const TextureID textureID, const TextureArrayID textureArrayID, u64 hash, bool hasOwnership);

        private:
            ITextureHandlerVKData* _data;

            RenderDeviceVK* _device;
            BufferHandlerVK* _bufferHandler;
            DescriptorHandlerVK* _descriptorHandler = nullptr;
            UploadBufferHandlerVK* _uploadBufferHandler;
            SamplerHandlerVK* _samplerHandler;

            SamplerID _imguiSampler;
            TextureID _debugTexture;
            TextureID _debugOnionTexture; // "TextureArrays" using texture layers rather than arrays of descriptors are now called Onion Textures to make it possible to differentiate between them...
        };
    }
}