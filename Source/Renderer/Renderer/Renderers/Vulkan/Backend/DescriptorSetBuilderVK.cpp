#include "DescriptorSetBuilderVK.h"
#include "BufferHandlerVK.h"
#include "PipelineHandlerVK.h"
#include "RenderDeviceVK.h"
#include "ShaderHandlerVK.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/TrackedBufferBitSets.h"

#include <Base/Util/StringUtils.h>
#include <Base/Memory/StackAllocator.h>

#include <vulkan/vulkan.h>
#include <tracy/Tracy.hpp>

namespace Renderer
{
    namespace Backend
    {
        DescriptorSetBuilderVK::DescriptorSetBuilderVK(Memory::Allocator* allocator, GraphicsPipelineID pipelineID, PipelineHandlerVK* pipelineHandler, ShaderHandlerVK* shaderHandler, BufferHandlerVK* bufferHandler, DescriptorMegaPoolVK* parentPool)
            : _allocator(allocator)
        {
            _pipelineType = PipelineType::Graphics;
            _pipelineHandler = pipelineHandler;
            _shaderHandler = shaderHandler;
            _bufferHandler = bufferHandler;
            _parentPool = parentPool;
            _graphicsPipelineID = pipelineID;
        }

        DescriptorSetBuilderVK::DescriptorSetBuilderVK(Memory::Allocator* allocator, ComputePipelineID pipelineID, PipelineHandlerVK* pipelineHandler, ShaderHandlerVK* shaderHandler, BufferHandlerVK* bufferHandler, DescriptorMegaPoolVK* parentPool)
            : _allocator(allocator)
        {
            _pipelineType = PipelineType::Compute;
            _pipelineHandler = pipelineHandler;
            _shaderHandler = shaderHandler;
            _bufferHandler = bufferHandler;
            _parentPool = parentPool;
            _computePipelineID = pipelineID;
        }

        void DescriptorSetBuilderVK::InitReflectData()
        {
            ZoneScoped;

            if (_pipelineType == PipelineType::Graphics)
            {
                // Graphics pipeline
                GraphicsPipelineDesc desc = _pipelineHandler->GetDescriptor(_graphicsPipelineID);

                if (desc.states.vertexShader != VertexShaderID::Invalid())
                {
                    const Backend::BindReflection& bindReflection = _shaderHandler->GetBindReflection(desc.states.vertexShader);
                    _bindInfos.insert(_bindInfos.end(), bindReflection.dataBindings.begin(), bindReflection.dataBindings.end());
                }
                if (desc.states.pixelShader != PixelShaderID::Invalid())
                {
                    const BindReflection& bindReflection = _shaderHandler->GetBindReflection(desc.states.pixelShader);

                    // Loop over all new databindings
                    for (const BindInfo& dataBinding : bindReflection.dataBindings)
                    {
                        bool found = false;
                        // Loop over our current databindings
                        for (BindInfo& bindInfo : _bindInfos)
                        {
                            // If they occupy the same descriptor space
                            if (dataBinding.set == bindInfo.set &&
                                dataBinding.binding == bindInfo.binding)
                            {
                                // If the name, descriptorType and count matches as well we assume it matches and is fine
                                if (dataBinding.nameHash == bindInfo.nameHash &&
                                    dataBinding.descriptorType == bindInfo.descriptorType &&
                                    dataBinding.count == bindInfo.count)
                                {
                                    // Just add our stageflags to it
                                    bindInfo.stageFlags |= dataBinding.stageFlags;
                                }
                                else
                                {
                                    // Else somethings is really bad, lets fatal log
                                    DebugHandler::PrintFatal("Vertex Shader and Pixel Shader tries to use the same descriptor set and binding, but they don't seem to match");
                                }
                                found = true;
                                break;
                            }
                        }

                        // If we  didn't find a match, add it to our bindInfos
                        if (!found)
                        {
                            _bindInfos.push_back(dataBinding);
                        }
                    }
                }
            }
            else
            {
                // Compute pipeline
                ComputePipelineDesc desc = _pipelineHandler->GetDescriptor(_computePipelineID);

                const Backend::BindReflection& bindReflection = _shaderHandler->GetBindReflection(desc.computeShader);
                _bindInfos.insert(_bindInfos.end(), bindReflection.dataBindings.begin(), bindReflection.dataBindings.end());
            }

            for(u32 i = 0; i < _bindInfos.size(); i++)
            {
                _hashedNameToBindInfoIndex.insert_or_assign(_bindInfos[i].nameHash, i);
            }

        }

        void DescriptorSetBuilderVK::SetBufferPermissions(const TrackedBufferBitSets* bufferPermissions)
        {
            ZoneScoped;

            _bufferPermissions = bufferPermissions;

            u32 numBitSets = _bufferPermissions->GetNumBufferSets();
            _bufferReadAccesses = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
            _bufferWriteAccesses = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
            _bufferAccesses = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
        }

        void DescriptorSetBuilderVK::BindSampler(i32 set, i32 binding, VkDescriptorImageInfo& imageInfo)
        {
            ZoneScoped;

            for (auto& imageWrite : _imageWrites)
            {
                if (imageWrite.dstBinding == binding && imageWrite.dstSet == set)
                {
                    imageWrite.imageInfo = imageInfo;
                    return;
                }
            }

            ImageWriteDescriptor newWrite;
            newWrite.dstSet = set;
            newWrite.dstBinding = binding;
            newWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            newWrite.imageInfo = imageInfo;
            
            _imageWrites.push_back(newWrite);
        }

        void DescriptorSetBuilderVK::BindSampler(u32 nameHash, VkDescriptorImageInfo& imageInfo)
        {
            ZoneScoped;

            DebugHandler::Assert(imageInfo.sampler != nullptr, "DescriptorSetBuilderVK : BindSampler was passed an imageInfo with a nullptr sampler");

            if (!_hashedNameToBindInfoIndex.contains(nameHash))
                return;

            u32 bindInfoIndex = _hashedNameToBindInfoIndex[nameHash];
            BindInfo& bindInfo = _bindInfos[bindInfoIndex];

            BindSampler(bindInfo.set, bindInfo.binding, imageInfo);
        }

        void DescriptorSetBuilderVK::BindImage(i32 set, i32 binding, const VkDescriptorImageInfo& imageInfo, bool imageWrite)
        {
            ZoneScoped;

            for (auto& imageWrite : _imageWrites) 
            {
                if (imageWrite.dstBinding == binding && imageWrite.dstSet == set)
                {
                    imageWrite.imageInfo = imageInfo;
                    return;
                }
            }

            ImageWriteDescriptor newWrite;
            newWrite.dstSet = set;
            newWrite.dstBinding = binding;
            newWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            newWrite.imageInfo = imageInfo;
            if (imageWrite)
            {
                newWrite.imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }

            _imageWrites.push_back(newWrite);
        }

        void DescriptorSetBuilderVK::BindImage(u32 nameHash, const VkDescriptorImageInfo& imageInfo)
        {
            ZoneScoped;

            if (!_hashedNameToBindInfoIndex.contains(nameHash))
                return;

            u32 bindInfoIndex = _hashedNameToBindInfoIndex[nameHash];
            BindInfo& bindInfo = _bindInfos[bindInfoIndex];

            BindImage(bindInfo.set, bindInfo.binding, imageInfo);
        }

        void DescriptorSetBuilderVK::BindImageArray(i32 set, i32 binding, VkDescriptorImageInfo* images, i32 count)
        {
            ZoneScoped;

            for (auto& imageWrite : _imageWrites)
            {
                if (imageWrite.dstBinding == binding && imageWrite.dstSet == set)
                {
                    imageWrite.imageArray = images;
                    imageWrite.imageCount = count;
                    return;
                }
            }

            ImageWriteDescriptor newWrite;
            newWrite.dstSet = set;
            newWrite.dstBinding = binding;
            newWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            newWrite.imageArray = images;
            newWrite.imageCount = count;

            _imageWrites.push_back(newWrite);
        }

        void DescriptorSetBuilderVK::BindImageArray(u32 nameHash, VkDescriptorImageInfo* images, i32 count)
        {
            ZoneScoped;

            if (!_hashedNameToBindInfoIndex.contains(nameHash))
                return;

            u32 bindInfoIndex = _hashedNameToBindInfoIndex[nameHash];
            BindInfo& bindInfo = _bindInfos[bindInfoIndex];

            BindImageArray(bindInfo.set, bindInfo.binding, images, count);
        }

        void DescriptorSetBuilderVK::BindImageArrayIndex(i32 set, i32 binding, u32 arrayIndex, const VkDescriptorImageInfo& imageInfo, bool imageWrite)
        {
            ZoneScoped;

            for (auto& imageWrite : _imageWrites)
            {
                if (imageWrite.dstBinding == binding && imageWrite.dstSet == set && imageWrite.dstArrayIndex == arrayIndex)
                {
                    imageWrite.imageInfo = imageInfo;
                    return;
                }
            }

            ImageWriteDescriptor newWrite;
            newWrite.dstSet = set;
            newWrite.dstBinding = binding;
            newWrite.dstArrayIndex = arrayIndex;
            newWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            newWrite.imageInfo = imageInfo;
            if (imageWrite)
            {
                newWrite.imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }

            _imageWrites.push_back(newWrite);
        }

        void DescriptorSetBuilderVK::BindImageArrayIndex(u32 nameHash, const VkDescriptorImageInfo& imageInfo, u32 arrayIndex)
        {
            ZoneScoped;

            if (!_hashedNameToBindInfoIndex.contains(nameHash))
                return;

            u32 bindInfoIndex = _hashedNameToBindInfoIndex[nameHash];
            BindInfo& bindInfo = _bindInfos[bindInfoIndex];
            
            BindImageArrayIndex(bindInfo.set, bindInfo.binding, arrayIndex, imageInfo);
        }
        
        void DescriptorSetBuilderVK::BindStorageImage(i32 set, i32 binding, VkDescriptorImageInfo* imageInfos, i32 count)
        {
            ZoneScoped;

            ImageWriteDescriptor newWrite;
            newWrite.dstSet = set;
            newWrite.dstBinding = binding;
            newWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

            if (count == 1)
            {
                for (auto& imageWrite : _imageWrites)
                {
                    if (imageWrite.dstBinding == binding && imageWrite.dstSet == set)
                    {
                        imageWrite.imageInfo = imageInfos[0];
                        return;
                    }
                }

                newWrite.imageInfo = imageInfos[0];
            }
            else
            {
                for (auto& imageWrite : _imageWrites)
                {
                    if (imageWrite.dstBinding == binding && imageWrite.dstSet == set)
                    {
                        imageWrite.imageArray = imageInfos;
                        newWrite.imageCount = count;
                        return;
                    }
                }

                newWrite.imageArray = imageInfos;
                newWrite.imageCount = count;
            }

            _imageWrites.push_back(newWrite);
        }

        void DescriptorSetBuilderVK::BindStorageImage(u32 nameHash, VkDescriptorImageInfo* imageInfos, i32 count)
        {
            ZoneScoped;

            if (!_hashedNameToBindInfoIndex.contains(nameHash))
                return;

            u32 bindInfoIndex = _hashedNameToBindInfoIndex[nameHash];
            BindInfo& bindInfo = _bindInfos[bindInfoIndex];

            BindStorageImage(bindInfo.set, bindInfo.binding, imageInfos, count);
        }

        void DescriptorSetBuilderVK::BindStorageImageArray(u32 nameHash, VkDescriptorImageInfo* imageInfos, i32 count)
        {
            ZoneScoped;

            if (!_hashedNameToBindInfoIndex.contains(nameHash))
                return;

            u32 bindInfoIndex = _hashedNameToBindInfoIndex[nameHash];
            BindInfo& bindInfo = _bindInfos[bindInfoIndex];

            BindStorageImageArray(bindInfo.set, bindInfo.binding, imageInfos, count);
        }

        void DescriptorSetBuilderVK::BindStorageImageArray(i32 set, i32 binding, VkDescriptorImageInfo* imageInfos, i32 count)
        {
            ZoneScoped;

            for (auto& imageWrite : _imageWrites)
            {
                if (imageWrite.dstBinding == binding && imageWrite.dstSet == set)
                {
                    imageWrite.imageArray = imageInfos;
                    imageWrite.imageCount = count;
                    return;
                }
            }

            ImageWriteDescriptor newWrite;
            newWrite.dstSet = set;
            newWrite.dstBinding = binding;
            newWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            newWrite.imageArray = imageInfos;
            newWrite.imageCount = count;

            _imageWrites.push_back(newWrite);
        }

        void DescriptorSetBuilderVK::BindBuffer(i32 set, i32 binding, const VkDescriptorBufferInfo& bufferInfo, VkDescriptorType bufferType)
        {
            ZoneScoped;

            for (auto& bufferWrite : _bufferWrites) 
            {
                if (bufferWrite.dstBinding == binding && bufferWrite.dstSet == set)
                {
                    bufferWrite.bufferInfo = bufferInfo;
                    bufferWrite.descriptorType = bufferType;
                    return;
                }
            }

            BufferWriteDescriptor newWrite;
            newWrite.dstSet = set;
            newWrite.dstBinding = binding;
            newWrite.descriptorType = bufferType;
            newWrite.bufferInfo = bufferInfo;

            _bufferWrites.push_back(newWrite);
        }

        void DescriptorSetBuilderVK::BindBuffer(u32 nameHash, const VkDescriptorBufferInfo& bufferInfo, BufferID bufferID)
        {
            ZoneScoped;

            if (!_hashedNameToBindInfoIndex.contains(nameHash))
                return;

            u32 bindInfoIndex = _hashedNameToBindInfoIndex[nameHash];
            BindInfo& bindInfo = _bindInfos[bindInfoIndex];

            BindBuffer(bindInfo.set, bindInfo.binding, bufferInfo, bindInfo.descriptorType);

            BufferID::type bufferIndex = static_cast<BufferID::type>(bufferID);

            _bufferAccesses->Set(bufferIndex);
            if (bindInfo.isWrite)
            {
                _bufferWriteAccesses->Set(bufferIndex);
            }
            else
            {
                _bufferReadAccesses->Set(bufferIndex);
            }

            _bufferIndexToBindInfoIndex[bufferIndex] = bindInfoIndex;
        }

        void DescriptorSetBuilderVK::BindBufferArrayIndex(i32 set, i32 binding, u32 arrayIndex, const VkDescriptorBufferInfo& bufferInfo, VkDescriptorType bufferType)
        {
            ZoneScoped;

            for (auto& bufferWrite : _bufferWrites)
            {
                if (bufferWrite.dstBinding == binding && bufferWrite.dstSet == set && bufferWrite.dstArrayIndex == arrayIndex)
                {
                    bufferWrite.bufferInfo = bufferInfo;
                    bufferWrite.descriptorType = bufferType;
                    return;
                }
            }

            BufferWriteDescriptor newWrite;
            newWrite.dstSet = set;
            newWrite.dstBinding = binding;
            newWrite.dstArrayIndex = arrayIndex;
            newWrite.descriptorType = bufferType;
            newWrite.bufferInfo = bufferInfo;

            _bufferWrites.push_back(newWrite);
        }

        void DescriptorSetBuilderVK::BindBufferArrayIndex(u32 nameHash, const VkDescriptorBufferInfo& bufferInfo, u32 arrayIndex, BufferID bufferID)
        {
            ZoneScoped;

            if (!_hashedNameToBindInfoIndex.contains(nameHash))
                return;

            u32 bindInfoIndex = _hashedNameToBindInfoIndex[nameHash];
            BindInfo& bindInfo = _bindInfos[bindInfoIndex];

            BindBufferArrayIndex(bindInfo.set, bindInfo.binding, arrayIndex, bufferInfo, bindInfo.descriptorType);

            BufferID::type bufferIndex = static_cast<BufferID::type>(bufferID);

            _bufferAccesses->Set(bufferIndex);
            if (bindInfo.isWrite)
            {
                _bufferWriteAccesses->Set(bufferIndex);
            }
            else
            {
                _bufferReadAccesses->Set(bufferIndex);
            }

            _bufferIndexToBindInfoIndex[bufferIndex] = bindInfoIndex;
        }

        void DescriptorSetBuilderVK::BindRayStructure(i32 set, i32 binding, const VkWriteDescriptorSetAccelerationStructureKHR& info)
        {
            ZoneScoped;

            for (auto& bufferWrite : _bufferWrites)
            {
                if (bufferWrite.dstBinding == binding && bufferWrite.dstSet == set)
                {
                    bufferWrite.accelinfo = info;
                    return;
                }
            }

            BufferWriteDescriptor newWrite;
            newWrite.dstSet = set;
            newWrite.dstBinding = binding;
            newWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
            newWrite.accelinfo = info;

            _bufferWrites.push_back(newWrite);
        }

        void DescriptorSetBuilderVK::BindRayStructure(u32 nameHash, const VkWriteDescriptorSetAccelerationStructureKHR& info)
        {
            ZoneScoped;

            if (!_hashedNameToBindInfoIndex.contains(nameHash))
                return;

            u32 bindInfoIndex = _hashedNameToBindInfoIndex[nameHash];
            BindInfo& bindInfo = _bindInfos[bindInfoIndex];

            BindRayStructure(bindInfo.set, bindInfo.binding, info);
        }

        void DescriptorSetBuilderVK::UpdateDescriptorSet(i32 set, VkDescriptorSet& descriptor, RenderDeviceVK& device)
        {
            ZoneScoped;

            std::vector<VkWriteDescriptorSet> descriptorWrites;
            descriptorWrites.reserve(20);

            for (ImageWriteDescriptor& imageWrite : _imageWrites) 
            {
                if (imageWrite.dstSet == set) 
                {
                    VkWriteDescriptorSet newWrite = {};
                    newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    newWrite.pNext = nullptr;

                    newWrite.dstBinding = imageWrite.dstBinding;
                    newWrite.dstSet = descriptor;
                    newWrite.descriptorCount = 1;
                    newWrite.descriptorType = imageWrite.descriptorType;
                    newWrite.pImageInfo = &imageWrite.imageInfo;

                    //special case for the image arrays
                    if (imageWrite.imageArray != nullptr)
                    {
                        newWrite.descriptorCount = imageWrite.imageCount;
                        newWrite.pImageInfo = imageWrite.imageArray;
                    }
                    // special case for image arrays that only write to individual indices
                    if (imageWrite.dstArrayIndex != -1)
                    {
                        newWrite.dstArrayElement = imageWrite.dstArrayIndex;
                    }

                    descriptorWrites.push_back(newWrite);
                }
            }

            for (BufferWriteDescriptor& bufferWrite : _bufferWrites) 
            {
                if (bufferWrite.dstSet == set) 
                {
                    VkWriteDescriptorSet newWrite = {};
                    newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    newWrite.pNext = nullptr;

                    newWrite.dstBinding = bufferWrite.dstBinding;
                    newWrite.dstSet = descriptor;
                    newWrite.descriptorCount = 1;
                    newWrite.descriptorType = bufferWrite.descriptorType;
                    newWrite.pBufferInfo = &bufferWrite.bufferInfo;

                    if (bufferWrite.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR)
                    {
                        newWrite.pBufferInfo = nullptr;
                        newWrite.pNext = &bufferWrite.accelinfo;
                    }
                    if (bufferWrite.dstArrayIndex != -1)
                    {
                        newWrite.dstArrayElement = bufferWrite.dstArrayIndex;
                    }

                    descriptorWrites.push_back(newWrite);
                }
            }

            vkUpdateDescriptorSets(device._device, static_cast<u32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }

        VkDescriptorSet DescriptorSetBuilderVK::BuildDescriptorSet(i32 set, DescriptorLifetime lifetime)
        {
            ZoneScoped;

            VkDescriptorSetLayout* layout; 
            if (_pipelineType == PipelineType::Graphics)
            {
                layout = &_pipelineHandler->GetDescriptorSetLayout(_graphicsPipelineID, set);
            }
            else
            {
                layout = &_pipelineHandler->GetDescriptorSetLayout(_computePipelineID, set);
            }

            void* next = nullptr;
            u32 counts[1];
            counts[0] = Settings::MAX_TEXTURES;

            VkDescriptorSetVariableDescriptorCountAllocateInfo setCounts = {};
            setCounts.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
            setCounts.pNext = nullptr;
            setCounts.descriptorSetCount = 1;
            setCounts.pDescriptorCounts = counts;

            for (const ImageWriteDescriptor& imageWrite : _imageWrites)
            {
                if (imageWrite.descriptorType != VK_DESCRIPTOR_TYPE_STORAGE_IMAGE && imageWrite.imageArray != nullptr && imageWrite.dstSet == set)
                {
                    counts[0] = imageWrite.imageCount;
                    next = &setCounts;
                }
            }

            if (_bufferPermissions)
            {
                ValidateAccesses();
            }

            VkDescriptorSet newSet = _parentPool->AllocateDescriptor(*layout, lifetime, next);
            UpdateDescriptorSet(set, newSet, *_parentPool->_device);
            return newSet;
        }

        void DescriptorSetBuilderVK::ValidateAccesses()
        {
            ZoneScoped;

            bool didError = false;

            const BitSet& readPermissions = _bufferPermissions->GetReadBitSet();
            if (!_bufferReadAccesses->IsSubsetOf(readPermissions))
            {
                DebugHandler::Print("\n\n--- READS ---");

                BitSet* subtracted = _bufferReadAccesses->NewBitwiseUnset(readPermissions);

                subtracted->ForEachSetBit([&](u32 set, u32 bit) 
                {
                    u32 bufferIndex = set * 64 + bit;
                    BufferID bufferID = BufferID(bufferIndex);

                    const std::string& bufferName = _bufferHandler->GetBufferName(bufferID);

                    u32 bindInfoIndex = _bufferIndexToBindInfoIndex[bufferIndex];
                    BindInfo& bindInfo = _bindInfos[bindInfoIndex];
                    
                    DebugHandler::PrintError("DescriptorSetBuilderVK : Tried to read from BufferID {} (Buffer Name: {}, Binding Name: {}), but RenderPass does not have READ access for that", bufferIndex, bufferName, bindInfo.name);
                    didError = true;
                });
            }

            const BitSet& writePermissions = _bufferPermissions->GetWriteBitSet();
            if (!_bufferWriteAccesses->IsSubsetOf(writePermissions))
            {
                DebugHandler::Print("\n\n--- WRITES ---");

                BitSet* subtracted = _bufferWriteAccesses->NewBitwiseUnset(writePermissions);

                subtracted->ForEachSetBit([&](u32 set, u32 bit)
                {
                    u32 bufferIndex = set * 64 + bit;
                    BufferID bufferID = BufferID(bufferIndex);

                    const std::string& bufferName = _bufferHandler->GetBufferName(bufferID);

                    u32 bindInfoIndex = _bufferIndexToBindInfoIndex[bufferIndex];
                    BindInfo& bindInfo = _bindInfos[bindInfoIndex];

                    DebugHandler::PrintError("DescriptorSetBuilderVK : Tried to write to BufferID {} (Buffer Name: {}, Binding Name: {}), but RenderPass does not have WRITE access for that", bufferIndex, bufferName, bindInfo.name);
                    didError = true;
                });
            }

            bool isGraphics = _pipelineType == PipelineType::Graphics;
            const BitSet& shaderStagePermissions = (isGraphics) ? _bufferPermissions->GetGraphicsBitSet() : _bufferPermissions->GetComputeBitSet();
            if (!_bufferAccesses->IsSubsetOf(shaderStagePermissions))
            {
                std::string stageName = (isGraphics) ? "GRAPHICS" : "COMPUTE";
                DebugHandler::Print("\n\n--- {} ---", stageName);

                BitSet* subtracted = _bufferAccesses->NewBitwiseUnset(shaderStagePermissions);

                subtracted->ForEachSetBit([&](u32 set, u32 bit)
                {
                    u32 bufferIndex = set * 64 + bit;
                    BufferID bufferID = BufferID(bufferIndex);

                    const std::string& bufferName = _bufferHandler->GetBufferName(bufferID);

                    u32 bindInfoIndex = _bufferIndexToBindInfoIndex[bufferIndex];
                    BindInfo& bindInfo = _bindInfos[bindInfoIndex];

                    DebugHandler::PrintError("DescriptorSetBuilderVK : Tried to use BufferID {} (Buffer Name: {}, Binding Name: {}) in a {} pipeline, but RenderPass does not have access for that", bufferIndex, bufferName, bindInfo.name, stageName);
                    didError = true;
                });
            }

            if (didError)
            {
                DebugHandler::PrintFatal("DescriptorSetBuilderVK : ValidateAccesses failed to validate the RenderPass");
            }
        }

        VkDescriptorSet DescriptorMegaPoolVK::AllocateDescriptor(VkDescriptorSetLayout layout, DescriptorLifetime lifetime, void* next)
        {
            if (lifetime == DescriptorLifetime::Static)
            {
                VkDescriptorSet set;
                _staticHandle.Allocate(layout, set, next);
                return set;
            }
            else
            {
                VkDescriptorSet set;
                _dynamicHandle.Allocate(layout, set, next);
                return set;
            }
        }

        void DescriptorMegaPoolVK::Init(i32 numFrames, RenderDeviceVK* device)
        {
            _device = device;

            _dynamicAllocatorPool = DescriptorAllocatorPoolVK::Create(device, numFrames);
            _staticAllocatorPool = DescriptorAllocatorPoolVK::Create(device, 1);
            _dynamicHandle = _dynamicAllocatorPool->GetAllocator();
            _staticHandle = _staticAllocatorPool->GetAllocator();
        }

        void DescriptorMegaPoolVK::SetFrame(i32 frameNumber)
        {
            _dynamicAllocatorPool->Flip();
            _dynamicHandle = _dynamicAllocatorPool->GetAllocator();
        }
    }
}