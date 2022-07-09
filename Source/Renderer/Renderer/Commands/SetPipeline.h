#pragma once
#include "Renderer/Descriptors/GraphicsPipelineDesc.h"
#include "Renderer/Descriptors/ComputePipelineDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Commands
    {
        struct BeginGraphicsPipeline
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            GraphicsPipelineID pipeline = GraphicsPipelineID::Invalid();
        };

        struct EndGraphicsPipeline
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            GraphicsPipelineID pipeline = GraphicsPipelineID::Invalid();
        };
        
        struct BeginComputePipeline
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            ComputePipelineID pipeline = ComputePipelineID::Invalid();
        };

        struct EndComputePipeline
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            ComputePipelineID pipeline = ComputePipelineID::Invalid();
        };
    }
}