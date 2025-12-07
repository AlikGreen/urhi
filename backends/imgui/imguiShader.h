#pragma once

inline auto vertexShaderSource = R"(
#version 460 core

#type vertex
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in uint inColor;

layout(std140, binding = 0) uniform ImGuiProjection
{
    mat4 projMatrix;
} proj;

layout(location = 0) out vec2 vUV;
layout(location = 1) out vec4 vColor;

void main()
{
    vUV    = inUV;
    vColor = unpackUnorm4x8(inColor); // RGBA in [0,1]
    gl_Position = proj.projMatrix * vec4(inPosition, 0.0, 1.0);
}

#type fragment
layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D ImGuiTexture;

layout(location = 0) in vec2 vUV;
layout(location = 1) in vec4 vColor;

void main()
{
    vec4 texColor = texture(ImGuiTexture, vUV);
    outColor = vColor * texColor;
}
)";