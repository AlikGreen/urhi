#pragma once
#include "imGuiController.h"

namespace Neon::RHI
{
    class TextureView;
    class Sampler;

    struct ImGuiImage
    {
        TextureView *view;
        Sampler *sampler;

        ImGuiImage()
                : view(nullptr),
                  sampler(nullptr) { }

        ImGuiImage(TextureView* v)
                : view(v),
                  sampler(nullptr) { }

        ImGuiImage(TextureView* v, Sampler* s)
            : view(v),
              sampler(s) { }
    };

    inline bool operator==(const ImGuiImage& a, const ImGuiImage& b)
    {
        if(a.view == nullptr && b.view == nullptr) return true;
        return a.view == b.view && a.sampler == b.sampler;
    }

    inline bool operator!=(const ImGuiImage& a, const ImGuiImage& b)
    {
        return !(a == b);
    }
}

#define ImTextureID Neon::RHI::ImGuiImage

#define ImTextureID_Invalid ImTextureID()