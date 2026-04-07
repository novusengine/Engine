#include "DescriptorHandlerVK.h"
#include "TextureHandlerVK.h"
#include "BufferHandlerVK.h"
#include "RenderDeviceVK.h"
#include "FormatConverterVK.h"
#include "Renderer/TrackedBufferBitSets.h"

#include <Base/Container/SafeVector.h>
#include <Base/Container/PersistentBitSet.h>
#include <Base/Util/DebugHandler.h>

#include <FileFormat/Novus/ShaderPack/ShaderPack.h>

#include <tracy/Tracy.hpp>
#include <unordered_map>

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

            PersistentBitSet bufferAccesses;      // All accessed buffers (for pipeline stage check)
            PersistentBitSet bufferReadAccesses;  // Buffers read from
            PersistentBitSet bufferWriteAccesses; // Buffers written to
            
            // Bitset to show which bindings are write access
            PersistentBitSet writeBindings;
            
            // Bitset to track unbound bindings - set on create, unset on bind
            PersistentBitSet unboundBindings;
            
            // Track which buffer is bound to each binding so we can unset the bit when rebound
            std::unordered_map<u32, BufferID> bindingToBuffer;

            // Reverse map: buffer -> binding (for fast lookup during validation)
            std::unordered_map<BufferID::type, u32> bufferToBinding;
        };

        struct DescriptorHandlerData : public IDescriptorHandlerData
        {
             // Pool data
            VkDescriptorPool permanentPool;
            //VkDescriptorPool framePools[RenderDeviceVK::FRAME_INDEX_COUNT]; // TODO

            std::vector<DescriptorSet> descriptorSets;
        };

        std::string GetBindingName(const DescriptorSet& descriptorSet, u32 binding)
        {
            for (const auto& [_, descriptor] : descriptorSet.desc.reflection->descriptors)
            {
                if (descriptor.binding == binding)
                    return descriptor.name;
            }
            return "Unknown";
        }

        std::string BindingSlotNames[] = { 
            "INVALID", 
            "DEBUG",
            "GLOBAL",
            "LIGHT",
            "TERRAIN",
            "MODEL",
            "PER_PASS",
            "PER_DRAW" 
        };

        void DescriptorHandlerVK::Init(RenderDeviceVK* device, TextureHandlerVK* textureHandler, BufferHandlerVK* bufferHandler)
        {
            ZoneScoped;
            _device = device;
            _textureHandler = textureHandler;
            _bufferHandler = bufferHandler;
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

        bool DescriptorHandlerVK::ValidatePermissionViolations(u32 slot, const DescriptorSet& descriptorSet, const PersistentBitSet& accesses, const BitSet& permissions, const char* permissionName, const PersistentBitSet* usedBindings)
        {
            if (accesses.IsEmpty())
                return false;

            if (accesses.IsSubsetOf(permissions))
                return false;

            // Compute violations: accesses & ~permissions
            PersistentBitSet violations;
            violations.SetEquals(accesses);
            violations.BitwiseUnset(permissions);

            bool didError = false;
            violations.ForEachSetBit([&](u32 setIndex, u32 bitIndex)
            {
                u32 bufferIndex = setIndex * 64 + bitIndex;

                auto it = descriptorSet.bufferToBinding.find(bufferIndex);
                i32 binding = (it != descriptorSet.bufferToBinding.end()) ? static_cast<i32>(it->second) : -1;

                // Skip buffers at bindings the current pipeline doesn't use
                if (usedBindings && binding >= 0 && !usedBindings->Has(static_cast<u32>(binding)))
                    return;

                if (!didError)
                {
                    NC_LOG_ERROR("--- {} ACCESS VIOLATIONS ---", permissionName);
                    didError = true;
                }

                BufferID bufferID = BufferID(bufferIndex);
                const std::string& bufferName = _bufferHandler->GetBufferName(bufferID);
                std::string bindingName = (binding >= 0) ? GetBindingName(descriptorSet, static_cast<u32>(binding)) : "Unknown";

                NC_LOG_ERROR(" ({}) Set {} Buffer {} '{}' at binding {} '{}' needs {} permission", BindingSlotNames[slot], bufferIndex, bufferName, binding, bindingName, permissionName);
            });

            return didError;
        }

        void DescriptorHandlerVK::ValidatePermissions(u32 slot, DescriptorSetID descriptorSetID, const TrackedBufferBitSets* bufferPermissions, bool isGraphicsPipeline, const PersistentBitSet* usedBindings)
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);

            DescriptorSetID::type id = static_cast<DescriptorSetID::type>(descriptorSetID);
            if (id >= data.descriptorSets.size())
                return;

            DescriptorSet& descriptorSet = data.descriptorSets[id];
            bool didError = false;

            // Check for unbound bindings
            if (descriptorSet.unboundBindings.HasAnyBitSet())
            {
                bool hasUnbound = false;
                descriptorSet.unboundBindings.ForEachSetBit([&](u32 setIndex, u32 bitIndex)
                {
                    u32 binding = setIndex * 64 + bitIndex;
                    if (usedBindings && !usedBindings->Has(binding))
                        return;

                    if (!hasUnbound)
                    {
                        NC_LOG_ERROR("--- UNBOUND BINDINGS ---");
                        hasUnbound = true;
                    }
                    std::string bindingName = GetBindingName(descriptorSet, binding);
                    NC_LOG_ERROR(" ({}) Binding {} '{}' was never bound", BindingSlotNames[slot], binding, bindingName);
                });
                didError |= hasUnbound;
            }

            // Check read accesses
            didError |= ValidatePermissionViolations(slot, descriptorSet, descriptorSet.bufferReadAccesses, bufferPermissions->GetReadBitSet(), "READ", usedBindings);

            // Check write accesses
            didError |= ValidatePermissionViolations(slot, descriptorSet, descriptorSet.bufferWriteAccesses, bufferPermissions->GetWriteBitSet(), "WRITE", usedBindings);

            // Check pipeline stage permissions
            const BitSet& stagePermissions = isGraphicsPipeline ? bufferPermissions->GetGraphicsBitSet() : bufferPermissions->GetComputeBitSet();
            didError |= ValidatePermissionViolations(slot, descriptorSet, descriptorSet.bufferAccesses, stagePermissions, isGraphicsPipeline ? "GRAPHICS" : "COMPUTE", usedBindings);

            if (didError)
            {
                NC_LOG_CRITICAL("ValidatePermissions failed for DescriptorSet {}", id);
            }
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

            // Store binding info from reflection
            for (auto& [_, descriptor] : descriptorSet.desc.reflection->descriptors)
            {
                // Track which bindings need to be bound
                descriptorSet.unboundBindings.Set(descriptor.binding);
                
                // Track which bindings are write access
                if (descriptor.accessType == FileFormat::DescriptorAccessTypeReflection::ReadWrite ||
                    descriptor.accessType == FileFormat::DescriptorAccessTypeReflection::Write)
                {
                    descriptorSet.writeBindings.Set(descriptor.binding);
                }
            }
        }

        void DescriptorHandlerVK::BindDescriptor(DescriptorSetID setID, u32 binding, BufferID bufferID, VkBuffer buffer, DescriptorType type, u32 frameIndex)
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];

            BufferID::type newBufferIndex = static_cast<BufferID::type>(bufferID);
            
            // Check if this binding already has a buffer bound
            auto it = descriptorSet.bindingToBuffer.find(binding);
            if (it != descriptorSet.bindingToBuffer.end())
            {
                BufferID oldBufferID = it->second;
                
                // Only unset if it's a different buffer
                if (oldBufferID != bufferID)
                {
                    BufferID::type oldBufferIndex = static_cast<BufferID::type>(oldBufferID);

                    // Unset the old buffer's bits
                    descriptorSet.bufferAccesses.Unset(oldBufferIndex);
                    descriptorSet.bufferReadAccesses.Unset(oldBufferIndex);
                    if (descriptorSet.writeBindings.Has(binding))
                    {
                        descriptorSet.bufferWriteAccesses.Unset(oldBufferIndex);
                    }
                    descriptorSet.bufferToBinding.erase(oldBufferIndex);
                }
            }
            
            // Set the new buffer's bits
            descriptorSet.bufferAccesses.Set(newBufferIndex);
            descriptorSet.bufferReadAccesses.Set(newBufferIndex);
            
            // Pure bitwise check - no hash lookup
            if (descriptorSet.writeBindings.Has(binding))
            {
                descriptorSet.bufferWriteAccesses.Set(newBufferIndex);
            }
            
            // Update the binding <-> buffer mappings
            descriptorSet.bindingToBuffer[binding] = bufferID;
            descriptorSet.bufferToBinding[newBufferIndex] = binding;
            
            // Mark binding as bound
            descriptorSet.unboundBindings.Unset(binding);
            
            // Vulkan descriptor update
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
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);
            
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
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);
            
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
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);
            
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
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);
            
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
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);
            
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
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);
            
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
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);
            
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