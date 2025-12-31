#pragma once

namespace Neon::RHI
{
    inline auto blitShaderSource = R"(
#type vertex
layout(location = 0) in vec2 inPosition;
layout(location = 0) out vec2 vUV;

void main()
{
    vUV = inPosition * 0.5 + 0.5;
    gl_Position = vec4(inPosition, 0.0, 1.0);
}

#type fragment

layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D blitTexture;

void main()
{
    outColor = texture(blitTexture, vUV);
}
)";
}