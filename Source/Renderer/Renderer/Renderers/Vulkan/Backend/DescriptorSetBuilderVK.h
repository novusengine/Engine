#pragma once
#include "DescriptorAllocatorVK.h"
#include "ShaderHandlerVK.h"
#include "Renderer/Descriptors/GraphicsPipelineDesc.h"
#include "Renderer/Descriptors/ComputePipelineDesc.h"
#include "Renderer/Descriptors/BufferDesc.h"

#include <Base/Types.h>

#include <robinhood/robinhood.h>
#include <vulkan/vulkan_core.h>

#include <vector>

namespace Memory
{
    class Allocator;
}

class BitSet;

namespace Renderer
{
    class TrackedBufferBitSets;

    namespace Backend
    {
        class RenderDeviceVK;
        class PipelineHandlerVK;
        class ShaderHandlerVK;
        class BufferHandlerVK;
        
        enum class DescriptorLifetime 
        {
            Static,
            PerFrame
        };

        struct DescriptorMegaPoolVK;

        class DescriptorSetBuilderVK
        {
        public:
            DescriptorSetBuilderVK(::Memory::Allocator* allocator, GraphicsPipelineID pipelineID, PipelineHandlerVK* pipelineHandler, ShaderHandlerVK* shaderHandler, BufferHandlerVK* bufferHandler, DescriptorMegaPoolVK* parentPool);
            DescriptorSetBuilderVK(::Memory::Allocator* allocator, ComputePipelineID pipelineID, PipelineHandlerVK* pipelineHandler, ShaderHandlerVK* shaderHandler, BufferHandlerVK* bufferHandler, DescriptorMegaPoolVK* parentPool);

            void InitReflectData();
            void SetBufferPermissions(const TrackedBufferBitSets* bufferPermissions);

            void BindSampler(i32 set, i32 binding, VkDescriptorImageInfo& imageInfo);
            void BindSampler(u32 nameHash, VkDescriptorImageInfo& imageInfo);

            void BindImage(i32 set, i32 binding, const VkDescriptorImageInfo& imageInfo, bool imageWrite = false);
            void BindImage(u32 nameHash, const VkDescriptorImageInfo& imageInfo);

            void BindImageArray(i32 set, i32 binding, VkDescriptorImageInfo* images, i32 count);
            void BindImageArray(u32 nameHash, VkDescriptorImageInfo* images, i32 count);

            void BindImageArrayIndex(i32 set, i32 binding, u32 arrayIndex, const VkDescriptorImageInfo& imageInfo, bool imageWrite = false);
            void BindImageArrayIndex(u32 nameHash, const VkDescriptorImageInfo& imageInfo, u32 arrayIndex);

            void BindStorageImage(i32 set, i32 binding, VkDescriptorImageInfo* imageInfos, i32 count);
            void BindStorageImage(u32 nameHash, VkDescriptorImageInfo* imageInfos, i32 count);

            void BindStorageImageArray(i32 set, i32 binding, VkDescriptorImageInfo* imageInfos, i32 count);
            void BindStorageImageArray(u32 nameHash, VkDescriptorImageInfo* imageInfos, i32 count);

            void BindBuffer(i32 set, i32 binding, const VkDescriptorBufferInfo& bufferInfo, VkDescriptorType bufferType);
            void BindBuffer(u32 nameHash, const VkDescriptorBufferInfo& bufferInfo, BufferID bufferID);

            void BindBufferArrayIndex(i32 set, i32 binding, u32 arrayIndex, const VkDescriptorBufferInfo& bufferInfo, VkDescriptorType bufferType);
            void BindBufferArrayIndex(u32 nameHash, const VkDescriptorBufferInfo& bufferInfo, u32 arrayIndex, BufferID bufferID);

            void BindRayStructure(i32 set, i32 binding, const VkWriteDescriptorSetAccelerationStructureKHR& info);
            void BindRayStructure(u32 nameHash, const VkWriteDescriptorSetAccelerationStructureKHR& info);

            void UpdateDescriptorSet(i32 set, VkDescriptorSet& descriptor, RenderDeviceVK& device);
            VkDescriptorSet BuildDescriptorSet(i32 set, DescriptorLifetime lifetime);

        private:
            void ValidateAccesses();

        private:
            enum class PipelineType
            {
                Graphics,
                Compute,
            };

            struct ImageWriteDescriptor
            {
                int dstSet;
                int dstBinding;
                int dstArrayIndex = -1;
                VkDescriptorType descriptorType;
                VkDescriptorImageInfo imageInfo;
                VkDescriptorImageInfo* imageArray{ nullptr };
                int imageCount;
            };

            struct BufferWriteDescriptor
            {
                int dstSet;
                int dstBinding;
                int dstArrayIndex = -1;
                VkDescriptorType descriptorType;
                VkDescriptorBufferInfo bufferInfo;
                VkWriteDescriptorSetAccelerationStructureKHR accelinfo;
            };

        private:
            ::Memory::Allocator* _allocator;

            PipelineHandlerVK* _pipelineHandler;
            ShaderHandlerVK* _shaderHandler;
            BufferHandlerVK* _bufferHandler;
            DescriptorMegaPoolVK* _parentPool;

            PipelineType _pipelineType;

            GraphicsPipelineID _graphicsPipelineID;
            ComputePipelineID _computePipelineID;

            std::vector<Backend::BindInfo> _bindInfos;
            robin_hood::unordered_map<u32, u32> _hashedNameToBindInfoIndex;
            robin_hood::unordered_map<u32, u32> _bufferIndexToBindInfoIndex;

            std::vector<ImageWriteDescriptor> _imageWrites;
            std::vector<BufferWriteDescriptor> _bufferWrites;

            const TrackedBufferBitSets* _bufferPermissions = nullptr;
            BitSet* _bufferAccesses = nullptr;
            BitSet* _bufferReadAccesses = nullptr;
            BitSet* _bufferWriteAccesses = nullptr;
        };

        struct DescriptorAllocator
        {
            i32 maxDescriptors;
            i32 current_descriptors;
            VkDescriptorPool pool;
        };

        struct DescriptorMegaPoolVK
        {
            VkDescriptorSet AllocateDescriptor(VkDescriptorSetLayout layout, DescriptorLifetime lifetime, void* next = nullptr);

            void Init(i32 numFrames, RenderDeviceVK* device);
            void SetFrame(i32 frameNumber);

            DescriptorAllocatorHandleVK _dynamicHandle;
            DescriptorAllocatorHandleVK _staticHandle;

            DescriptorAllocatorPoolVK* _dynamicAllocatorPool;
            DescriptorAllocatorPoolVK* _staticAllocatorPool;

            RenderDeviceVK* _device;
        };
    }
}