#pragma once
#include <neonCore/neonCore.h>
#include <glm/glm.hpp>

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

#undef ImTextureID
#define ImTextureID Neon::RHI::ImGuiImage*

#undef ImTextureID_Invalid
#define ImTextureID_Invalid nullptr

#define IM_VEC2_CLASS_EXTRA \
ImVec2(const glm::vec2& v) { x = v.x; y = v.y; } \
operator glm::vec2() const { return glm::vec2(x, y); }

#define IM_VEC4_CLASS_EXTRA \
ImVec4(const glm::vec4& v) { x = v.x; y = v.y; z = v.z; w = v.w; } \
operator glm::vec4() const { return glm::vec4(x, y, z, w); }
