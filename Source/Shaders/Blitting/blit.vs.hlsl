
struct VSInput
{
    uint vertexID : SV_VertexID;
};

struct VSOutput
{
    float2 uv : TEXCOORD0;
    float4 position : SV_POSITION;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    output.uv = float2((input.vertexID << 1) & 2, input.vertexID & 2);
    output.position = float4(output.uv * 2.0f + -1.0f, 0.0f, 1.0f);

    return output;
}