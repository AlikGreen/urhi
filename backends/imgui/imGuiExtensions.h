#pragma once
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <urhi/urhi.h>

namespace ImGui
{
    void ClearTextureCache();

    void Image(const grl::Rc<urhi::TextureView>& textureView, ImVec2 size, ImVec2 uv0 = ImVec2(0, 0), ImVec2 uv1 = ImVec2(1, 1));
    void Image(const grl::Rc<urhi::TextureView>& textureView, const grl::Rc<urhi::Sampler>& sampler, ImVec2 size, ImVec2 uv0 = ImVec2(0, 0), ImVec2 uv1 = ImVec2(1, 1));

    bool Checkbox(const char* label, bool& value);

    bool ColorEdit3(const char* label, glm::vec3& color, ImGuiColorEditFlags flags = 0);
    bool ColorEdit4(const char* label, glm::vec4& color, ImGuiColorEditFlags flags = 0);

    bool InputText(const char* label, std::string& input, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* userData = nullptr);

    bool InputInt(const char* label, int& input, int step = 1, int stepFast = 100, ImGuiInputTextFlags flags = 0);
    bool InputInt2(const char* label, glm::ivec2& input, ImGuiInputTextFlags flags = 0);
    bool InputInt3(const char* label, glm::ivec3& input, ImGuiInputTextFlags flags = 0);
    bool InputInt4(const char* label, glm::ivec4& input, ImGuiInputTextFlags flags = 0);

    bool InputFloat(const char* label, float& input, float step = 0.0f, float stepFast = 0.0f, const char* format = nullptr, ImGuiInputTextFlags flags = 0);
    bool InputFloat2(const char* label, glm::vec2& input, const char* format = nullptr, ImGuiInputTextFlags flags = 0);
    bool InputFloat3(const char* label, glm::vec3& input, const char* format = nullptr, ImGuiInputTextFlags flags = 0);
    bool InputFloat4(const char* label, glm::vec4& input, const char* format = nullptr, ImGuiInputTextFlags flags = 0);

    bool DragInt(const char* label, int& input, float speed = 1.0f, int minValue = 0, int maxValue = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool DragInt2(const char* label, glm::ivec2& input, float speed = 1.0f, int minValue = 0, int maxValue = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool DragInt3(const char* label, glm::ivec3& input, float speed = 1.0f, int minValue = 0, int maxValue = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool DragInt4(const char* label, glm::ivec4& input, float speed = 1.0f, int minValue = 0, int maxValue = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);

    bool DragFloat(const char* label, float& input, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool DragFloat2(const char* label, glm::vec2& input, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool DragFloat3(const char* label, glm::vec3& input, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool DragFloat4(const char* label, glm::vec4& input, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
}
