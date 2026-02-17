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

    void close() override;

    void pollEvents(std::function<void(Event&)> callback) override;

    uint32_t getWidth() override;
    uint32_t getHeight() override;
    glm::ivec2 getSize() override;

    void setWidth(uint32_t width) override;
    void setHeight(uint32_t height) override;
    void setSize(glm::ivec2 size) override;

    std::string getTitle() override;
    void setTitle(std::string title) override;

    void setCursorLocked(bool locked) override;
    void setCursorVisible(bool visible) override;

    [[nodiscard]] vk::SurfaceKHR getSurface() const;
private:
    GLFWwindow* m_handle = nullptr;
    std::vector<Event> m_events{};

    uint32_t m_width{};
    uint32_t m_height{};

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
