#include "TextureHandlerVK.h"

#include "RenderDeviceVK.h"
#include "FormatConverterVK.h"
#include "DebugMarkerUtilVK.h"
#include "BufferHandlerVK.h"
#include "DescriptorHandlerVK.h"
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
#include <xxhash/xxhash64.h>

#include <vulkan/vulkan.h>
#include <gli/gli.hpp>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <tracy/Tracy.hpp>
#include <robinhood/robinhood.h>

#include <filesystem>
#include <limits>
#include <queue>
#include <vector>

namespace Renderer
{
    namespace Backend
    {
        struct Texture
        {
        public:
            bool loaded = true;
            u32 referenceCount = 1; // Standalone owner or the array that adopts a create/load reference.
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

        struct TextureArrayBinding
        {
            DescriptorSetID descriptorSetID;
            u32 bindingIndex;
        };

        struct TextureArray
        {
        public:
            u32 size;

            SafeVector<TextureID>* textures = nullptr;
            SafeVector<u64>* textureHashes = nullptr;
            robin_hood::unordered_map<TextureID::type, size_t>* textureIDToArrayIndex;
            robin_hood::unordered_set<TextureID::type> ownedTextureIDs;

            std::vector<TextureArrayBinding> registeredBindings;
            u32 lastFlushedCount = 0;  // Track how many textures were in last descriptor update
        };

        struct TextureHandlerVKData : ITextureHandlerVKData
        {
            SafeVector<Texture*> textures;
            std::queue<Texture*> freeTextureQueue;
            std::vector<VkDescriptorSet> descriptorsToFree;

            SafeVector<TextureArray> textureArrays;
        };

        void TextureHandlerVK::Init(RenderDeviceVK* device, BufferHandlerVK* bufferHandler, DescriptorHandlerVK* descriptorHandler, UploadBufferHandlerVK* uploadBufferHandler, SamplerHandlerVK* samplerHandler)
        {
            _data = new TextureHandlerVKData();
            _device = device;
            _bufferHandler = bufferHandler;
            _descriptorHandler = descriptorHandler;
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

            // Flush texture array updates - only update newly added textures
            if (_descriptorHandler != nullptr)
            {
                data.textureArrays.WriteLock(
                    [&](std::vector<TextureArray>& textureArrays)
                    {
                        for (size_t i = 0; i < textureArrays.size(); i++)
                        {
                            TextureArray& textureArray = textureArrays[i];

                            u32 currentCount = static_cast<u32>(textureArray.textures->Size());
                            if (currentCount <= textureArray.lastFlushedCount)
                            {
                                continue; // No new textures to flush
                            }

                            u32 newTexturesCount = currentCount - textureArray.lastFlushedCount;
                            u32 startIndex = textureArray.lastFlushedCount;

                            // Update all registered descriptor sets with only the new textures
                            textureArray.textures->ReadLock(
                                [&](const std::vector<TextureID>& textures)
                                {
                                    const TextureID* textureIDs = textures.data() + startIndex;

                                    for (const auto& binding : textureArray.registeredBindings)
                                    {
                                        _descriptorHandler->UpdateTextureArrayDescriptors(
                                            binding.descriptorSetID, 
                                            binding.bindingIndex, 
                                            textureIDs, 
                                            startIndex, 
                                            newTexturesCount);
                                    }
                                });

                            textureArray.lastFlushedCount = currentCount;
                        }
                    });
            }
        }

        TextureID TextureHandlerVK::LoadTexture(const TextureDesc& desc)
        {
            ZoneScoped;

            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);

            u64 cacheDescHash = CalculateDescHash(desc);
            TextureID existingTextureID;
            if (TryAcquireExistingTexture(cacheDescHash, existingTextureID))
                return existingTextureID;

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
        TextureID TextureHandlerVK::LoadDataTexture(const DataTextureDesc& desc)
        {
            ZoneScoped;

            if (desc.hash == 0 || desc.data == nullptr || desc.size == 0)
            {
                NC_LOG_CRITICAL("TextureHandlerVK::LoadDataTexture : Hash, data, and size must be set");
                return { };
            }

            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);

            u64 cacheDescHash = desc.hash;
            TextureID existingTextureID;
            if (TryAcquireExistingTexture(cacheDescHash, existingTextureID))
                return existingTextureID;

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
            texture->desc.debugName = std::to_string(cacheDescHash);

            texture->textureIndex = static_cast<TextureID::type>(textureID);
            LoadFromMemory(desc.data, desc.size, *texture, textureID);

            return textureID;
        }

        TextureID TextureHandlerVK::LoadTextureIntoArray(const TextureDesc& desc, TextureArrayID textureArrayID, size_t& arrayIndex, bool allowDuplicates)
        {
            ZoneScoped;

            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureArrayID::type id = static_cast<TextureArrayID::type>(textureArrayID);
            
            if (id >= data.textureArrays.Size())
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
                    arrayIndex = nextID;
                    return textureID; // This texture already exists in this array
                }
            }

            textureID = LoadTexture(desc);

            bool ownershipAccepted = false;
            arrayIndex = AddTextureToArrayInternal(textureID, textureArrayID, descHash, true, &ownershipAccepted);
            if (!ownershipAccepted)
                UnloadTexture(textureID);
            
            return textureID;
        }
        TextureID TextureHandlerVK::LoadDataTextureIntoArray(const DataTextureDesc& desc, TextureArrayID textureArrayID, size_t& arrayIndex, bool allowDuplicates)
        {
            ZoneScoped;

            if (desc.hash == 0 || desc.data == nullptr || desc.size == 0)
            {
                NC_LOG_CRITICAL("TextureHandlerVK::LoadDataTextureIntoArray : Hash, data, and size must be set");
                return { };
            }

            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureArrayID::type id = static_cast<TextureArrayID::type>(textureArrayID);

            if (id >= data.textureArrays.Size())
            {
                NC_LOG_CRITICAL("TextureHandlerVK::LoadTextureIntoArray : Tried to load texture into array ({0}) that doesn't exist", id);
            }

            // Check the cache, we only want to do this for LOADED textures though, never CREATED data textures
            size_t nextID;
            u64 descHash = desc.hash;

            TextureID textureID;

            if (!allowDuplicates)
            {
                if (TryFindExistingTextureInArray(textureArrayID, descHash, nextID, textureID))
                {
                    arrayIndex = nextID;
                    return textureID; // This texture already exists in this array
                }
            }

            textureID = LoadDataTexture(desc);

            bool ownershipAccepted = false;
            arrayIndex = AddTextureToArrayInternal(textureID, textureArrayID, descHash, true, &ownershipAccepted);
            if (!ownershipAccepted)
                UnloadTexture(textureID);

            return textureID;
        }

        void TextureHandlerVK::UnloadTexture(const TextureID textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);

            data.textures.WriteLock(
                [&](std::vector<Texture*>& textures)
                {
                    const TextureID::type id = static_cast<TextureID::type>(textureID);
                    if (id >= textures.size())
                    {
                        NC_LOG_CRITICAL("TextureHandlerVK::UnloadTexture : Tried to unload invalid TextureID {0}", id);
                        return;
                    }

                    Texture* texture = textures[id];

                    if (!texture->loaded)
                        return;

                    if (texture->referenceCount == 0)
                    {
                        NC_LOG_CRITICAL("TextureHandlerVK::UnloadTexture : TextureID {0} has no references", id);
                        return;
                    }

                    texture->referenceCount--;
                    if (texture->referenceCount > 0)
                        return;

                    texture->loaded = false;
                    texture->hash = 0;

                    vmaFreeMemory(_device->_allocator, texture->allocation);
                    vkDestroyImage(_device->_device, texture->image, nullptr);
                    vkDestroyImageView(_device->_device, texture->imageView, nullptr);

                    for (i32 i = 0; i < texture->desc.mipLevels; i++)
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

                    // Update lastFlushedCount so subsequent loads in the same frame get flushed correctly
                    textureArray.lastFlushedCount = Math::Min(textureArray.lastFlushedCount, unloadStartIndex);
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
                    textureArray.textureIDToArrayIndex = new robin_hood::unordered_map<TextureID::type, size_t>();

                });

            return TextureArrayID(static_cast<TextureArrayID::type>(nextHandle));
        }

        void TextureHandlerVK::RegisterTextureArrayBinding(TextureArrayID textureArrayID, DescriptorSetID descriptorSetID, u32 bindingIndex)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureArrayID::type id = static_cast<TextureArrayID::type>(textureArrayID);

            if (data.textureArrays.Size() <= id)
            {
                NC_LOG_CRITICAL("TextureHandlerVK::RegisterTextureArrayBinding: Tried to register binding for invalid TextureArrayID: {0}", id);
            }

            data.textureArrays.WriteLock(
                [&](std::vector<TextureArray>& textureArrays)
                {
                    TextureArray& textureArray = textureArrays[id];

                    // Check if this binding already exists
                    for (const auto& binding : textureArray.registeredBindings)
                    {
                        if (binding.descriptorSetID == descriptorSetID && binding.bindingIndex == bindingIndex)
                        {
                            return; // Already registered
                        }
                    }

                    TextureArrayBinding newBinding;
                    newBinding.descriptorSetID = descriptorSetID;
                    newBinding.bindingIndex = bindingIndex;
                    textureArray.registeredBindings.push_back(newBinding);
                });
        }

        void TextureHandlerVK::FlushTextureArrayDescriptors(TextureArrayID textureArrayID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            TextureArrayID::type id = static_cast<TextureArrayID::type>(textureArrayID);

            if (data.textureArrays.Size() <= id)
            {
                NC_LOG_CRITICAL("TextureHandlerVK::RegisterTextureArrayBinding: Tried to register binding for invalid TextureArrayID: {0}", id);
            }

            data.textureArrays.WriteLock(
                [&](std::vector<TextureArray>& textureArrays)
                {
                    TextureArray& textureArray = textureArrays[id];
                    
                    u32 currentCount = static_cast<u32>(textureArray.textures->Size());
                    if (currentCount <= textureArray.lastFlushedCount)
                    {
                        return; // No new textures to flush
                    }

                    u32 newTexturesCount = currentCount - textureArray.lastFlushedCount;
                    u32 startIndex = textureArray.lastFlushedCount;

                    // Update all registered descriptor sets with only the new textures
                    textureArray.textures->ReadLock(
                        [&](const std::vector<TextureID>& textures)
                        {
                            const TextureID* textureIDs = textures.data() + startIndex;

                            for (const auto& binding : textureArray.registeredBindings)
                            {
                                _descriptorHandler->UpdateTextureArrayDescriptors(
                                    binding.descriptorSetID,
                                    binding.bindingIndex,
                                    textureIDs,
                                    startIndex,
                                    newTexturesCount);
                            }
                        });

                    textureArray.lastFlushedCount = currentCount;
                    
                });
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
                for (i32 i = 0; i < desc.mipLevels; ++i)
                {
                    // For each mip level, calculate the reduced dimensions.
                    u32 mipWidth = Math::Max(1, desc.width >> i);
                    u32 mipHeight = Math::Max(1, desc.height >> i);
                    mipUploadSize += static_cast<u64>(static_cast<f64>(mipWidth) * static_cast<f64>(mipHeight) * FormatTexelSize(FormatConverterVK::ToVkFormat(texture->desc.format)));
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

        TextureID TextureHandlerVK::CreateDataTextureIntoArray(const DataTextureDesc& desc, TextureArrayID textureArrayID, size_t& arrayIndex)
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

        size_t TextureHandlerVK::AddTextureToArray(const TextureID textureID, const TextureArrayID textureArrayID)
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

        void TextureHandlerVK::CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, size_t srcOffset, TextureID dstTextureID, const TextureUploadRegion& region)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            const TextureID::type id = static_cast<TextureID::type>(dstTextureID);

            data.textures.WriteLock(
                [&](std::vector<Texture*>& textures)
                {
                    if (textures.size() <= id)
                    {
                        NC_LOG_CRITICAL("Tried to upload a region to an invalid TextureID: {0}", id);
                        return;
                    }

                    Texture& texture = *textures[id];
                    if (!texture.loaded)
                        return;

                    const u32 mipWidth = glm::max(1u, static_cast<u32>(texture.desc.width) >> region.mipLevel);
                    const u32 mipHeight = glm::max(1u, static_cast<u32>(texture.desc.height) >> region.mipLevel);
                    if (region.extent.x == 0 || region.extent.y == 0 || region.layer >= static_cast<u32>(texture.desc.layers) || region.mipLevel >= static_cast<u32>(texture.desc.mipLevels) ||
                        region.offset.x > mipWidth || region.extent.x > mipWidth - region.offset.x || region.offset.y > mipHeight || region.extent.y > mipHeight - region.offset.y)
                    {
                        NC_LOG_CRITICAL("Texture region upload is outside the target texture");
                        return;
                    }

                    VkImageMemoryBarrier barrier = {};
                    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.image = texture.image;
                    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    barrier.subresourceRange.baseMipLevel = region.mipLevel;
                    barrier.subresourceRange.levelCount = 1;
                    barrier.subresourceRange.baseArrayLayer = region.layer;
                    barrier.subresourceRange.layerCount = 1;
                    barrier.oldLayout = texture.layoutUndefined ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    barrier.srcAccessMask = texture.layoutUndefined ? 0 : VK_ACCESS_SHADER_READ_BIT;
                    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                    VkBufferImageCopy copy = {};
                    copy.bufferOffset = srcOffset;
                    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    copy.imageSubresource.mipLevel = region.mipLevel;
                    copy.imageSubresource.baseArrayLayer = region.layer;
                    copy.imageSubresource.layerCount = 1;
                    copy.imageOffset = { static_cast<i32>(region.offset.x), static_cast<i32>(region.offset.y), 0 };
                    copy.imageExtent = { region.extent.x, region.extent.y, 1 };
                    vkCmdCopyBufferToImage(commandBuffer, srcBuffer, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

                    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
                    texture.layoutUndefined = false;
                });
        }

        void TextureHandlerVK::TransitionImageLayout(VkCommandBuffer commandBuffer, TextureID textureID, VkImageAspectFlags aspects, VkImageLayout oldLayout, VkImageLayout newLayout)
        {
            //TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            //TextureID::type id = static_cast<TextureID::type>(textureID);
        }

        TextureID TextureHandlerVK::GetTextureIDInArray(const TextureArrayID textureArrayID, size_t index)
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

        bool TextureHandlerVK::TryGetTextureUploadRegionSize(const TextureID textureID, const TextureUploadRegion& region, size_t& uploadSize)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            const TextureID::type id = static_cast<TextureID::type>(textureID);
            uploadSize = 0;

            if (textureID == TextureID::Invalid() || data.textures.Size() <= id)
                return false;

            bool valid = false;
            data.textures.ReadLock(
                [&](const std::vector<Texture*>& textures)
                {
                    const Texture* texture = textures[id];
                    if (!texture->loaded || texture->desc.width <= 0 || texture->desc.height <= 0 || texture->desc.layers <= 0 || texture->desc.mipLevels <= 0 ||
                        region.mipLevel >= static_cast<u32>(texture->desc.mipLevels) || region.mipLevel >= 32 || region.layer >= static_cast<u32>(texture->desc.layers))
                    {
                        return;
                    }

                    const u32 mipWidth = glm::max(1u, static_cast<u32>(texture->desc.width) >> region.mipLevel);
                    const u32 mipHeight = glm::max(1u, static_cast<u32>(texture->desc.height) >> region.mipLevel);
                    if (region.extent.x == 0 || region.extent.y == 0 || region.offset.x > mipWidth || region.extent.x > mipWidth - region.offset.x ||
                        region.offset.y > mipHeight || region.extent.y > mipHeight - region.offset.y)
                    {
                        return;
                    }

                    const VkFormat format = FormatConverterVK::ToVkFormat(texture->desc.format);
                    const VkExtent3D blockExtent = FormatTexelBlockExtent(format);
                    const u32 blockSize = FormatElementSize(format, VK_IMAGE_ASPECT_COLOR_BIT);
                    if (blockExtent.width == 0 || blockExtent.height == 0 || blockSize == 0)
                        return;

                    if (FormatIsCompressed(format) &&
                        ((region.offset.x % blockExtent.width) != 0 || (region.offset.y % blockExtent.height) != 0 ||
                            ((region.extent.x % blockExtent.width) != 0 && region.offset.x + region.extent.x != mipWidth) ||
                            ((region.extent.y % blockExtent.height) != 0 && region.offset.y + region.extent.y != mipHeight)))
                    {
                        return;
                    }

                    const size_t blockCountX = (static_cast<size_t>(region.extent.x) + blockExtent.width - 1) / blockExtent.width;
                    const size_t blockCountY = (static_cast<size_t>(region.extent.y) + blockExtent.height - 1) / blockExtent.height;
                    if (blockCountX > std::numeric_limits<size_t>::max() / blockCountY || blockCountX * blockCountY > std::numeric_limits<size_t>::max() / blockSize)
                        return;

                    uploadSize = blockCountX * blockCountY * blockSize;
                    valid = true;
                });

            return valid;
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

        bool TextureHandlerVK::TryFindExistingTexture(u64 descHash, TextureID& textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            textureID = TextureID::Invalid();

            data.textures.ReadLock(
                [&](const std::vector<Texture*>& textures)
                {
                    for (size_t i = 0; i < textures.size(); i++)
                    {
                        const Texture* texture = textures[i];
                        if (texture->loaded && descHash == texture->hash)
                        {
                            textureID = TextureID(static_cast<TextureID::type>(i));
                            return;
                        }
                    }
                });

            bool foundTexture = textureID != TextureID::Invalid();
            return foundTexture;
        }

        bool TextureHandlerVK::TryAcquireExistingTexture(u64 descHash, TextureID& textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            textureID = TextureID::Invalid();

            data.textures.WriteLock(
                [&](std::vector<Texture*>& textures)
                {
                    for (size_t i = 0; i < textures.size(); i++)
                    {
                        Texture* texture = textures[i];
                        if (!texture->loaded || texture->hash != descHash)
                            continue;

                        if (texture->referenceCount == std::numeric_limits<u32>::max())
                        {
                            NC_LOG_CRITICAL("TextureHandlerVK::TryAcquireExistingTexture : TextureID {0} reference count overflow", i);
                            return;
                        }

                        texture->referenceCount++;
                        textureID = TextureID(static_cast<TextureID::type>(i));
                        return;
                    }
                });

            return textureID != TextureID::Invalid();
        }

        bool TextureHandlerVK::TryFindExistingTextureInArray(TextureArrayID textureArrayID, u64 descHash, size_t& arrayIndex, TextureID& textureID)
        {
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);
            const TextureArrayID::type id = static_cast<TextureArrayID::type>(textureArrayID);
            arrayIndex = 0;
            textureID = TextureID::Invalid();
            if (data.textureArrays.Size() <= id)
            {
                NC_LOG_CRITICAL("Tried to access invalid TextureArrayID: {0}", id);
                return false;
            }

            bool foundTexture = false;
            data.textureArrays.ReadLock(
                [&](const std::vector<TextureArray>& textureArrays)
                {
                    const TextureArray& array = textureArrays[id];
                    data.textures.ReadLock(
                        [&](const std::vector<Texture*>& textures)
                        {
                            for (arrayIndex = 0; arrayIndex < array.textureHashes->Size(); arrayIndex++)
                            {
                                const TextureID candidateTextureID = array.textures->ReadGet(arrayIndex);
                                const TextureID::type candidateID = static_cast<TextureID::type>(candidateTextureID);
                                if (descHash == array.textureHashes->ReadGet(arrayIndex) && candidateID < textures.size() && textures[candidateID]->loaded)
                                {
                                    textureID = candidateTextureID;
                                    foundTexture = true;
                                    return;
                                }
                            }
                        });
                });

            return foundTexture;
        }

        void TextureHandlerVK::LoadFile(const std::string& filename, Texture& texture, TextureID textureID)
        {
            ZoneScoped;

            int channels;

            void* pixels = stbi_load(filename.c_str(), &texture.desc.width, &texture.desc.height, &channels, STBI_rgb_alpha);
            const bool freePixels = pixels != nullptr;

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

            if (freePixels)
            {
                stbi_image_free(pixels);
            }
        }

        void TextureHandlerVK::LoadFromMemory(const u8* data, size_t size, Texture& texture, TextureID textureID)
        {
            ZoneScoped;

            int channels;

            void* pixels = stbi_load_from_memory(data, static_cast<i32>(size), &texture.desc.width, &texture.desc.height, &channels, STBI_rgb_alpha);
            const bool freePixels = pixels != nullptr;

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
                gliTexture = gli::load(reinterpret_cast<const char*>(data), size);
                if (gliTexture.empty())
                {
                    NC_LOG_CRITICAL("Failed to load texture ({0})", "Unknown");
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

            if (freePixels)
            {
                stbi_image_free(pixels);
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

            for (i32 i = 0; i < texture.desc.mipLevels; ++i)
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

        size_t TextureHandlerVK::AddTextureToArrayInternal(const TextureID textureID, const TextureArrayID textureArrayID, u64 hash, bool hasOwnership, bool* ownershipAccepted)
        {
            ZoneScoped;
            TextureHandlerVKData& data = static_cast<TextureHandlerVKData&>(*_data);

            if (ownershipAccepted)
                *ownershipAccepted = false;

            size_t arrayIndex = 0;
            data.textureArrays.WriteLock(
                [&](std::vector<TextureArray>& textureArrays)
                {
                    TextureArray& textureArray = textureArrays[static_cast<TextureArrayID::type>(textureArrayID)];

                    TextureID::type textureIDTyped = static_cast<TextureID::type>(textureID);
                    if (textureArray.textureIDToArrayIndex->find(textureIDTyped) != textureArray.textureIDToArrayIndex->end())
                    {
                        arrayIndex = (*textureArray.textureIDToArrayIndex)[textureIDTyped];

                        if (hasOwnership && !textureArray.ownedTextureIDs.contains(textureIDTyped))
                        {
                            textureArray.ownedTextureIDs.insert(textureIDTyped);
                            if (ownershipAccepted)
                                *ownershipAccepted = true;
                        }

                        return;
                    }

                    arrayIndex = textureArray.textures->Size();
                    textureArray.textures->PushBack(textureID);
                    textureArray.textureHashes->PushBack(hash);

                    if (hasOwnership)
                    {
                        textureArray.ownedTextureIDs.insert(textureIDTyped);
                        if (ownershipAccepted)
                            *ownershipAccepted = true;
                    }

                    (*textureArray.textureIDToArrayIndex)[textureIDTyped] = arrayIndex;
                });

            return arrayIndex;
        }
    }
}
