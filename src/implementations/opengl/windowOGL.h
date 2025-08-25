#pragma once
#include "event.h"
#include "window.h"

#include "GLFW/glfw3.h"

namespace NRHI
{
class WindowOGL final : public Window
{
public:
    explicit WindowOGL(const WindowCreationOptions& creationOptions);
    Device* createDevice() override;

    void run() override;
    void close() override;
    std::vector<Event> pollEvents() override;

    uint32_t getWidth() override;
    uint32_t getHeight() override;
    glm::ivec2 getSize() override;

    void setWidth(uint32_t width) override;
    void setHeight(uint32_t height) override;
    void setSize(glm::ivec2 size) override;

    std::string getTitle() override;
    void setTitle(std::string title) override;

    void swapBuffers() const;

    void setCursorLocked(bool locked) override;
    void setCursorVisible(bool visible) override;
private:
    WindowCreationOptions creationOptions;
    GLFWwindow* handle{};
    bool cursorVisible = true;
    bool cursorLocked = false;

    std::vector<Event> events{};

    void updateCursorState() const;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void windowCloseCallback(GLFWwindow* window);
};
}
