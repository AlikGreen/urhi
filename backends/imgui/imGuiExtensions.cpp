#include "imGuiExtensions.h"

#include <unordered_map>
#include <vector>

#include <fstream>

#include <clogr.h>

static std::unordered_map<size_t, urhi::ImGuiImage*> imguiImageMap{};

std::size_t hashTwoPointers(void const* a, void const* b) noexcept
{
    const std::size_t h1 = std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(a));
    const std::size_t h2 = std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(b));

    return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
}

namespace ImGui
{
    void ClearTextureCache()
    {
        for (const auto &entry : imguiImageMap)
        {
            delete entry.second;
        }
        imguiImageMap.clear();
    }

    void Image(const grl::Rc<urhi::TextureView>& textureView, const ImVec2 size, const ImVec2 uv0 , const ImVec2 uv1)
    {
        const size_t hash = hashTwoPointers(textureView.get(), nullptr);

        if(imguiImageMap.contains(hash))
        {
            ImGui::Image(imguiImageMap.at(hash), size, uv0, uv1);
            return;
        }

        auto* image = new urhi::ImGuiImage();
        image->view = textureView;
        image->sampler = nullptr;

        imguiImageMap.emplace(hash, image);

        ImGui::Image(image, size, uv0, uv1);
    }

    void Image(const grl::Rc<urhi::TextureView> &textureView, const grl::Rc<urhi::Sampler> &sampler,
               const ImVec2 size, const ImVec2 uv0, const ImVec2 uv1)
    {
        const size_t hash = hashTwoPointers(textureView.get(), sampler.get());
        if(imguiImageMap.contains(hash))
        {
            ImGui::Image(imguiImageMap.at(hash), size, uv0, uv1);
            return;
        }

        auto* image = new urhi::ImGuiImage();
        image->view = textureView;
        image->sampler = sampler;

        imguiImageMap.emplace(hash, image);

        ImGui::Image(image, size, uv0, uv1);
    }

    bool Checkbox(const char* label, bool& value)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::Checkbox("##chk", &value);
        ImGui::PopID();

        return changed;
    }

    bool ColorEdit3(const char* label, glm::vec3& color, ImGuiColorEditFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::ColorEdit3("##col3", &color.x, flags);
        ImGui::PopID();

        return changed;
    }

    bool ColorEdit4(const char* label, glm::vec4& color, ImGuiColorEditFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::ColorEdit4("##col4", &color.x, flags);
        ImGui::PopID();

        return changed;
    }

    bool InputText(const char* label, std::string& input, const ImGuiInputTextFlags flags, const ImGuiInputTextCallback callback, void* userData)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::InputText("##text", &input, flags, callback, userData);
        ImGui::PopID();

        return changed;
    }

    bool InputInt(const char* label, int& input, const int step, const int stepFast, const ImGuiInputTextFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::InputInt("##int", &input, step, stepFast, flags);
        ImGui::PopID();

        return changed;
    }

    bool InputInt2(const char* label, glm::ivec2& input, const ImGuiInputTextFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::InputInt2("##int2", &input.x, flags);
        ImGui::PopID();

        return changed;
    }

    bool InputInt3(const char* label, glm::ivec3& input, const ImGuiInputTextFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::InputInt3("##int3", &input.x, flags);
        ImGui::PopID();

        return changed;
    }

    bool InputInt4(const char* label, glm::ivec4& input, const ImGuiInputTextFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::InputInt4("##int4", &input.x, flags);
        ImGui::PopID();

        return changed;
    }

    bool InputFloat(const char* label, float& input, const float step, const float stepFast, const char* format, const ImGuiInputTextFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::InputFloat("##float", &input, step, stepFast, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool InputFloat2(const char* label, glm::vec2& input, const char* format, const ImGuiInputTextFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::InputFloat2("##float2", &input.x, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool InputFloat3(const char* label, glm::vec3& input, const char* format, const ImGuiInputTextFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::InputFloat3("##float3", &input.x, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool InputFloat4(const char* label, glm::vec4& input, const char* format, const ImGuiInputTextFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::InputFloat4("##float4", &input.x, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool DragInt(const char* label, int& input, const float speed, const int minValue, const int maxValue, const char* format, const ImGuiSliderFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::DragInt("##int", &input, speed, minValue, maxValue, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool DragInt2(const char* label, glm::ivec2& input, const float speed, const int minValue, const int maxValue, const char* format, const ImGuiSliderFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::DragInt2("##int2", &input.x, speed, minValue, maxValue, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool DragInt3(const char* label, glm::ivec3& input, const float speed, const int minValue, const int maxValue, const char* format, const ImGuiSliderFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::DragInt3("##int3", &input.x, speed, minValue, maxValue, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool DragInt4(const char* label, glm::ivec4& input, const float speed, const int minValue, const int maxValue, const char* format, const ImGuiSliderFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::DragInt4("##int4", &input.x, speed, minValue, maxValue, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool DragFloat(const char* label, float& input, const float speed, const float minValue, const float maxValue, const char* format, const ImGuiSliderFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::DragFloat("##float", &input, speed, minValue, maxValue, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool DragFloat2(const char* label, glm::vec2& input, const float speed, const float minValue, const float maxValue, const char* format, const ImGuiSliderFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::DragFloat2("##float2", &input.x, speed, minValue, maxValue, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool DragFloat3(const char* label, glm::vec3& input, const float speed, const float minValue, const float maxValue, const char* format, const ImGuiSliderFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::DragFloat3("##float3", &input.x, speed, minValue, maxValue, format, flags);
        ImGui::PopID();

        return changed;
    }

    bool DragFloat4(const char* label, glm::vec4& input, const float speed, const float minValue, const float maxValue, const char* format, const ImGuiSliderFlags flags)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0.0f, 0.4f);

        ImGui::PushID(label);
        const bool changed = ImGui::DragFloat4("##float4", &input.x, speed, minValue, maxValue, format, flags);
        ImGui::PopID();

        return changed;
    }
}
