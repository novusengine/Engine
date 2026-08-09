#pragma once
#include "Renderer/FrameResource.h"
#include "Renderer/Descriptors/CommandListDesc.h"
#include "Renderer/Descriptors/GraphicsPipelineDesc.h"
#include "Renderer/Descriptors/ComputePipelineDesc.h"

#include <Base/Types.h>

#include <vulkan/vulkan_core.h>

namespace tracy
{
    class VkCtxManualScope;
}

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;

        struct ICommandListHandlerVKData {};

        enum QueueType
        {
            Graphics,
            Transfer,
            COUNT
        };

        class CommandListHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device);

            void FlipFrame();
            void ResetCommandBuffers();

            CommandListID BeginCommandList(QueueType queueType);
            void EndCommandList(CommandListID id, VkFence fence);

            VkCommandBuffer GetCommandBuffer(CommandListID id);

            void AddWaitSemaphore(CommandListID id, VkSemaphore semaphore);
            void AddSignalSemaphore(CommandListID id, VkSemaphore semaphore);

            void SetBoundGraphicsPipeline(CommandListID id, GraphicsPipelineID pipelineID);
            void SetBoundComputePipeline(CommandListID id, ComputePipelineID pipelineID);

            GraphicsPipelineID GetBoundGraphicsPipeline(CommandListID id);
            ComputePipelineID GetBoundComputePipeline(CommandListID id);

            i8 GetRenderPassOpenCount(CommandListID id);
            void SetRenderPassOpenCount(CommandListID id, i8 count);

            i8 GetPipelineOpenCount(CommandListID id);
            void SetPipelineOpenCount(CommandListID id, i8 count);

            // Bitmask of descriptor set slots the currently bound pipeline statically uses but which haven't been bound on this command list yet.
            // Initialized in BeginPipeline from the pipeline's used-set mask, cleared bit-by-bit by BindDescriptorSet, validated by Draw / Dispatch, reset on EndPipeline.
            u16 GetUnboundDescriptorSets(CommandListID id);
            void SetUnboundDescriptorSets(CommandListID id, u16 mask);

            tracy::VkCtxManualScope*& GetTracyScope(CommandListID id);

            VkFence GetCurrentFence();

        private:
            CommandListID CreateCommandList(QueueType queueType);

        private:

        private:
            RenderDeviceVK* _device;

            ICommandListHandlerVKData* _data;
        };
    }
}
