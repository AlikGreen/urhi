#pragma once
#include "window.h"

#include "descriptions/windowDesc.h"

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

namespace urhi
{
class D3D12Window final : public Window
{
public:
    explicit D3D12Window(const WindowDesc& desc);

    void show() override;
    void hide() override;

    void close() override;

    std::vector<Event> pollEvents() override;

    int32_t width() override;
    int32_t height() override;

    void width(int32_t width) override;
    void height(int32_t height) override;

    std::string title() override;
    void title(const std::string &title) override;

    void setCursorLocked(bool locked) override;
    void setCursorVisible(bool visible) override;

    HWND hwnd() const { return m_hwnd; };
private:
    void updateCursorState() const;

    GLFWwindow* m_window;
    HWND m_hwnd;

    std::vector<Event> m_events{};

    int32_t m_width{};
    int32_t m_height{};

    bool m_locked = false;
    bool m_visible = true;


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
