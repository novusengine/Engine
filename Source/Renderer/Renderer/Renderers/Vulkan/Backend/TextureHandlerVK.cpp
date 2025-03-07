#include "TextureHandlerVK.h"

#include "RenderDeviceVK.h"
#include "FormatConverterVK.h"
#include "DebugMarkerUtilVK.h"
#include "BufferHandlerVK.h"
#include "UploadBufferHandlerVK.h"
#include "SamplerHandlerVK.h"

#include "vk_mem_alloc.h"
#include "vkformat/vk_format.h"
#include "vk_format_utils.h"
#define STB_IMAGE_IMPLEMENTATION
#if __GNUC__
#define STBI_NO_SIMD
#endif
#include "stb_image.h"

#include <Base/Math/Math.h>
#include <Base/Memory/FileReader.h>
#include <Base/Container/SafeVector.h>
#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>
#include <Base/Util/XXHash64.h>

#include <vulkan/vulkan.h>
#include <gli/gli.hpp>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <tracy/Tracy.hpp>
#include <robinhood/robinhood.h>

#include <vector>
#include <queue>
#include <filesystem>

namespace Renderer
{
    namespace Backend
    {
        struct Texture
        {
            bool loaded = true;
            u64 hash;

            TextureID::type textureIndex;

            TextureBaseDesc desc;
            i32 numGeneratedMipLevels = 0;

            size_t uploadSize;
            size_t totalSize; // Including any possibly generated mips

            VmaAllocation allocation;
            VkImage image;
            VkImageView imageView;
            std::vector<VkImageView> mipViews;

            VkDescriptorSet imguiTextureHandle = 0;

            bool layoutUndefined = true;
        };

        struct TextureArray
        {
            u32 size;

            SafeVector<TextureID>* textures = nullptr;
            SafeVector<u64>* textureHashes = nullptr;
            robin_hood::unordered_map<TextureID::type, u32>* textureIDToArrayIndex;
            robin_hood::unordered_set<TextureID::type> ownedTextureIDs;
        };

        struct TextureHandlerVKData : ITextureHandlerVKData
        {
            SafeVector<Texture*> textures;
            std::queue<Texture*> freeTextureQueue;
            std::vector<VkDescriptorSet> descriptorsToFree;

            SafeVector<TextureArray> textureArrays;
        };

        void TextureHandlerVK::Init(RenderDeviceVK* device, BufferHandlerVK* bufferHandler, UploadBufferHandlerVK* uploadBufferHandler, SamplerHandlerVK* samplerHandler)
        {
            _data = new TextureHandlerVKData();
            _device = device;
            _bufferHandler = bufferHandler;
            _uploadBufferHandler = uploadBufferHandler;
            _samplerHandler = samplerHandler;
        }

        void TextureHandlerVK::InitDebugTexture()
        {
            // Create imgui sampler
            SamplerDesc samplerDesc;
            samplerDesc.enabled = true;
            samplerDesc.filter = Renderer::SamplerFilter::MIN_MAG_MIP_LINEAR;
            samplerDesc.addressU = Renderer::TextureAddressMode::WRAP;
            samplerDesc.addressV = Renderer::TextureAddressMode::WRAP;
            samplerDesc.addressW = Renderer::TextureAddressMode::CLAMP;
            samplerDesc.shaderVisibility = Renderer::ShaderVisibility::PIXEL;
            _imguiSampler = _samplerHandler->CreateSampler(samplerDesc);

            // Create 2d debug texture
            DataTextureDesc desc;
            desc.debugName = "DebugTexture";
            desc.format = ImageFormat::R8G8B8A8_UNORM;
            desc.width = 8;
            desc.height = 8;
            desc.layers = 1;

            // 8x8 checkerboard pattern between black and magenta
            desc.data = new u8[4*8*8]
            {
                0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,
                255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,
                0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,
                255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,
                0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,
                255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,
                0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,
                255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255
            };
            
            _debugTexture = CreateDataTexture(desc);
            delete[] desc.data;

            // Create 3d debug texture (onion)
            desc.width = 1;
            desc.height = 1;
            desc.layers = 256;
            desc.format = ImageFormat::R8G8B8A8_UNORM;
            desc.data = new u8[1 * 1 * 256 * 4]{ 1 };

            _debugOnionTexture = CreateDataTexture(desc);
            delete[] desc.data;
        }

        void TextureHandlerVK::FlipFrame(u32 frameIndex)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);

            if (data.descriptorsToFree.size() > 0)
            {
                vkFreeDescriptorSets(_device->_device, _device->_imguiContext->imguiPool, static_cast<u32>(data.descriptorsToFree.size()), data.descriptorsToFree.data());
                data.descriptorsToFree.clear();
            }
        }

        TextureID TextureHandlerVK::LoadTexture(const TextureDesc& desc)
        {
            ZoneScoped;

            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);

            // Check the cache, we only want to do this for LOADED textures though, never CREATED data textures
            size_t nextID;
            u64 cacheDescHash = CalculateDescHash(desc);
            if (TryFindExistingTexture(cacheDescHash, nextID))
            {
                TextureID::type id = static_cast<TextureID::type>(nextID);

                const Texture* texture = nullptr;

                data.textures.ReadLock(
                    [&](const std::vector<Texture*>& textures)
                    {
                        texture = textures[id];
                    });

                if (texture->loaded)
                {
                    return TextureID(id); // We already loaded this texture
                }
            }

            // TODO: Check the clearlist before allocating a new one

            TextureID textureID;
            Texture* texture = new Texture();

            data.textures.WriteLock(
                [&](std::vector<Texture*>& textures)
                {
                    size_t nextHandle = textures.size();

                    // Make sure we haven't exceeded the limit of the ImageID type, if this hits you need to change type of ImageID to something bigger
                    if (nextHandle >= TextureID::MaxValue())
                    {
                        NC_LOG_CRITICAL("We exceeded the limit of the TextureID type!");
                    }

                    textures.push_back(texture);
                    textureID = TextureID(static_cast<TextureID::type>(nextHandle));
                });

            texture->hash = cacheDescHash;
            texture->desc.debugName = desc.path;

            texture->textureIndex = static_cast<TextureID::type>(textureID);
            LoadFile(desc.path, *texture, textureID);

            return textureID;
        }

        TextureID TextureHandlerVK::LoadTextureIntoArray(const TextureDesc& desc, TextureArrayID textureArrayID, u32& arrayIndex, bool allowDuplicates)
        {
            ZoneScoped;

            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureArrayID::type id = static_cast<TextureArrayID::type>(textureArrayID);
            
            if (data.textureArrays.Size() < id)
            {
                NC_LOG_CRITICAL("TextureHandlerVK::LoadTextureIntoArray : Tried to load texture into array ({0}) that doesn't exist", id);
            }

            // Check the cache, we only want to do this for LOADED textures though, never CREATED data textures
            size_t nextID;
            u64 descHash = CalculateDescHash(desc);

            if (descHash == 0) // What are the odds? All data textures has a 0 hash so we don't wanna go ahead with this, figure out why this happens.
            {
                NC_LOG_CRITICAL("Calculated texture descriptor hash was 0, this is a big issue! ({0})", desc.path.c_str());
            }

            TextureID textureID;

            if (!allowDuplicates)
            {
                if (TryFindExistingTextureInArray(textureArrayID, descHash, nextID, textureID))
                {
                    arrayIndex = static_cast<u32>(nextID);
                    return textureID; // This texture already exists in this array
                }
            }

            textureID = LoadTexture(desc);

            arrayIndex = AddTextureToArrayInternal(textureID, textureArrayID, descHash, true);
            
            return textureID;
        }

        void TextureHandlerVK::UnloadTexture(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);

            data.textures.WriteLock(
                [&](std::vector<Texture*>& textures)
                {
                    Texture* texture = textures[static_cast<TextureID::type>(textureID)];

                    if (!texture->loaded)
                    {
                        return;
                    }

                    texture->loaded = false;
                    texture->hash = 0;

                    vmaFreeMemory(_device->_allocator, texture->allocation);
                    vkDestroyImage(_device->_device, texture->image, nullptr);
                    vkDestroyImageView(_device->_device, texture->imageView, nullptr);

                    for (u32 i = 0; i < texture->desc.mipLevels; i++)
                    {
                        vkDestroyImageView(_device->_device, texture->mipViews[i], nullptr);
                    }

                    data.descriptorsToFree.push_back(texture->imguiTextureHandle);
                    texture->imguiTextureHandle = VK_NULL_HANDLE;

                    data.freeTextureQueue.push(texture);
                });
        }

        void TextureHandlerVK::UnloadTexturesInArray(const TextureArrayID textureArrayID, u32 unloadStartIndex)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);

            data.textureArrays.WriteLock(
                [&](std::vector<TextureArray>& textureArrays)
                {
                    TextureArray& textureArray = textureArrays[static_cast<TextureArrayID::type>(textureArrayID)];

                    textureArray.textures->ReadLock(
                        [&](const std::vector<TextureID>& textures) 
                        {
                            for (u32 i = unloadStartIndex; i < textures.size(); i++)
                            {
                                TextureID::type textureIDTyped = static_cast<TextureID::type>(textures[i]);
                                textureArray.textureIDToArrayIndex->erase(textureIDTyped);

                                if (!textureArray.ownedTextureIDs.contains(textureIDTyped))
                                    continue;

                                UnloadTexture(textures[i]);
                                textureArray.ownedTextureIDs.erase(textureIDTyped);
                            }
                        });

                    textureArray.textureHashes->Resize(unloadStartIndex);
                    textureArray.textures->Resize(unloadStartIndex);
                });
        }

        TextureArrayID TextureHandlerVK::CreateTextureArray(const TextureArrayDesc& desc)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            if (desc.size == 0)
            {
                NC_LOG_CRITICAL("Tried to create a texture array with a size of zero!");
            }

            size_t nextHandle;
            data.textureArrays.WriteLock(
                [&](std::vector<TextureArray>& textureArrays)
                {
                    nextHandle = textureArrays.size();
                    TextureArray& textureArray = textureArrays.emplace_back();
                    textureArray.textures = new SafeVector<TextureID>();
                    textureArray.textures->Reserve(desc.size);
                    textureArray.textureHashes = new SafeVector<u64>();
                    textureArray.textureHashes->Reserve(desc.size);
                    textureArray.size = desc.size;
                    textureArray.textureIDToArrayIndex = new robin_hood::unordered_map<TextureID::type, u32>();

                });

            return TextureArrayID(static_cast<TextureArrayID::type>(nextHandle));
        }

        TextureID TextureHandlerVK::CreateDataTexture(const DataTextureDesc& desc)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            if (desc.width == 0 || desc.height == 0 || desc.layers == 0)
            {
                NC_LOG_CRITICAL("Invalid DataTexture dimensions! (width {0}, height {1}, layers {2}) ({3})", desc.width, desc.height, desc.layers, desc.debugName.c_str());
            }

            size_t nextHandle;
            Texture* texture = new Texture();

            data.textures.WriteLock([&](std::vector<Texture*>& textures)
            {
                nextHandle = textures.size();

                if (nextHandle >= TextureID::MaxValue())
                {
                    NC_LOG_CRITICAL("We exceeded the limit of the TextureID type!");
                }

                textures.push_back(texture);
            });

            texture->desc = desc;

            if (desc.mipLevels > 1)
            {
                // Texture with mipmaps: Sum size for each mip level.
                size_t mipUploadSize = 0;
                for (uint32_t i = 0; i < desc.mipLevels; ++i)
                {
                    // For each mip level, calculate the reduced dimensions.
                    u32 mipWidth = Math::Max(1u, desc.width >> i);
                    u32 mipHeight = Math::Max(1u, desc.height >> i);
                    mipUploadSize += mipWidth * mipHeight * FormatTexelSize(FormatConverterVK::ToVkFormat(texture->desc.format));
                }
                texture->uploadSize = mipUploadSize;
            }
            else
            {
                // Texture with layers: Normal calculation.
                texture->uploadSize = Math::RoofToInt(static_cast<double>(
                    desc.width * desc.height * desc.layers * FormatTexelSize(FormatConverterVK::ToVkFormat(texture->desc.format))));
            }
            texture->totalSize = texture->uploadSize;

            TextureID textureID = TextureID(static_cast<TextureID::type>(nextHandle));

            // Create texture
            CreateTexture(*texture);

            if (desc.data != nullptr)
            {
                // Create upload buffer
                auto uploadBuffer = _uploadBufferHandler->CreateUploadBuffer(textureID, 0, texture->uploadSize);

                // Copy data to upload buffer
                memcpy(uploadBuffer->mappedMemory, desc.data, texture->uploadSize);
            }
            else
            {
                _device->TransitionImageLayout(texture->image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, desc.layers, desc.mipLevels);
            }

            return textureID;
        }

        TextureID TextureHandlerVK::CreateDataTextureIntoArray(const DataTextureDesc& desc, TextureArrayID textureArrayID, u32& arrayIndex)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);

            if (static_cast<TextureArrayID::type>(textureArrayID) >= data.textureArrays.Size())
            {
                NC_LOG_CRITICAL("Tried to create DataTexture ({0}) into invalid array", desc.debugName.c_str());
            }

            TextureID textureID = CreateDataTexture(desc);

            arrayIndex = AddTextureToArrayInternal(textureID, textureArrayID, 0, true);

            return textureID;
        }

        u32 TextureHandlerVK::AddTextureToArray(const TextureID textureID, const TextureArrayID textureArrayID)
        {
            ZoneScoped;

            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);
            TextureID::type arrayID = static_cast<TextureID::type>(textureArrayID);
            
            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to add invalid TextureID: {0} to array {1}", id, arrayID);
            }

            if (data.textureArrays.Size() <= arrayID)
            {
                NC_LOG_CRITICAL("Tried to add TextureID: {0} to invalid array {1}", id, arrayID);
            }

            return AddTextureToArrayInternal(textureID, textureArrayID, data.textures.ReadGet(id)->hash, false);
        }

        void TextureHandlerVK::CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, size_t srcOffset, TextureID dstTextureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(dstTextureID);

            data.textures.WriteLock(
                [&](std::vector<Texture*>& textures)
                {
                    // Lets make sure this id exists
                    if (textures.size() <= id)
                    {
                        NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
                    }

                    Texture& texture = *textures[id];

                    // If the texture has been unloaded, just return
                    if (!texture.loaded)
                        return;

                    // Transition to TRANSFER_DST_OPTIMAL
                    VkImageLayout beforeLayout = (texture.layoutUndefined) ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    _device->TransitionImageLayout(commandBuffer, texture.image, VK_IMAGE_ASPECT_COLOR_BIT, beforeLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture.desc.layers, texture.desc.mipLevels);

                    u32 numMipsToCopy = texture.desc.mipLevels - texture.numGeneratedMipLevels;

                    // Do the copy
                    VkFormat vkFormat = FormatConverterVK::ToVkFormat(texture.desc.format);
                    _device->CopyBufferToImage(commandBuffer, srcBuffer, srcOffset, texture.image, vkFormat, static_cast<u32>(texture.desc.width), static_cast<u32>(texture.desc.height), texture.desc.layers, numMipsToCopy);

                    // Transition back to SHADER_READ_ONLY_OPTIMAL
                    _device->TransitionImageLayout(commandBuffer, texture.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture.desc.layers, texture.desc.mipLevels);
                    texture.layoutUndefined = false;
                });
        }

        void TextureHandlerVK::TransitionImageLayout(VkCommandBuffer commandBuffer, TextureID textureID, VkImageAspectFlags aspects, VkImageLayout oldLayout, VkImageLayout newLayout)
        {
            //TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            //TextureID::type id = static_cast<TextureID::type>(textureID);
        }

        TextureID TextureHandlerVK::GetTextureIDInArray(const TextureArrayID textureArrayID, u32 index)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureArrayID::type id = static_cast<TextureArrayID::type>(textureArrayID);

            // Lets make sure this array exists
            if (data.textureArrays.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureArrayID: {0}", id);
            }

            const TextureArray& textureArray = data.textureArrays.ReadGet(id);
            
            // Lets make sure this index exists in the array
            if (textureArray.textures->Size() <= index)
            {
                NC_LOG_CRITICAL("Tried to access invalid index {0} in TextureArrayID: {1}", index, id);
            }

            return textureArray.textures->ReadGet(index);
        }

        const SafeVector<TextureID>& TextureHandlerVK::GetTextureIDsInArray(const TextureArrayID textureArrayID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureArrayID::type id = static_cast<TextureArrayID::type>(textureArrayID);

            // Lets make sure this id exists
            if (data.textureArrays.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureArrayID: {0}", id);
            }

            const SafeVector<TextureID>* textures = nullptr;
            data.textureArrays.ReadLock(
                [&](const std::vector<TextureArray>& textureArrays)
                {
                    textures = textureArrays[id].textures;
                });

            return *textures;
        }

        bool TextureHandlerVK::IsOnionTexture(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);

            // Lets make sure this id exists
            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
            }

            return data.textures.ReadGet(id)->desc.layers != 1;
        }

        VkImage TextureHandlerVK::GetImage(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);

            // Lets make sure this id exists
            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
            }

            return data.textures.ReadGet(id)->image;
        }

        VkImageView TextureHandlerVK::GetImageView(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);

            // Lets make sure this id exists
            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
            }

            return data.textures.ReadGet(id)->imageView;
        }

        VkImageView TextureHandlerVK::GetImageView(const TextureID textureID, u32 mipLevel)
        {
            if (mipLevel == 0)
            {
                return GetImageView(textureID);
            }

            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);

            // Lets make sure this id exists
            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
            }

            Texture* texture = data.textures.ReadGet(id);
            if (mipLevel >= texture->mipViews.size())
            {
                NC_LOG_CRITICAL("TextureHandlerVK: GetImageView tried to get a mipLevel that doesn't exist");
            }

            return texture->mipViews[mipLevel];
        }

        VkImageView TextureHandlerVK::GetDebugTextureImageView()
        {
            return GetImageView(_debugTexture);
        }

        VkImageView TextureHandlerVK::GetDebugOnionTextureImageView()
        {
            return GetImageView(_debugOnionTexture);
        }

        VkDescriptorSet TextureHandlerVK::GetImguiTextureID(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);

            // Lets make sure this id exists
            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
            }

            VkDescriptorSet descriptorSet = data.textures.ReadGet(id)->imguiTextureHandle;
            return descriptorSet;
        }

        size_t TextureHandlerVK::GetTextureUploadSize(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);

            // Lets make sure this id exists
            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
            }

            return data.textures.ReadGet(id)->uploadSize;
        }

        size_t TextureHandlerVK::GetTextureTotalSize(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);

            // Lets make sure this id exists
            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
            }

            return data.textures.ReadGet(id)->totalSize;
        }

        TextureBaseDesc TextureHandlerVK::GetTextureDesc(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);

            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
            }

            return data.textures.ReadGet(id)->desc;
        }

        u32 TextureHandlerVK::GetTextureArraySize(const TextureArrayID textureArrayID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureArrayID::type id = static_cast<TextureArrayID::type>(textureArrayID);

            // Lets make sure this id exists
            if (data.textureArrays.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureArrayID: {0}", id);
            }

            return data.textureArrays.ReadGet(id).size;
        }

        ivec2 TextureHandlerVK::GetTextureDimensions(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);

            // Lets make sure this id exists
            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
            }
            const Texture* texture = data.textures.ReadGet(id);
            return ivec2(texture->desc.width, texture->desc.height);
        }

        const std::string& TextureHandlerVK::GetDebugName(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureID::type id = static_cast<TextureID::type>(textureID);

            // Lets make sure this id exists
            if (data.textures.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureID: {0}", id);
            }

            return data.textures.ReadGet(id)->desc.debugName;
        }

        u64 TextureHandlerVK::CalculateDescHash(const TextureDesc& desc)
        {
            u64 hash = XXHash64::hash(desc.path.c_str(), desc.path.size(), 0);
            return hash;
        }

        bool TextureHandlerVK::TryFindExistingTexture(u64 descHash, size_t& id)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            id = 0;

            bool foundTexture = false;
            data.textures.ReadLock(
                [&](const std::vector<Texture*>& textures)
                {
                    for (auto* texture : textures)
                    {
                        if (descHash == texture->hash)
                        {
                            foundTexture = true;
                            return;
                        }
                        id++;
                    }
                });

            return foundTexture;
        }

        bool TextureHandlerVK::TryFindExistingTextureInArray(TextureArrayID textureArrayID, u64 descHash, size_t& arrayIndex, TextureID& textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureArrayID::type id = static_cast<TextureArrayID::type>(textureArrayID);
            if (data.textureArrays.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureArrayID: {0}", id);
            }

            bool foundTexture = false;
            data.textureArrays.WriteLock(
                [&](std::vector<TextureArray>& textureArrays)
                {
                    TextureArray& array = textureArrays[id];

                    for (arrayIndex = 0; arrayIndex < array.textureHashes->Size(); arrayIndex++)
                    {
                        if (descHash == array.textureHashes->ReadGet(arrayIndex))
                        {
                            textureID = array.textures->ReadGet(arrayIndex);
                            foundTexture = true;
                            return;
                        }
                    }
                });

            return foundTexture;
        }

        void TextureHandlerVK::LoadFile(const std::string& filename, Texture& texture, TextureID textureID)
        {
            ZoneScoped;

            int channels;

            void* pixels = stbi_load(filename.c_str(), &texture.desc.width, &texture.desc.height, &channels, STBI_rgb_alpha);

            gli::texture gliTexture;

            // If stbi could open this file
            if (pixels != nullptr)
            {
                // This is hardcoded to 4 instead of channels since STBI is loading it as STBI_rgb_alpha, making it 4 channels
                texture.uploadSize = texture.desc.width * texture.desc.height * 4;
                texture.desc.format = Renderer::ImageFormat::R8G8B8A8_UNORM;
                texture.desc.layers = 1; // If we are not loading using gli we don't support layers, so don't bother with it

                texture.desc.mipLevels = static_cast<u32>(std::floor(std::log2(std::max(texture.desc.width, texture.desc.height)))) + 1;
                u32 totalSize = 0;

                u32 width = texture.desc.width;
                u32 height = texture.desc.height;

                for (size_t i = 0; i < texture.desc.mipLevels; i++)
                {
                    u32 size = width * height * 4;
                    totalSize += size;

                    if (width > 1) width /= 2;
                    if (height > 1) height /= 2;
                }
                texture.totalSize = totalSize;
                texture.numGeneratedMipLevels = texture.desc.mipLevels - 1;
            }
            else
            {
                // Try to open it with gli
                gliTexture = gli::load(filename);
                if (gliTexture.empty())
                {
                    NC_LOG_CRITICAL("Failed to load texture ({0})", filename.c_str());
                }

                gli::gl gl(gli::gl::PROFILE_GL33);
                gli::gl::format const gliFormat = gl.translate(gliTexture.format(), gliTexture.swizzles());

                texture.desc.width = gliTexture.extent().x;
                texture.desc.height = gliTexture.extent().y;
                texture.desc.layers = gliTexture.extent().z;//static_cast<i32>(gliTexture.layers());
                texture.desc.mipLevels = static_cast<i32>(gliTexture.levels());

                texture.desc.format = FormatConverterVK::ToImageFormat(vkGetFormatFromOpenGLInternalFormat(gliFormat.Internal));
                texture.uploadSize = gliTexture.size();
                texture.totalSize = texture.uploadSize; // TODO: Support generating mipmaps for loaded DDSes?

                pixels = gliTexture.data();
            }

            {
                ZoneScopedN("CreateTexture");
                // Create texture
                CreateTexture(texture);

                // Create upload buffer
                auto uploadBuffer = _uploadBufferHandler->CreateUploadBuffer(textureID, 0, texture.uploadSize, texture.numGeneratedMipLevels);

                // Copy data to upload buffer
                memcpy(uploadBuffer->mappedMemory, pixels, texture.uploadSize);
            }
        }

        void TextureHandlerVK::CreateTexture(Texture& texture)
        {
            VkFormat vkFormat = FormatConverterVK::ToVkFormat(texture.desc.format);

            // Create image
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = static_cast<u32>(texture.desc.width);
            imageInfo.extent.height = static_cast<u32>(texture.desc.height);
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = texture.desc.mipLevels;
            imageInfo.arrayLayers = texture.desc.layers;
            imageInfo.format = vkFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

            if (texture.desc.renderable)
            {
                imageInfo.usage = imageInfo.usage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
            }

            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.flags = 0; // Optional

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            if (vmaCreateImage(_device->_allocator, &imageInfo, &allocInfo, &texture.image, &texture.allocation, nullptr) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create image!");
            }

            DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)texture.image, VK_OBJECT_TYPE_IMAGE, texture.desc.debugName.c_str());

            //_device->TransitionImageLayout(texture.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture.layers, texture.mipLevels);

            // Create color view
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = texture.image;
            viewInfo.viewType = (texture.desc.layers > 1) ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = vkFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = texture.desc.mipLevels;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = texture.desc.layers;

            //we want a full mip chain of views
            texture.mipViews.resize(texture.desc.mipLevels);

            for (u32 i = 0; i < texture.desc.mipLevels; ++i)
            {
                VkImageViewCreateInfo pyramidLevelInfo = viewInfo;
                pyramidLevelInfo.subresourceRange.baseMipLevel = i;
                pyramidLevelInfo.subresourceRange.levelCount = 1;

                if (vkCreateImageView(_device->_device, &pyramidLevelInfo, nullptr, &texture.mipViews[i]) != VK_SUCCESS)
                {
                    NC_LOG_CRITICAL("Failed to create color image view!");
                }

                std::string mipName = texture.desc.debugName + " mip " + std::to_string(i);
                DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)texture.mipViews[i], VK_OBJECT_TYPE_IMAGE_VIEW, mipName.c_str());
            }

            if (vkCreateImageView(_device->_device, &viewInfo, nullptr, &texture.imageView) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create texture image view!");
            }

            VkSampler imguiSampler = _samplerHandler->GetSampler(_imguiSampler);

            static std::mutex imguiAddTextureLock;
            {
                std::scoped_lock lock(imguiAddTextureLock);
                texture.imguiTextureHandle = ImGui_ImplVulkan_AddTexture(imguiSampler, texture.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }

            DebugMarkerUtilVK::SetObjectName(_device->_device, (u64)texture.imageView, VK_OBJECT_TYPE_IMAGE_VIEW, texture.desc.debugName.c_str());
        }

        u32 TextureHandlerVK::AddTextureToArrayInternal(const TextureID textureID, const TextureArrayID textureArrayID, u64 hash, bool hasOwnership)
        {
            ZoneScoped;
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);

            u32 arrayIndex = 0;
            data.textureArrays.WriteLock(
                [&, hasOwnership](std::vector<TextureArray>& textureArrays)
                {
                    TextureArray& textureArray = textureArrays[static_cast<TextureArrayID::type>(textureArrayID)];

                    TextureID::type textureIDTyped = static_cast<TextureID::type>(textureID);
                    if (textureArray.textureIDToArrayIndex->find(textureIDTyped) != textureArray.textureIDToArrayIndex->end())
                    {
                        arrayIndex = (*textureArray.textureIDToArrayIndex)[textureIDTyped];
                        return;
                    }

                    arrayIndex = static_cast<u32>(textureArray.textures->Size());
                    textureArray.textures->PushBack(textureID);
                    textureArray.textureHashes->PushBack(hash);

                    if (hasOwnership)
                    {
                        textureArray.ownedTextureIDs.insert(textureIDTyped);
                    }

                    (*textureArray.textureIDToArrayIndex)[textureIDTyped] = arrayIndex;
                });

            return arrayIndex;
        }
    }
}
