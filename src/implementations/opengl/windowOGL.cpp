#include "windowOGL.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "convertOGL.h"
#include <clogr.h>
#include "deviceOGL.h"
#include "grl/grl.h"

namespace urhi
{
    void WindowOGL::keyCallback(GLFWwindow* windowGLFW, int key, int scancode, int action, int mods)
    {
        const auto window = static_cast<WindowOGL*>(glfwGetWindowUserPointer(windowGLFW));
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            auto event = Event();
            event.type = Event::Type::KeyDown;
            event.data = Event::KeyEvent{ ConvertOGL::keyCodeFromGLFW(key), action == GLFW_REPEAT };
            window->events.emplace_back(event);
        }
        else if (action == GLFW_RELEASE)
        {
            auto event = Event();
            event.type = Event::Type::KeyUp;
            event.data = Event::KeyEvent{ ConvertOGL::keyCodeFromGLFW(key), false };
            window->events.emplace_back(event);
        }
    }

    void WindowOGL::mouseButtonCallback(GLFWwindow* windowGLFW, int button, int action, int mods)
    {
        const auto window = static_cast<WindowOGL*>(glfwGetWindowUserPointer(windowGLFW));
        if (action == GLFW_PRESS)
        {
            auto event = Event();
            event.type = Event::Type::MouseButtonDown;
            event.data = Event::MouseButtonEvent{ ConvertOGL::mouseButtonFromGLFW(button), 0, 0, 0 };
            window->events.emplace_back(event);
        }
        else if (action == GLFW_RELEASE)
        {
            auto event = Event();
            event.type = Event::Type::MouseButtonUp;
            event.data = Event::MouseButtonEvent{ ConvertOGL::mouseButtonFromGLFW(button), 0, 0, 0 };
            window->events.emplace_back(event);
        }
    }

    void WindowOGL::cursorPosCallback(GLFWwindow* windowGLFW, const double xPos, const double yPos)
    {
        const auto window = static_cast<WindowOGL*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::MouseMotion;
        event.data = Event::MouseMotionEvent{ static_cast<float>(xPos), static_cast<float>(yPos) };
        window->events.emplace_back(event);
    }

    void WindowOGL::windowSizeCallback(GLFWwindow* windowGLFW, const int width, const int height)
    {
        const auto window = static_cast<WindowOGL*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::WindowResize;
        event.data = Event::WindowResizeEvent{ width, height };
        window->events.emplace_back(event);
    }

    void WindowOGL::windowCloseCallback(GLFWwindow* windowGLFW)
    {
        const auto window = static_cast<WindowOGL*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::Quit;
        event.data = std::monostate{};
        window->events.emplace_back(event);
    }

    void WindowOGL::charCallback(GLFWwindow* windowGLFW, const unsigned int codepoint)
    {
        const auto window = static_cast<WindowOGL*>(glfwGetWindowUserPointer(windowGLFW));

        auto event = Event();
        event.type = Event::Type::TextInput;
        event.data = Event::TextInputEvent{ static_cast<uint32_t>(codepoint) };
        window->events.emplace_back(event);
    }

    void WindowOGL::scrollCallback(GLFWwindow* windowGLFW, const double xOffset, const double yOffset)
    {
        const auto window = static_cast<WindowOGL*>(glfwGetWindowUserPointer(windowGLFW));

        auto event = Event();
        event.type = Event::Type::MouseWheel;
        event.data = Event::MouseWheelEvent{ static_cast<int>(xOffset), static_cast<int>(yOffset) };
        window->events.emplace_back(event);
    }

    void WindowOGL::dropCallback(GLFWwindow* windowGLFW, int pathCount, const char** paths)
    {
        const auto window = static_cast<WindowOGL*>(glfwGetWindowUserPointer(windowGLFW));
        for (int i = 0; i < pathCount; ++i)
        {
            const char* path = paths[i];

            auto event = Event();
            event.type = Event::Type::DropFile;
            event.data = Event::DropFileEvent{ std::string(path) };
            window->events.emplace_back(event);
        }
    }

    WindowOGL::WindowOGL(const WindowCreationOptions &creationOptions) : creationOptions(creationOptions) { }

    grl::Rc<Device> WindowOGL::createDevice()
    {
        return grl::makeRc<DeviceOGL>();
    }


    void WindowOGL::run()
    {
        clogr::ensure(glfwInit(), "Failed to initialize GLFW");
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_RESIZABLE, creationOptions.resizable ? GLFW_TRUE : GLFW_FALSE);

        int width = creationOptions.width;
        int height = creationOptions.height;

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        clogr::ensure(primaryMonitor != nullptr, "Failed to get primary monitor");

        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        clogr::ensure(mode != nullptr, "Couldn't get video mode");

        if(width <= 0)
            width = static_cast<int>(mode->width * 0.75);

        if(height <= 0)
            height = static_cast<int>(mode->height * 0.75);

        GLFWmonitor* monitor = creationOptions.fullscreen ? primaryMonitor : nullptr;
        handle = glfwCreateWindow(width, height, creationOptions.title, monitor, nullptr);
        clogr::ensure(handle != nullptr, "Failed to create GLFW window");

        glfwSetWindowUserPointer(handle, this);

        glfwSetKeyCallback(handle, keyCallback);
        glfwSetMouseButtonCallback(handle, mouseButtonCallback);
        glfwSetScrollCallback(handle, scrollCallback);
        glfwSetCursorPosCallback(handle, cursorPosCallback);
        glfwSetWindowSizeCallback(handle, windowSizeCallback);
        glfwSetWindowCloseCallback(handle, windowCloseCallback);
        glfwSetCharCallback(handle, charCallback);
        glfwSetDropCallback(handle, dropCallback);

        glfwMakeContextCurrent(handle);

        clogr::ensure(gladLoadGL(glfwGetProcAddress), "Failed to initialize GLAD");

        glViewport(0, 0, width, height);

        if(creationOptions.vsync)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);
    }

    void WindowOGL::close()
    {
        if(handle)
        {
            glfwDestroyWindow(handle);
            handle = nullptr;
        }
        glfwTerminate();
    }

    std::vector<Event> WindowOGL::pollEvents()
    {
        events.clear();
        glfwPollEvents();
        return events;
    }

    uint32_t WindowOGL::getWidth()
    {
        return getSize().x;
    }

    uint32_t WindowOGL::getHeight()
    {
        return getSize().y;
    }

    glm::ivec2 WindowOGL::getSize()
    {
        glm::ivec2 size;
        glfwGetWindowSize(handle, &size.x, &size.y);
        return size;
    }

    void WindowOGL::setWidth(const uint32_t width)
    {
        glfwSetWindowSize(handle, static_cast<int>(width), static_cast<int>(getHeight()));
    }

    void WindowOGL::setHeight(const uint32_t height)
    {
        glfwSetWindowSize(handle, static_cast<int>(getWidth()), static_cast<int>(height));
    }

    void WindowOGL::setSize(const glm::ivec2 size)
    {
        glfwSetWindowSize(handle, size.x, size.y);
    }

    std::string WindowOGL::getTitle()
    {
        return glfwGetWindowTitle(handle);
    }

    void WindowOGL::setTitle(const std::string title)
    {
        glfwSetWindowTitle(handle, title.c_str());
    }

    void WindowOGL::setCursorLocked(const bool locked)
    {
        cursorLocked = locked;
        updateCursorState();
    }

    void WindowOGL::setCursorVisible(const bool visible)
    {
        cursorVisible = visible;
        updateCursorState();
    }

    void WindowOGL::swapBuffers()
    {
        glfwSwapBuffers(handle);
    }

    void WindowOGL::updateCursorState() const
    {
        if (cursorLocked && !cursorVisible)
        {
            glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else if (cursorLocked && cursorVisible)
        {
            glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        } else if (!cursorVisible)
        {
            glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        } else
        {
            glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}
