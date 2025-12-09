#pragma once

namespace Neon::RHI
{
    class TextureView;
    class Sampler;

    struct ImGuiImage
    {
        TextureView *view;
        Sampler *sampler;
    };
}

#define ImTextureID Neon::RHI::ImGuiImage*

#define ImTextureID_Invalid nullptr