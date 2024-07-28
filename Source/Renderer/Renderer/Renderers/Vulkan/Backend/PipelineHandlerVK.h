#pragma once
#include "Renderer/Descriptors/GraphicsPipelineDesc.h"
#include "Renderer/Descriptors/ComputePipelineDesc.h"

#include <Base/Types.h>

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Memory
{
    class Allocator;
}

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class ShaderHandlerVK;
        class ImageHandlerVK;
        class BufferHandlerVK;
        class DescriptorSetBuilderVK;
        struct GraphicsPipeline;

        struct DescriptorSetLayoutData
        {
            VkDescriptorSetLayoutCreateInfo createInfo;
            std::vector<VkDescriptorSetLayoutBinding> bindings;
        };

        struct IPipelineHandlerVKData {};

        class PipelineHandlerVK
        {
            using gIDType = type_safe::underlying_type<GraphicsPipelineID>;
            using cIDType = type_safe::underlying_type<ComputePipelineID>;

        public:
            void Init(Memory::Allocator* allocator, RenderDeviceVK* device, ShaderHandlerVK* shaderHandler, ImageHandlerVK* imageHandler, BufferHandlerVK* bufferHandler);
            void DiscardPipelines();

            GraphicsPipelineID CreatePipeline(const GraphicsPipelineDesc& desc);
            ComputePipelineID CreatePipeline(const ComputePipelineDesc& desc);

            const GraphicsPipelineDesc& GetDescriptor(GraphicsPipelineID id);
            const ComputePipelineDesc& GetDescriptor(ComputePipelineID id);

            // Be careful with these
            GraphicsPipelineDesc& GetMutableDescriptor(GraphicsPipelineID id);
            ComputePipelineDesc& GetMutableDescriptor(ComputePipelineID id);

            VkPipeline GetPipeline(GraphicsPipelineID id);
            VkPipeline GetPipeline(ComputePipelineID id);

            VkRenderPass GetRenderPass(GraphicsPipelineID id);
            uvec2 GetRenderPassResolution(GraphicsPipelineID id);

            VkFramebuffer GetFramebuffer(GraphicsPipelineID id);

            u32 GetNumPushConstantRanges(GraphicsPipelineID id);
            u32 GetNumPushConstantRanges(ComputePipelineID id);

            const VkPushConstantRange& GetPushConstantRange(GraphicsPipelineID id, u32 index);
            const VkPushConstantRange& GetPushConstantRange(ComputePipelineID id, u32 index);

            u32 GetNumDescriptorSetLayouts(GraphicsPipelineID id);
            u32 GetNumDescriptorSetLayouts(ComputePipelineID id);

            DescriptorSetLayoutData& GetDescriptorSetLayoutData(GraphicsPipelineID id, u32 index);
            DescriptorSetLayoutData& GetDescriptorSetLayoutData(ComputePipelineID id, u32 index);

            VkDescriptorSetLayout& GetDescriptorSetLayout(GraphicsPipelineID id, u32 index);
            VkDescriptorSetLayout& GetDescriptorSetLayout(ComputePipelineID id, u32 index);

            VkPipelineLayout& GetPipelineLayout(GraphicsPipelineID id);
            VkPipelineLayout& GetPipelineLayout(ComputePipelineID id);

            DescriptorSetBuilderVK& GetDescriptorSetBuilder(GraphicsPipelineID id);
            DescriptorSetBuilderVK& GetDescriptorSetBuilder(ComputePipelineID id);

        private:
            u64 CalculateCacheDescHash(const GraphicsPipelineDesc& desc);
            u64 CalculateCacheDescHash(const ComputePipelineDesc& desc);
            bool TryFindExistingGPipeline(u64 descHash, size_t& id);
            bool TryFindExistingCPipeline(u64 descHash, size_t& id);
            DescriptorSetLayoutData& GetDescriptorSet(i32 setNumber, std::vector<DescriptorSetLayoutData>& sets);
            
            void CreateFramebuffer(GraphicsPipeline& pipeline, const std::string& debugName);

        private:
            Memory::Allocator* _allocator;

            RenderDeviceVK* _device;
            ShaderHandlerVK* _shaderHandler;
            ImageHandlerVK* _imageHandler;
            BufferHandlerVK* _bufferHandler;

            IPipelineHandlerVKData* _data;
        };
    }
}