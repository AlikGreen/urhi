#pragma once

namespace urhi
{
    inline auto blitShaderSource = R"(
struct VSInput
{
    float2 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

// Vertex shader
[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;
    output.uv = input.position * 0.5 + 0.5;
    output.position = float4(input.position, 0.0, 1.0);
    return output;
}

// Fragment shader
[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target
{
    return blitTexture.Sample(blitSampler, input.uv);
}

// Resources
Texture2D blitTexture;
SamplerState blitSampler;
)";
}