#pragma once
#include "VertexShaderDesc.h"
#include "PixelShaderDesc.h"
#include "MeshShaderDesc.h"
#include "TaskShaderDesc.h"
#include "ImageDesc.h"
#include "DepthImageDesc.h"
#include "RenderTargetDesc.h"
#include "Renderer/RenderStates.h"
#include "Renderer/RenderPassResources.h"

#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

#include <functional>
#include <variant>

namespace Renderer
{
    class RenderGraph;

    struct VertexPipelineStages
    {
        VertexShaderID vertexShader = VertexShaderID::Invalid();
    };

    struct MeshPipelineStages
    {
        MeshShaderID meshShader = MeshShaderID::Invalid();
        TaskShaderID taskShader = TaskShaderID::Invalid();
    };

    using GraphicsPipelineStages = std::variant<VertexPipelineStages, MeshPipelineStages>;

    struct GraphicsPipelineDesc
    {
        static const int MAX_INPUT_LAYOUTS = 8;

        // This part of the descriptor contributes to the PipelineHandler cache key
        struct States
        {
            // States
            RasterizerState rasterizerState;
            DepthStencilState depthStencilState;
            BlendState blendState;

            InputLayout inputLayouts[MAX_INPUT_LAYOUTS];
            PrimitiveTopology primitiveTopology = PrimitiveTopology::Triangles;

            // Shaders
            PixelShaderID pixelShader = PixelShaderID::Invalid();

            ImageFormat renderTargetFormats[MAX_RENDER_TARGETS] = { ImageFormat::UNKNOWN };
            DepthImageFormat depthStencilFormat = DepthImageFormat::UNKNOWN;
        };
        States states;
        GraphicsPipelineStages shaderStages;

        // Everything below this isn't hashable in the PipelineHandler since it will depend on the RenderGraph (which gets recreated every frame)
        std::string debugName = "";
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(GraphicsPipelineID, u16);
}
