#include <unordered_map>
#include <vector>

#include "imGuiExtensions.h"

#include <fstream>
#include <ryml.hpp>
#include <c4/std/string.hpp>

#include "debug.h"

static std::unordered_map<size_t, Neon::RHI::ImGuiImage*> imguiImageMap{};

std::size_t hashTwoPointers(void const* a, void const* b) noexcept
{
    const std::size_t h1 = std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(a));
    const std::size_t h2 = std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(b));

    return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
}

namespace NeonGui
{
    void ClearTextureCache()
    {
        for (auto &entry : imguiImageMap)
        {
            delete entry.second;
        }
        imguiImageMap.clear();
    }

    void Image(const Neon::Rc<Neon::RHI::TextureView>& textureView, const ImVec2 size, const ImVec2 uv0 , const ImVec2 uv1)
    {
        const size_t hash = hashTwoPointers(textureView.get(), nullptr);

        if(imguiImageMap.contains(hash))
        {
            ImGui::Image(imguiImageMap.at(hash), size, uv0, uv1);
            return;
        }

        auto* image = new Neon::RHI::ImGuiImage();
        image->view = textureView;
        image->sampler = nullptr;

        imguiImageMap.emplace(hash, image);

        ImGui::Image(image, size, uv0, uv1);
    }

    void Image(const Neon::Rc<Neon::RHI::TextureView> &textureView, const Neon::Rc<Neon::RHI::Sampler> &sampler,
               const ImVec2 size, const ImVec2 uv0, const ImVec2 uv1)
    {
        const size_t hash = hashTwoPointers(textureView.get(), sampler.get());
        if(imguiImageMap.contains(hash))
        {
            ImGui::Image(imguiImageMap.at(hash), size, uv0, uv1);
            return;
        }

        auto* image = new Neon::RHI::ImGuiImage();
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

    static void SetFloat(ryml::NodeRef node, const char* key, const float value)
    {
        node[key] << value;
    }

    static void SetBool(ryml::NodeRef node, const char* key, const bool value)
    {
        node[key] << value;
    }

    static void SetInt(ryml::NodeRef node, const char* key, const int value)
    {
        node[key] << value;
    }

    static bool GetFloat(const ryml::ConstNodeRef node, const char* key, float& out)
    {
        const ryml::ConstNodeRef child = node.has_child(key) ? node[key] : ryml::NodeRef();
        if (child.invalid() || !child.has_val())
        {
            return false;
        }
        child >> out;
        return true;
    }

    static bool GetBool(const ryml::ConstNodeRef node, const char* key, bool& out)
    {
        const ryml::ConstNodeRef child = node.has_child(key) ? node[key] : ryml::NodeRef();
        if (child.invalid() || !child.has_val())
        {
            return false;
        }
        child >> out;
        return true;
    }

    static bool GetInt(const ryml::ConstNodeRef node, const char* key, int& out)
    {
        const ryml::ConstNodeRef child = node.has_child(key) ? node[key] : ryml::NodeRef();
        if (child.invalid() || !child.has_val())
        {
            return false;
        }
        child >> out;
        return true;
    }

    static ryml::NodeRef AddVec2(ryml::NodeRef parent, const char* key, const ImVec2& v)
    {
        ryml::NodeRef n = parent[key];
        n |= ryml::MAP;
        SetFloat(n, "x", v.x);
        SetFloat(n, "y", v.y);
        return n;
    }

    static ryml::NodeRef AddVec4(ryml::NodeRef parent, const char* key, const ImVec4& v)
    {
        ryml::NodeRef n = parent[key];
        n |= ryml::MAP;
        SetFloat(n, "x", v.x);
        SetFloat(n, "y", v.y);
        SetFloat(n, "z", v.z);
        SetFloat(n, "w", v.w);
        return n;
    }

    static bool ReadVec2(const ryml::ConstNodeRef parent, const char* key, ImVec2& out)
    {
        if (parent.invalid() || !parent.has_child(key))
        {
            return false;
        }

        ryml::ConstNodeRef n = parent[key];
        if (n.invalid())
        {
            return false;
        }

        return GetFloat(n, "x", out.x) && GetFloat(n, "y", out.y);
    }

    static bool ReadVec4(const ryml::ConstNodeRef parent, const char* key, ImVec4& out)
    {
        if (parent.invalid() || !parent.has_child(key))
        {
            return false;
        }

        const ryml::ConstNodeRef n = parent[key];
        if (n.invalid())
        {
            return false;
        }

        return GetFloat(n, "x", out.x) && GetFloat(n, "y", out.y) && GetFloat(n, "z", out.z) && GetFloat(n, "w", out.w);
    }

    static ryml::Tree StyleToTree(const ImGuiStyle& style)
    {
        ryml::Tree tree;
        ryml::NodeRef root = tree.rootref();
        root |= ryml::MAP;

        SetInt(root, "version", 1);

        SetFloat(root, "Alpha", style.Alpha);
        SetFloat(root, "DisabledAlpha", style.DisabledAlpha);

        AddVec2(root, "WindowPadding", style.WindowPadding);
        SetFloat(root, "WindowRounding", style.WindowRounding);
        SetFloat(root, "WindowBorderSize", style.WindowBorderSize);
        AddVec2(root, "WindowMinSize", style.WindowMinSize);
        AddVec2(root, "WindowTitleAlign", style.WindowTitleAlign);
        SetInt(root, "WindowMenuButtonPosition", static_cast<int>(style.WindowMenuButtonPosition));

        SetFloat(root, "ChildRounding", style.ChildRounding);
        SetFloat(root, "ChildBorderSize", style.ChildBorderSize);

        SetFloat(root, "PopupRounding", style.PopupRounding);
        SetFloat(root, "PopupBorderSize", style.PopupBorderSize);

        AddVec2(root, "FramePadding", style.FramePadding);
        SetFloat(root, "FrameRounding", style.FrameRounding);
        SetFloat(root, "FrameBorderSize", style.FrameBorderSize);

        AddVec2(root, "ItemSpacing", style.ItemSpacing);
        AddVec2(root, "ItemInnerSpacing", style.ItemInnerSpacing);
        AddVec2(root, "CellPadding", style.CellPadding);

        AddVec2(root, "TouchExtraPadding", style.TouchExtraPadding);
        SetFloat(root, "IndentSpacing", style.IndentSpacing);
        SetFloat(root, "ColumnsMinSpacing", style.ColumnsMinSpacing);

        SetFloat(root, "ScrollbarSize", style.ScrollbarSize);
        SetFloat(root, "ScrollbarRounding", style.ScrollbarRounding);

        SetFloat(root, "GrabMinSize", style.GrabMinSize);
        SetFloat(root, "GrabRounding", style.GrabRounding);

        SetFloat(root, "LogSliderDeadzone", style.LogSliderDeadzone);

        SetFloat(root, "TabRounding", style.TabRounding);
        SetFloat(root, "TabBorderSize", style.TabBorderSize);
        SetFloat(root, "TabCloseButtonMinWidthSelected", style.TabCloseButtonMinWidthSelected);

        SetFloat(root, "TabBarBorderSize", style.TabBarBorderSize);
        SetFloat(root, "TableAngledHeadersAngle", style.TableAngledHeadersAngle);
        AddVec2(root, "TableAngledHeadersTextAlign", style.TableAngledHeadersTextAlign);

        SetInt(root, "ColorButtonPosition", static_cast<int>(style.ColorButtonPosition));
        AddVec2(root, "ButtonTextAlign", style.ButtonTextAlign);
        AddVec2(root, "SelectableTextAlign", style.SelectableTextAlign);

        SetFloat(root, "SeparatorTextBorderSize", style.SeparatorTextBorderSize);
        AddVec2(root, "SeparatorTextAlign", style.SeparatorTextAlign);
        AddVec2(root, "SeparatorTextPadding", style.SeparatorTextPadding);

        AddVec2(root, "DisplayWindowPadding", style.DisplayWindowPadding);
        AddVec2(root, "DisplaySafeAreaPadding", style.DisplaySafeAreaPadding);

        SetFloat(root, "MouseCursorScale", style.MouseCursorScale);
        SetBool(root, "AntiAliasedLines", style.AntiAliasedLines);
        SetBool(root, "AntiAliasedLinesUseTex", style.AntiAliasedLinesUseTex);
        SetBool(root, "AntiAliasedFill", style.AntiAliasedFill);
        SetFloat(root, "CurveTessellationTol", style.CurveTessellationTol);
        SetFloat(root, "CircleTessellationMaxError", style.CircleTessellationMaxError);

        ryml::NodeRef colors = root["Colors"];
        colors |= ryml::MAP;

        for (int i = 0; i < ImGuiCol_COUNT; ++i)
        {
            const char* name = ImGui::GetStyleColorName(i);
            ryml::NodeRef c = colors[name];
            c |= ryml::MAP;

            const ImVec4 v = style.Colors[i];
            SetFloat(c, "x", v.x);
            SetFloat(c, "y", v.y);
            SetFloat(c, "z", v.z);
            SetFloat(c, "w", v.w);
        }

        return tree;
    }

    static bool TreeToStyle(const ryml::Tree& tree, ImGuiStyle& style)
    {
        const ryml::ConstNodeRef root = tree.rootref();
        if (root.invalid() || !root.is_map())
        {
            return false;
        }

        (void)GetFloat(root, "Alpha", style.Alpha);
        (void)GetFloat(root, "DisabledAlpha", style.DisabledAlpha);

        (void)ReadVec2(root, "WindowPadding", style.WindowPadding);
        (void)GetFloat(root, "WindowRounding", style.WindowRounding);
        (void)GetFloat(root, "WindowBorderSize", style.WindowBorderSize);
        (void)ReadVec2(root, "WindowMinSize", style.WindowMinSize);
        (void)ReadVec2(root, "WindowTitleAlign", style.WindowTitleAlign);

        int tmpInt = 0;
        if (GetInt(root, "WindowMenuButtonPosition", tmpInt))
        {
            style.WindowMenuButtonPosition = static_cast<ImGuiDir>(tmpInt);
        }

        (void)GetFloat(root, "ChildRounding", style.ChildRounding);
        (void)GetFloat(root, "ChildBorderSize", style.ChildBorderSize);

        (void)GetFloat(root, "PopupRounding", style.PopupRounding);
        (void)GetFloat(root, "PopupBorderSize", style.PopupBorderSize);

        (void)ReadVec2(root, "FramePadding", style.FramePadding);
        (void)GetFloat(root, "FrameRounding", style.FrameRounding);
        (void)GetFloat(root, "FrameBorderSize", style.FrameBorderSize);

        (void)ReadVec2(root, "ItemSpacing", style.ItemSpacing);
        (void)ReadVec2(root, "ItemInnerSpacing", style.ItemInnerSpacing);
        (void)ReadVec2(root, "CellPadding", style.CellPadding);

        (void)ReadVec2(root, "TouchExtraPadding", style.TouchExtraPadding);
        (void)GetFloat(root, "IndentSpacing", style.IndentSpacing);
        (void)GetFloat(root, "ColumnsMinSpacing", style.ColumnsMinSpacing);

        (void)GetFloat(root, "ScrollbarSize", style.ScrollbarSize);
        (void)GetFloat(root, "ScrollbarRounding", style.ScrollbarRounding);

        (void)GetFloat(root, "GrabMinSize", style.GrabMinSize);
        (void)GetFloat(root, "GrabRounding", style.GrabRounding);

        (void)GetFloat(root, "LogSliderDeadzone", style.LogSliderDeadzone);

        (void)GetFloat(root, "TabRounding", style.TabRounding);
        (void)GetFloat(root, "TabBorderSize", style.TabBorderSize);
        (void)GetFloat(root, "TabCloseButtonMinWidthSelected", style.TabCloseButtonMinWidthSelected);

        (void)GetFloat(root, "TabBarBorderSize", style.TabBarBorderSize);
        (void)GetFloat(root, "TableAngledHeadersAngle", style.TableAngledHeadersAngle);
        (void)ReadVec2(root, "TableAngledHeadersTextAlign", style.TableAngledHeadersTextAlign);

        if (GetInt(root, "ColorButtonPosition", tmpInt))
        {
            style.ColorButtonPosition = static_cast<ImGuiDir>(tmpInt);
        }

        (void)ReadVec2(root, "ButtonTextAlign", style.ButtonTextAlign);
        (void)ReadVec2(root, "SelectableTextAlign", style.SelectableTextAlign);

        (void)GetFloat(root, "SeparatorTextBorderSize", style.SeparatorTextBorderSize);
        (void)ReadVec2(root, "SeparatorTextAlign", style.SeparatorTextAlign);
        (void)ReadVec2(root, "SeparatorTextPadding", style.SeparatorTextPadding);

        (void)ReadVec2(root, "DisplayWindowPadding", style.DisplayWindowPadding);
        (void)ReadVec2(root, "DisplaySafeAreaPadding", style.DisplaySafeAreaPadding);

        (void)GetFloat(root, "MouseCursorScale", style.MouseCursorScale);
        (void)GetBool(root, "AntiAliasedLines", style.AntiAliasedLines);
        (void)GetBool(root, "AntiAliasedLinesUseTex", style.AntiAliasedLinesUseTex);
        (void)GetBool(root, "AntiAliasedFill", style.AntiAliasedFill);
        (void)GetFloat(root, "CurveTessellationTol", style.CurveTessellationTol);
        (void)GetFloat(root, "CircleTessellationMaxError", style.CircleTessellationMaxError);

        if (root.has_child("Colors"))
        {
            ryml::ConstNodeRef colors = root["Colors"];
            for (int i = 0; i < ImGuiCol_COUNT; ++i)
            {
                const char* name = ImGui::GetStyleColorName(i);
                if (!colors.has_child(name))
                {
                    continue;
                }

                ImVec4 v = style.Colors[i];
                (void)ReadVec4(colors, name, v);
                style.Colors[i] = v;
            }
        }

        return true;
    }

    bool SaveStyle(const std::string &filepath, const ImGuiStyle& style)
    {
        try
        {
            const ryml::Tree tree = StyleToTree(style);

            std::string out;
            out.reserve(64 * 1024);
            ryml::emitrs_yaml(tree, &out);

            std::ofstream file(filepath, std::ios::out | std::ios::binary);
            if (!file.is_open())
            {
                return false;
            }

            file.write(out.data(), static_cast<std::streamsize>(out.size()));
            return file.good();
        }
        catch (...)
        {
            return false;
        }
    }

    bool LoadStyle(const std::string &filepath, ImGuiStyle& style)
    {
        try
        {
            std::ifstream file(filepath, std::ios::in | std::ios::binary);
            if (!file.is_open())
            {
                return false;
            }

            std::string text;
            file.seekg(0, std::ios::end);
            text.resize(static_cast<size_t>(file.tellg()));
            file.seekg(0, std::ios::beg);
            file.read(text.data(), static_cast<std::streamsize>(text.size()));

            ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(text));
            return TreeToStyle(tree, style);
        }
        catch (...)
        {
            return false;
        }
    }
}
