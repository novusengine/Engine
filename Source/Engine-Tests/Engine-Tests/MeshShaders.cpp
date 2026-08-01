#include <catch2/catch2.hpp>

#include <Renderer/Descriptors/GraphicsPipelineDesc.h>
#include <Renderer/RendererCapabilities.h>
#include <ShaderCooker/SlangBridge.h>

TEST_CASE("ShaderCooker compiles the advertised mesh shader profile", "[Renderer][ShaderCooker][MeshShader]")
{
    constexpr const char* source = R"(
        struct MeshVertex
        {
            float4 position : SV_Position;
        };

        [shader("mesh")]
        [numthreads(1, 1, 1)]
        [outputtopology("triangle")]
        void main(out vertices MeshVertex vertices[3], out indices uint3 triangles[1])
        {
            SetMeshOutputCounts(3, 1);
            vertices[0].position = float4(-1.0, -1.0, 0.0, 1.0);
            vertices[1].position = float4(0.0, 1.0, 0.0, 1.0);
            vertices[2].position = float4(1.0, -1.0, 0.0, 1.0);
            triangles[0] = uint3(0, 1, 2);
        }
    )";

    ShaderCooker::SlangBridge bridge(std::filesystem::current_path());
    FileFormat::ShaderInMemory shader;
    REQUIRE(bridge.Compile("MeshShader.ms.slang", source, shader));
    REQUIRE(shader.data != nullptr);
    REQUIRE(shader.size > 0);
    delete[] shader.data;
}

TEST_CASE("Mesh shader renderer descriptors have safe defaults", "[Renderer][MeshShader]")
{
    Renderer::GraphicsPipelineDesc pipeline;
    REQUIRE(std::holds_alternative<Renderer::VertexPipelineStages>(pipeline.shaderStages));
    const Renderer::VertexPipelineStages& vertexStages =
        std::get<Renderer::VertexPipelineStages>(pipeline.shaderStages);
    CHECK(vertexStages.vertexShader == Renderer::VertexShaderID::Invalid());

    pipeline.shaderStages = Renderer::MeshPipelineStages{};
    REQUIRE(std::holds_alternative<Renderer::MeshPipelineStages>(pipeline.shaderStages));
    const Renderer::MeshPipelineStages& meshStages =
        std::get<Renderer::MeshPipelineStages>(pipeline.shaderStages);
    CHECK(meshStages.taskShader == Renderer::TaskShaderID::Invalid());
    CHECK(meshStages.meshShader == Renderer::MeshShaderID::Invalid());

    Renderer::MeshShaderProperties properties;
    CHECK_FALSE(properties.meshShaderSupported);
    CHECK_FALSE(properties.taskShaderSupported);
    CHECK(properties.maxOutputVertices == 0);
    CHECK(properties.maxOutputPrimitives == 0);
}
