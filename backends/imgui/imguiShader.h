#pragma once

inline auto imGuiShaderSource = R"(
struct VertexInput
{
    float2 inPosition;
    float2 inUV;
    uint   inColor;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float2 vUV;
    float4 vColor;
};

cbuffer ImGuiProjection
{
    float4x4 projMatrix;
};

Texture2D<float4> ImGuiTexture;
SamplerState ImGuiSampler;

float4 unpackUnorm4x8(uint c)
{
    float r = (c & 0xFFu) / 255.0f;
    float g = ((c >> 8) & 0xFFu) / 255.0f;
    float b = ((c >> 16) & 0xFFu) / 255.0f;
    float a = ((c >> 24) & 0xFFu) / 255.0f;
    return float4(r, g, b, a);
}

[shader("vertex")]
VertexOutput vertexMain(VertexInput input)
{
    VertexOutput output;

    output.vUV = input.inUV;
    output.vColor = unpackUnorm4x8(input.inColor);
    output.position = mul(projMatrix, float4(input.inPosition, 0.0, 1.0));

    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexOutput inV)
{
    float4 texColor = ImGuiTexture.Sample(ImGuiSampler, inV.vUV);
    return inV.vColor * texColor;
}
)";