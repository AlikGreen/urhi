#pragma once

namespace Neon::RHI
{
    class TextureView;
    class Sampler;

    struct ImGuiImage
    {
        TextureView *view;
        Sampler *sampler;

        ImGuiImage(TextureView* v)
                : view(v),
                  sampler(nullptr)
        {
        }

        ImGuiImage(TextureView* v, Sampler* s)
            : view(v),
              sampler(s)
        {
        }
    };
}

#define ImTextureID Neon::RHI::ImGuiImage
