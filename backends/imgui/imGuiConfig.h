#pragma once
#include <grl/grl.h>
#include <glm/glm.hpp>
#include <utility>

namespace urhi
{
    class TextureView;
    class Sampler;

    struct ImGuiImage
    {
        grl::Rc<TextureView> view;
        grl::Rc<Sampler> sampler;

        ImGuiImage(grl::Rc<TextureView> view, grl::Rc<Sampler> sampler)
            : view(std::move(view)), sampler(std::move(sampler)) {  }

        explicit ImGuiImage(grl::Rc<TextureView> view)
            : view(std::move(view)), sampler(nullptr) {  }

        ImGuiImage()
            : view(nullptr), sampler(nullptr) {  }
    };
}

#undef ImTextureID
#define ImTextureID urhi::ImGuiImage*

#undef ImTextureID_Invalid
#define ImTextureID_Invalid nullptr

#define IM_VEC2_CLASS_EXTRA \
ImVec2(const glm::vec2& v) { x = v.x; y = v.y; } \
operator glm::vec2() const { return glm::vec2(x, y); }

#define IM_VEC4_CLASS_EXTRA \
ImVec4(const glm::vec4& v) { x = v.x; y = v.y; z = v.z; w = v.w; } \
operator glm::vec4() const { return glm::vec4(x, y, z, w); }
