#pragma once
#include <neonCore/neonCore.h>

namespace Neon::RHI
{
    class TextureView;
    class Sampler;

    struct ImGuiImage
    {
        Rc<TextureView> view;
        Rc<Sampler> sampler;
    };
}

#define ImTextureID Neon::RHI::ImGuiImage*

#define ImTextureID_Invalid nullptr