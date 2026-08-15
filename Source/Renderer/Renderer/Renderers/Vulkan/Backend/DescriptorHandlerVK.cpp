#include "DescriptorHandlerVK.h"
#include "TextureHandlerVK.h"
#include "BufferHandlerVK.h"
#include "RenderDeviceVK.h"
#include "FormatConverterVK.h"
#include "Renderer/DescriptorSet.h"
#include "Renderer/TrackedBufferBitSets.h"

#include <Base/Container/SafeVector.h>
#include <Base/Container/PersistentBitSet.h>
#include <Base/Util/DebugHandler.h>

#include <FileFormat/Novus/ShaderPack/ShaderPack.h>

#include <tracy/Tracy.hpp>
#include <algorithm>
#include <array>
#include <limits>
#include <memory>
#include <unordered_map>

namespace Renderer
{
    namespace Backend
    {
        VkDescriptorPoolSize poolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 512 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 131072 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4096 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 256 },
            { VK_DESCRIPTOR_TYPE_SAMPLER, 128 }
        };
        constexpr u32 maxDescriptorSets = 512;

        // [Temp descriptor sets] Per-frame transient pools, reset in FlipFrame once the slot's fence
        // guarantees the GPU is done with the previous frame's transient sets
        VkDescriptorPoolSize framePoolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 256 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1024 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1024 },
            { VK_DESCRIPTOR_TYPE_SAMPLER, 128 }
        };
        constexpr u32 maxTempDescriptorSetsPerFrame = 256;

        // [Frame-safe descriptor rebind] A buffer descriptor write that must wait until its target frame
        // slot is no longer being read by an in-flight frame before it can safely be applied.
        struct PendingBufferWrite
        {
            BufferID bufferID;
            DescriptorType type;
        };

        struct DescriptorSet
        {
            FileFormat::DescriptorSetReflection reflection;

            VkDescriptorSet sets[RenderDeviceVK::FRAME_INDEX_COUNT];
            VkDescriptorSetLayout layout;
            std::array<u32, 5> poolUsage = {};

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

            // [Frame-safe descriptor rebind] Buffer-binding writes recorded per frame-copy and applied in
            // FlushPendingBufferWrites once that slot's fence has been waited (its previous frame is done),
            // so we never rewrite a descriptor copy an in-flight frame is still reading.
            std::unordered_map<u32, PendingBufferWrite> pendingBufferWritesPerSlot[RenderDeviceVK::FRAME_INDEX_COUNT];

            // [Temp descriptor sets] Highest written element end per fixed-size array binding, so a
            // snapshot only copies descriptors that have actually been written
            std::unordered_map<u32, u32> bindingWrittenCounts;

            bool hasVariableBinding = false;

            // Rebind-after-bind detection, see WarnIfBoundThisFrame
            u64 lastBoundGeneration = 0;
            u64 lastWarnGeneration = 0;
        };

        struct DescriptorHandlerData : public IDescriptorHandlerData
        {
             // Pool data
            VkDescriptorPool permanentPool;
            VkDescriptorPool framePools[RenderDeviceVK::FRAME_INDEX_COUNT];
            std::vector<VkDescriptorSet> transientSets[RenderDeviceVK::FRAME_INDEX_COUNT];

            std::vector<std::unique_ptr<DescriptorSet>> descriptorSets;
            std::vector<DescriptorSetID> freeDescriptorSetIDs;
            DescriptorPoolStats poolStats;
        };

        std::string GetBindingName(const DescriptorSet& descriptorSet, u32 binding)
        {
            for (const auto& [_, descriptor] : descriptorSet.reflection.descriptors)
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
            "MATERIAL",
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

            DescriptorSetID id;
            if (data.freeDescriptorSetIDs.empty())
            {
                id = DescriptorSetID(static_cast<DescriptorSetID::type>(data.descriptorSets.size()));
                data.descriptorSets.push_back(std::make_unique<DescriptorSet>());
            }
            else
            {
                id = data.freeDescriptorSetIDs.back();
                data.freeDescriptorSetIDs.pop_back();
                data.descriptorSets[static_cast<DescriptorSetID::type>(id)] = std::make_unique<DescriptorSet>();
            }

            DescriptorSet& descriptorSet = *data.descriptorSets[static_cast<DescriptorSetID::type>(id)];
            descriptorSet.reflection = *desc.reflection;

            CreateDescriptorSet(descriptorSet);

            return id;
        }

        void DescriptorHandlerVK::DestroyDescriptorSet(DescriptorSetID setID)
        {
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            const DescriptorSetID::type id = static_cast<DescriptorSetID::type>(setID);
            NC_ASSERT(id < data.descriptorSets.size() && data.descriptorSets[id], "DescriptorHandlerVK::DestroyDescriptorSet: Invalid DescriptorSetID");

            DescriptorSet& descriptorSet = *data.descriptorSets[id];
            _textureHandler->UnregisterDescriptorSet(setID);
            VkResult result = vkFreeDescriptorSets(_device->_device, data.permanentPool, RenderDeviceVK::FRAME_INDEX_COUNT, descriptorSet.sets);
            if (result != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("DescriptorHandlerVK::DestroyDescriptorSet: Failed to free descriptor sets ({})", static_cast<i32>(result));
            }
            vkDestroyDescriptorSetLayout(_device->_device, descriptorSet.layout, nullptr);

            data.poolStats.liveSets -= RenderDeviceVK::FRAME_INDEX_COUNT;
            data.poolStats.liveUniformBuffers -= descriptorSet.poolUsage[0];
            data.poolStats.liveSampledImages -= descriptorSet.poolUsage[1];
            data.poolStats.liveStorageBuffers -= descriptorSet.poolUsage[2];
            data.poolStats.liveStorageImages -= descriptorSet.poolUsage[3];
            data.poolStats.liveSamplers -= descriptorSet.poolUsage[4];
            data.descriptorSets[id].reset();
            data.freeDescriptorSetIDs.push_back(setID);
        }

        DescriptorPoolStats DescriptorHandlerVK::GetPoolStats() const
        {
            const DescriptorHandlerData& data = *static_cast<const DescriptorHandlerData*>(_data);
            return data.poolStats;
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

                NC_LOG_ERROR(" ({}) Buffer {} '{}' at binding {} '{}' needs {} permission", BindingSlotNames[slot], bufferIndex, bufferName, binding, bindingName, permissionName);
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

            if (!data.descriptorSets[id])
                return;
            DescriptorSet& descriptorSet = *data.descriptorSets[id];
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
            if (id >= data.descriptorSets.size() || !data.descriptorSets[id])
            {
                NC_LOG_CRITICAL("DescriptorHandlerVK::GetVkDescriptorSet: Invalid DescriptorSetID {}", id);
            }

            return data.descriptorSets[id]->sets[frameIndex];
        }

        VkDescriptorSetLayout DescriptorHandlerVK::GetVkDescriptorSetLayout(DescriptorSetID descriptorSetID)
        {
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);

            DescriptorSetID::type id = static_cast<DescriptorSetID::type>(descriptorSetID);
            if (id >= data.descriptorSets.size() || !data.descriptorSets[id])
            {
                NC_LOG_CRITICAL("DescriptorHandlerVK::GetVkDescriptorSetLayout: Invalid DescriptorSetID {}", id);
            }

            return data.descriptorSets[id]->layout;
        }

        void DescriptorHandlerVK::CreateDescriptorPool()
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);

            VkDescriptorPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
            poolInfo.maxSets = maxDescriptorSets;
            poolInfo.poolSizeCount = ARRAY_COUNT(poolSizes);
            poolInfo.pPoolSizes = poolSizes;
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

            vkCreateDescriptorPool(_device->_device, &poolInfo, nullptr, &data.permanentPool);

            VkDescriptorPoolCreateInfo framePoolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
            framePoolInfo.maxSets = maxTempDescriptorSetsPerFrame;
            framePoolInfo.poolSizeCount = ARRAY_COUNT(framePoolSizes);
            framePoolInfo.pPoolSizes = framePoolSizes;
            framePoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;

            for (u32 i = 0; i < RenderDeviceVK::FRAME_INDEX_COUNT; i++)
            {
                vkCreateDescriptorPool(_device->_device, &framePoolInfo, nullptr, &data.framePools[i]);
            }

            data.poolStats.setCapacity = maxDescriptorSets;
            data.poolStats.uniformBufferCapacity = poolSizes[0].descriptorCount;
            data.poolStats.sampledImageCapacity = poolSizes[1].descriptorCount;
            data.poolStats.storageBufferCapacity = poolSizes[2].descriptorCount;
            data.poolStats.storageImageCapacity = poolSizes[3].descriptorCount;
            data.poolStats.samplerCapacity = poolSizes[4].descriptorCount;
        }

        void DescriptorHandlerVK::CreateDescriptorSet(DescriptorSet& descriptorSet)
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            u32 numSupportedTextures = _device->HasExtendedTextureSupport() ? 8192 : 4096;

            // Init bindings
            u32 numReflectedDescriptors = static_cast<u32>(descriptorSet.reflection.descriptors.size());
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.reserve(numReflectedDescriptors);

            std::vector<VkDescriptorBindingFlags> bindingFlags;
            bindingFlags.reserve(numReflectedDescriptors);

            bool hasVariableBinding = false;

            for (auto& [_, descriptor] : descriptorSet.reflection.descriptors)
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
                binding.stageFlags = _device->GetEnabledShaderStageFlags();
                binding.pImmutableSamplers = nullptr;
                bindings.push_back(binding);
                bindingFlags.push_back(flags);

                u32 poolIndex = std::numeric_limits<u32>::max();
                switch (binding.descriptorType)
                {
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: poolIndex = 0; break;
                    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE: poolIndex = 1; break;
                    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: poolIndex = 2; break;
                    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE: poolIndex = 3; break;
                    case VK_DESCRIPTOR_TYPE_SAMPLER: poolIndex = 4; break;
                    default: break;
                }
                if (poolIndex != std::numeric_limits<u32>::max())
                    descriptorSet.poolUsage[poolIndex] += count * RenderDeviceVK::FRAME_INDEX_COUNT;
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
                    NC_LOG_CRITICAL("DescriptorHandlerVK::CreateDescriptorSet: Allocation failed ({}) at sets={}/{} uniform={}/{} sampled={}/{} storage_buffers={}/{} storage_images={}/{} samplers={}/{}",
                        static_cast<i32>(result), data.poolStats.liveSets, data.poolStats.setCapacity,
                        data.poolStats.liveUniformBuffers, data.poolStats.uniformBufferCapacity,
                        data.poolStats.liveSampledImages, data.poolStats.sampledImageCapacity,
                        data.poolStats.liveStorageBuffers, data.poolStats.storageBufferCapacity,
                        data.poolStats.liveStorageImages, data.poolStats.storageImageCapacity,
                        data.poolStats.liveSamplers, data.poolStats.samplerCapacity);
                }
            }

            descriptorSet.hasVariableBinding = hasVariableBinding;

            // Store binding info from reflection
            for (auto& [_, descriptor] : descriptorSet.reflection.descriptors)
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

            data.poolStats.liveSets += RenderDeviceVK::FRAME_INDEX_COUNT;
            data.poolStats.liveUniformBuffers += descriptorSet.poolUsage[0];
            data.poolStats.liveSampledImages += descriptorSet.poolUsage[1];
            data.poolStats.liveStorageBuffers += descriptorSet.poolUsage[2];
            data.poolStats.liveStorageImages += descriptorSet.poolUsage[3];
            data.poolStats.liveSamplers += descriptorSet.poolUsage[4];
            data.poolStats.peakSets = std::max(data.poolStats.peakSets, data.poolStats.liveSets);
            data.poolStats.peakUniformBuffers = std::max(data.poolStats.peakUniformBuffers, data.poolStats.liveUniformBuffers);
            data.poolStats.peakSampledImages = std::max(data.poolStats.peakSampledImages, data.poolStats.liveSampledImages);
            data.poolStats.peakStorageBuffers = std::max(data.poolStats.peakStorageBuffers, data.poolStats.liveStorageBuffers);
            data.poolStats.peakStorageImages = std::max(data.poolStats.peakStorageImages, data.poolStats.liveStorageImages);
            data.poolStats.peakSamplers = std::max(data.poolStats.peakSamplers, data.poolStats.liveSamplers);
        }

        void DescriptorHandlerVK::BindDescriptor(DescriptorSetID setID, u32 binding, BufferID bufferID, DescriptorType type, u32 frameIndex)
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = *data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];

            BufferID::type newBufferIndex = static_cast<BufferID::type>(bufferID);

            // Check if this binding already has a buffer bound
            auto it = descriptorSet.bindingToBuffer.find(binding);
            bool contentChanged = it == descriptorSet.bindingToBuffer.end() || it->second != bufferID;
            if (contentChanged)
            {
                WarnIfBoundThisFrame(descriptorSet, binding);
            }
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

            // [Frame-safe descriptor rebind] A GPUVector resize swaps in a NEW VkBuffer, BufferIDs are
            // recycled, and two frames are in flight sharing this set's per-frame descriptor copies. The only
            // moment at which writing a slot's copy is both safe (the slot's previous frame is fully done, so
            // no in-flight read races the write under UPDATE_AFTER_BIND) and current is right after that slot's
            // fence has been waited. So record the desired buffer per (slot, binding) here and apply it in
            // FlushPendingBufferWrites, which runs in FlipFrame immediately after that fence wait. The actual
            // VkBuffer is resolved from bufferID at flush time, so it always reflects the latest generation.
            descriptorSet.pendingBufferWritesPerSlot[frameIndex][binding] = PendingBufferWrite{ bufferID, type };
        }

        void DescriptorHandlerVK::WriteBufferDescriptor(VkDescriptorSet dstSet, u32 binding, VkBuffer buffer, DescriptorType type)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet descriptorWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = dstSet;
            descriptorWrite.dstBinding = binding;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.descriptorType = FormatConverterVK::ToVkDescriptorType(type);
            descriptorWrite.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(_device->_device, 1, &descriptorWrite, 0, nullptr);
        }

        void DescriptorHandlerVK::WriteBufferDescriptor(DescriptorSet& descriptorSet, u32 binding, VkBuffer buffer, DescriptorType type, u32 frameIndex)
        {
            WriteBufferDescriptor(descriptorSet.sets[frameIndex], binding, buffer, type);
        }

        void DescriptorHandlerVK::FlushPendingBufferWrites(u32 frameIndex)
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);

            for (const std::unique_ptr<DescriptorSet>& descriptorSetPtr : data.descriptorSets)
            {
                if (!descriptorSetPtr)
                    continue;
                DescriptorSet& descriptorSet = *descriptorSetPtr;
                auto& pending = descriptorSet.pendingBufferWritesPerSlot[frameIndex];
                for (auto& [binding, write] : pending)
                {
                    VkBuffer buffer = _bufferHandler->GetBuffer(write.bufferID);
                    WriteBufferDescriptor(descriptorSet, binding, buffer, write.type, frameIndex);
                }
                pending.clear();
            }
        }

        u32 DescriptorHandlerVK::SnapshotTempDescriptorSet(DescriptorSetID setID, u32 frameIndex)
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = *data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];

            NC_ASSERT(!descriptorSet.hasVariableBinding, "DescriptorHandlerVK::SnapshotTempDescriptorSet: Sets with variable-count bindings are not supported");

            VkDescriptorSetAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
            allocInfo.descriptorPool = data.framePools[frameIndex];
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &descriptorSet.layout;

            VkDescriptorSet tempSet;
            VkResult result = vkAllocateDescriptorSets(_device->_device, &allocInfo, &tempSet);
            if (result != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("DescriptorHandlerVK::SnapshotTempDescriptorSet: Failed to allocate temp descriptor set! You probably need to increase maxTempDescriptorSetsPerFrame or the frame pool sizes.");
            }

            // Copy every written binding from the canonical frame copy. Pending buffer binds are skipped
            // here (their canonical descriptor may never have been written) and applied directly below.
            auto& pendingBufferWrites = descriptorSet.pendingBufferWritesPerSlot[frameIndex];

            std::vector<VkCopyDescriptorSet> copies;
            copies.reserve(descriptorSet.reflection.descriptors.size());

            for (auto& [_, descriptor] : descriptorSet.reflection.descriptors)
            {
                if (descriptorSet.unboundBindings.Has(descriptor.binding))
                    continue;

                if (pendingBufferWrites.find(descriptor.binding) != pendingBufferWrites.end())
                    continue;

                u32 count = 1;
                if (descriptor.count != 1)
                {
                    // Fixed-size array, only copy the elements that have been written
                    auto countIt = descriptorSet.bindingWrittenCounts.find(descriptor.binding);
                    count = countIt != descriptorSet.bindingWrittenCounts.end() ? countIt->second : 1;
                }

                VkCopyDescriptorSet& copy = copies.emplace_back();
                copy = { VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET };
                copy.srcSet = descriptorSet.sets[frameIndex];
                copy.srcBinding = descriptor.binding;
                copy.dstSet = tempSet;
                copy.dstBinding = descriptor.binding;
                copy.descriptorCount = count;
            }

            if (!copies.empty())
            {
                vkUpdateDescriptorSets(_device->_device, 0, nullptr, static_cast<u32>(copies.size()), copies.data());
            }

            // A temp set is never in flight, so buffer binds recorded this frame (which the canonical copy
            // only receives at its next flush) can be applied immediately
            for (auto& [binding, write] : pendingBufferWrites)
            {
                VkBuffer buffer = _bufferHandler->GetBuffer(write.bufferID);
                WriteBufferDescriptor(tempSet, binding, buffer, write.type);
            }

            u32 transientSetIndex = static_cast<u32>(data.transientSets[frameIndex].size());
            data.transientSets[frameIndex].push_back(tempSet);
            return transientSetIndex;
        }

        VkDescriptorSet DescriptorHandlerVK::GetTransientVkDescriptorSet(u32 transientSetIndex, u32 frameIndex)
        {
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            return data.transientSets[frameIndex][transientSetIndex];
        }

        void DescriptorHandlerVK::MarkBound(DescriptorSetID setID)
        {
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            data.descriptorSets[static_cast<DescriptorSetID::type>(setID)]->lastBoundGeneration = _frameGeneration;
        }

        // Rewriting a descriptor that a set bound earlier in this frame's recording still references means
        // the earlier bind sees the new contents when the GPU executes (UPDATE_AFTER_BIND reads at
        // execution time). Warn once per set per frame and point at the safe alternative.
        void DescriptorHandlerVK::WarnIfBoundThisFrame(DescriptorSet& descriptorSet, u32 binding)
        {
            if (!_inFrameRecording || descriptorSet.lastBoundGeneration != _frameGeneration || descriptorSet.lastWarnGeneration == _frameGeneration)
                return;

            descriptorSet.lastWarnGeneration = _frameGeneration;
            NC_LOG_ERROR("DescriptorHandlerVK: Binding {} ({}) was rewritten after its descriptor set was already bound this frame, the earlier bind will see the new contents. Use CommandList::BindTempDescriptorSet for per-dispatch descriptor state.", binding, GetBindingName(descriptorSet, binding));
        }

        void DescriptorHandlerVK::FlipFrame(u32 frameIndex)
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);

            _frameGeneration++;
            _inFrameRecording = true;

            if (!data.transientSets[frameIndex].empty())
            {
                vkResetDescriptorPool(_device->_device, data.framePools[frameIndex], 0);
                data.transientSets[frameIndex].clear();
            }
        }

        void DescriptorHandlerVK::OnFrameEnd()
        {
            _inFrameRecording = false;
        }

        void DescriptorHandlerVK::BindDescriptor(DescriptorSetID setID, u32 binding, VkImageView image, DescriptorType type, bool isRT, u32 frameIndex)
        {
            ZoneScoped;
            DescriptorHandlerData& data = *static_cast<DescriptorHandlerData*>(_data);
            DescriptorSet& descriptorSet = *data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);
            WarnIfBoundThisFrame(descriptorSet, binding);

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
            DescriptorSet& descriptorSet = *data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);
            WarnIfBoundThisFrame(descriptorSet, binding);

            u32& writtenCount = descriptorSet.bindingWrittenCounts[binding];
            writtenCount = std::max(writtenCount, arrayOffset + 1);

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
            DescriptorSet& descriptorSet = *data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);
            WarnIfBoundThisFrame(descriptorSet, binding);

            u32 count = static_cast<u32>(images.size());

            u32& writtenCount = descriptorSet.bindingWrittenCounts[binding];
            writtenCount = std::max(writtenCount, arrayOffset + count);

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
            DescriptorSet& descriptorSet = *data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
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
            DescriptorSet& descriptorSet = *data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
            descriptorSet.unboundBindings.Unset(binding);

            u32& writtenCount = descriptorSet.bindingWrittenCounts[binding];
            writtenCount = std::max(writtenCount, arrayIndex + 1);

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
            DescriptorSet& descriptorSet = *data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
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
            DescriptorSet& descriptorSet = *data.descriptorSets[static_cast<DescriptorSetID::type>(setID)];
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
