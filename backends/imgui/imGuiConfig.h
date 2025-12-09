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

        bool operator==(const ImGuiImage& other) const
        {
            return view == other.view && sampler == other.sampler;
        }

        bool operator!=(const ImGuiImage& a, const ImGuiImage& b) const
        {
            return a != b;
        }
    };
}

#define ImTextureID Neon::RHI::ImGuiImage

#define ImTextureID_Invalid ImTextureID()
#define ImTextureID_Compare(lhs, rhs) ((lhs) == (rhs))
#define ImTextureID_IsNil(id)         ((id).view == nullptr)
#define ImTextureID_IsValid(id)       (!ImTextureID_IsNil(id))