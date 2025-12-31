#include "DescriptorHandlerVK.h"
#include "TextureHandlerVK.h"
#include "RenderDeviceVK.h"
#include "FormatConverterVK.h"

#include <Base/Container/SafeVector.h>
#include <Base/Util/DebugHandler.h>

#include <tracy/Tracy.hpp>

namespace Renderer
{
    namespace Backend
    {
        VkDescriptorPoolSize poolSizes[] = 
        {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 40000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLER, 100 }
        };
        constexpr u32 maxDescriptorSets = 128;

        struct DescriptorSet
        {
            DescriptorSetDesc desc;

            VkDescriptorSet sets[RenderDeviceVK::FRAME_INDEX_COUNT];
            VkDescriptorSetLayout layout;
        };

        struct DescriptorHandlerData : public IDescriptorHandlerData
        {
             // Pool data
            VkDescriptorPool permanentPool;
            //VkDescriptorPool framePools[RenderDeviceVK::FRAME_INDEX_COUNT]; // TODO

            std::vector<DescriptorSet> descriptorSets;
        };

        void DescriptorHandlerVK::Init(RenderDeviceVK* device, TextureHandlerVK* textureHandler)
        {
            ZoneScoped;
            _device = device;
            _textureHandler = textureHandler;
            _data = new DescriptorHandlerData();

            CreateDescriptorPool();
        }

        DescriptorSetID DescriptorHandlerVK::CreateDescriptorSet(const DescriptorSetDesc& desc)
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);

            DescriptorSetID id = DescriptorSetID(static_cast<DescriptorSetID::type>(data.descriptorSets.size()));
            DescriptorSet& descriptorSet = data.descriptorSets.emplace_back();
            descriptorSet.desc = desc;

            CreateDescriptorSet(descriptorSet);

            return id;
        }

        VkDescriptorSet DescriptorHandlerVK::GetVkDescriptorSet(DescriptorSetID descriptorSetID, u32 frameIndex)
        {
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);

            DescriptorSetID::type id = static_cast<DescriptorSetID::type>(descriptorSetID);
            if (id >= data.descriptorSets.size())
            {
                NC_LOG_CRITICAL("DescriptorHandlerVK::GetVkDescriptorSet: Invalid DescriptorSetID {}", id);
            }

            return data.descriptorSets[id].sets[frameIndex];
        }

        VkDescriptorSetLayout DescriptorHandlerVK::GetVkDescriptorSetLayout(DescriptorSetID descriptorSetID)
        {
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);

            DescriptorSetID::type id = static_cast<DescriptorSetID::type>(descriptorSetID);
            if (id >= data.descriptorSets.size())
            {
                NC_LOG_CRITICAL("DescriptorHandlerVK::GetVkDescriptorSetLayout: Invalid DescriptorSetID {}", id);
            }

            return data.descriptorSets[id].layout;
        }

        void DescriptorHandlerVK::CreateDescriptorPool()
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);

            VkDescriptorPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
            poolInfo.maxSets = maxDescriptorSets;
            poolInfo.poolSizeCount = ARRAY_COUNT(poolSizes);
            poolInfo.pPoolSizes = poolSizes;
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;

            vkCreateDescriptorPool(_device->_device, &poolInfo, nullptr, &data.permanentPool);
        }

        void DescriptorHandlerVK::CreateDescriptorSet(DescriptorSet& descriptorSet)
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            u32 numSupportedTextures = _device->HasExtendedTextureSupport() ? 8192 : 4096;

            // Init bindings
            u32 numReflectedDescriptors = static_cast<u32>(descriptorSet.desc.reflection->descriptors.size());
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.reserve(numReflectedDescriptors);

            std::vector<VkDescriptorBindingFlags> bindingFlags;
            bindingFlags.reserve(numReflectedDescriptors);

            bool hasVariableBinding = false;

            for (auto& [_, descriptor] : descriptorSet.desc.reflection->descriptors)
            {
                VkDescriptorBindingFlags flags = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
                u32 count = descriptor.count;

                VkDescriptorType vkDescType = FormatConverterVK::ToVkDescriptorType(descriptor);
                bool isTextureType = vkDescType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || vkDescType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                bool isTextureArray = isTextureType && (count != 1);

                if (isTextureArray)
                {
                    flags |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;

                    if (count == 0)
                    {
                        count = numSupportedTextures;
                        hasVariableBinding = true;
                        flags |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
                    }
                }

                VkDescriptorSetLayoutBinding binding{};
                binding.binding = descriptor.binding;
                binding.descriptorType = FormatConverterVK::ToVkDescriptorType(descriptor);
                binding.descriptorCount = count;
                binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
                binding.pImmutableSamplers = nullptr;
                bindings.push_back(binding);
                bindingFlags.push_back(flags);
            }

            // Create layout
            VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
            flagsInfo.bindingCount = static_cast<u32>(bindingFlags.size());
            flagsInfo.pBindingFlags = bindingFlags.data();

            VkDescriptorSetLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            layoutInfo.pNext = &flagsInfo;
            layoutInfo.bindingCount = static_cast<u32>(bindings.size());
            layoutInfo.pBindings = bindings.data();
            layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

            VkResult result = vkCreateDescriptorSetLayout(_device->_device, &layoutInfo, nullptr, &descriptorSet.layout);
            if (result != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("DescriptorHandlerVK::CreateDescriptorSet: Failed to create descriptor set layout!");
            }

            // Create descriptor set
            VkDescriptorSetVariableDescriptorCountAllocateInfo variableCountInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO };
            variableCountInfo.descriptorSetCount = 1;
            variableCountInfo.pDescriptorCounts = &numSupportedTextures;

            VkDescriptorSetAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
            if (hasVariableBinding)
            {
                allocInfo.pNext = &variableCountInfo;
            }
            allocInfo.descriptorPool = data.permanentPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &descriptorSet.layout;

            for (u32 i = 0; i < RenderDeviceVK::FRAME_INDEX_COUNT; i++)
            {
                result = vkAllocateDescriptorSets(_device->_device, &allocInfo, &descriptorSet.sets[i]);
                if (result != VK_SUCCESS)
                {
                    NC_LOG_CRITICAL("DescriptorHandlerVK::CreateDescriptorSet: Failed to allocate descriptor set! You probably need to increase maxDescriptorSets.");
                }
            }
        }

        void DescriptorHandlerVK::BindDescriptor(DescriptorSetID setID, u32 binding, VkBuffer buffer, DescriptorType type, u32 frameIndex)
        {
            ZoneScoped;
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet descriptorWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = GetVkDescriptorSet(setID, frameIndex);
            descriptorWrite.dstBinding = binding;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.descriptorType = FormatConverterVK::ToVkDescriptorType(type);
            descriptorWrite.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(_device->_device, 1, &descriptorWrite, 0, nullptr);
        }

        void DescriptorHandlerVK::BindDescriptor(DescriptorSetID setID, u32 binding, VkImageView image, DescriptorType type, bool isRT, u32 frameIndex)
        {
            ZoneScoped;
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageView = image;
            imageInfo.imageLayout = (!isRT && type == DescriptorType::SampledImage) ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_GENERAL;

            VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            write.dstSet = GetVkDescriptorSet(setID, frameIndex);
            write.dstBinding = binding;
            write.descriptorCount = 1;
            write.descriptorType = FormatConverterVK::ToVkDescriptorType(type);
            write.pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(_device->_device, 1, &write, 0, nullptr);
        }

        void DescriptorHandlerVK::BindDescriptorArray(DescriptorSetID setID, u32 binding, VkImageView image, u32 arrayOffset, DescriptorType type, bool isRT, u32 frameIndex)
        {
            ZoneScoped;
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageView = image;
            imageInfo.imageLayout = (!isRT && type == DescriptorType::SampledImage) ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_GENERAL;

            VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            write.dstSet = GetVkDescriptorSet(setID, frameIndex);
            write.dstBinding = binding;
            write.dstArrayElement = arrayOffset;
            write.descriptorCount = 1;
            write.descriptorType = FormatConverterVK::ToVkDescriptorType(type);
            write.pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(_device->_device, 1, &write, 0, nullptr);
        }

        void DescriptorHandlerVK::BindDescriptorArray(DescriptorSetID setID, u32 binding, std::vector<VkImageView>& images, u32 arrayOffset, DescriptorType type, bool isRT, u32 frameIndex)
        {
            ZoneScoped;
            u32 count = static_cast<u32>(images.size());

            VkImageLayout layout = (!isRT && type == DescriptorType::SampledImage)
                ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                : VK_IMAGE_LAYOUT_GENERAL;

            std::vector<VkDescriptorImageInfo> imageInfos(count);
            for (u32 i = 0; i < count; ++i)
            {
                imageInfos[i].sampler = VK_NULL_HANDLE;
                imageInfos[i].imageView = images[i];
                imageInfos[i].imageLayout = layout;
            }

            VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            write.dstSet = GetVkDescriptorSet(setID, frameIndex);
            write.dstBinding = binding;
            write.dstArrayElement = arrayOffset;
            write.descriptorCount = count;
            write.descriptorType = FormatConverterVK::ToVkDescriptorType(type);
            write.pImageInfo = imageInfos.data();

            vkUpdateDescriptorSets(_device->_device, 1, &write, 0, nullptr);
        }

        void DescriptorHandlerVK::BindDescriptor(DescriptorSetID setID, u32 binding, VkSampler sampler, u32 frameIndex)
        {
            ZoneScoped;
            VkDescriptorImageInfo samplerInfo{};
            samplerInfo.sampler = sampler;

            VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            write.dstSet = GetVkDescriptorSet(setID, frameIndex);
            write.dstBinding = binding;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            write.pImageInfo = &samplerInfo;

            vkUpdateDescriptorSets(_device->_device, 1, &write, 0, nullptr);
        }

        void DescriptorHandlerVK::BindDescriptorArray(DescriptorSetID setID, u32 binding, VkSampler sampler, u32 arrayIndex, u32 frameIndex)
        {
            ZoneScoped;
            VkDescriptorImageInfo samplerInfo{};
            samplerInfo.sampler = sampler;

            VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            write.dstSet = GetVkDescriptorSet(setID, frameIndex);
            write.dstBinding = binding;
            write.dstArrayElement = arrayIndex;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            write.pImageInfo = &samplerInfo;

            vkUpdateDescriptorSets(_device->_device, 1, &write, 0, nullptr);
        }

        void DescriptorHandlerVK::BindDescriptor(DescriptorSetID setID, u32 binding, TextureArrayID textureArrayID)
        {
            ZoneScoped;
            // Register this binding so future texture array updates can propagate to this descriptor set
            _textureHandler->RegisterTextureArrayBinding(textureArrayID, setID, binding);

            const SafeVector<TextureID>& textureIDs = _textureHandler->GetTextureIDsInArray(textureArrayID);
            
            u32 numTextures = static_cast<u32>(textureIDs.Size());

            // Only update the descriptor if there are textures in the array
            if (numTextures == 0)
            {
                return;
            }
            
            std::vector<VkDescriptorImageInfo> imageInfos(numTextures);

            for(u32 i = 0; i < numTextures; i++)
            {
                TextureID textureID = textureIDs.ReadGetUnsafe(i);
                VkImageView imageView = _textureHandler->GetImageView(textureID);

                imageInfos[i].imageView = imageView;
                imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfos[i].sampler = VK_NULL_HANDLE;
            }

            std::vector<VkWriteDescriptorSet> writes(RenderDeviceVK::FRAME_INDEX_COUNT);
            for(u32 i = 0; i < RenderDeviceVK::FRAME_INDEX_COUNT; i++)
            {
                VkWriteDescriptorSet& write = writes[i];
                write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                write.dstSet = GetVkDescriptorSet(setID, i);
                write.dstBinding = binding;
                write.dstArrayElement = 0;
                write.descriptorCount = numTextures;
                write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                write.pImageInfo = imageInfos.data();
            }

            vkUpdateDescriptorSets(_device->_device, static_cast<u32>(writes.size()), writes.data(), 0, nullptr);
        }

        void DescriptorHandlerVK::UpdateTextureArrayDescriptors(DescriptorSetID setID, u32 binding, const TextureID* textureIDs, u32 startIndex, u32 count)
        {
            ZoneScoped;
            if (count == 0)
            {
                return;
            }

            std::vector<VkDescriptorImageInfo> imageInfos(count);

            for (u32 i = 0; i < count; i++)
            {
                VkImageView imageView = _textureHandler->GetImageView(textureIDs[i]);

                imageInfos[i].imageView = imageView;
                imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfos[i].sampler = VK_NULL_HANDLE;
            }

            std::vector<VkWriteDescriptorSet> writes(RenderDeviceVK::FRAME_INDEX_COUNT);
            for (u32 i = 0; i < RenderDeviceVK::FRAME_INDEX_COUNT; i++)
            {
                VkWriteDescriptorSet& write = writes[i];
                write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                write.dstSet = GetVkDescriptorSet(setID, i);
                write.dstBinding = binding;
                write.dstArrayElement = startIndex;
                write.descriptorCount = count;
                write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                write.pImageInfo = imageInfos.data();
            }

            vkUpdateDescriptorSets(_device->_device, static_cast<u32>(writes.size()), writes.data(), 0, nullptr);
        }
    }
}