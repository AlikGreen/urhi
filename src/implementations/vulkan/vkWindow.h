#pragma once

#include <vulkan/vulkan.hpp>

#include "window.h"
#include "descriptions/windowDesc.h"

struct GLFWwindow;
namespace urhi
{
    class VkContext;

class VkWindow final : public Window
{
public:
    explicit VkWindow(const WindowDesc& options, const VkContext* context);

    void show() override;
    void hide() override;

    void close() override;

    std::vector<Event> pollEvents() override;

    int32_t width() override;
    int32_t height() override;

    void width(int32_t width) override;
    void height(int32_t height) override;

    std::string title() override;
    void title(const std::string& title) override;

    void setCursorLocked(bool locked) override;
    void setCursorVisible(bool visible) override;

    [[nodiscard]] vk::SurfaceKHR getSurface() const;
private:
    void updateCursorState() const;

    GLFWwindow* m_handle = nullptr;
    std::vector<Event> m_events{};

    int32_t m_width{};
    int32_t m_height{};

    bool m_locked = false;
    bool m_visible = true;

    vk::SurfaceKHR m_surface;

    static void keyCallback(GLFWwindow* windowGLFW, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* windowGLFW, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* windowGLFW, double xPos, double yPos);
    static void windowSizeCallback(GLFWwindow* windowGLFW, int width, int height);
    static void windowCloseCallback(GLFWwindow* windowGLFW);
    static void charCallback(GLFWwindow* windowGLFW, unsigned int codepoint);
    static void scrollCallback(GLFWwindow *windowGLFW, double xOffset, double yOffset);
    static void dropCallback(GLFWwindow *windowGLFW, int pathCount, const char **paths);
};
}
