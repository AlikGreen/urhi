#include "vkWindow.h"

#include "clogr.h"
#include "VkBootstrap.h"
#include "vkContext.h"
#include "vkConvert.h"
#include "GLFW/glfw3.h"

namespace urhi
{
    VkWindow::VkWindow(const WindowDesc &options, const VkContext *context)
    {
        clogr::ensure(glfwInit(), "Failed to initialize GLFW");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindowHint(GLFW_RESIZABLE, options.resizable ? GLFW_TRUE : GLFW_FALSE);

        int width = options.width;
        int height = options.height;

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        clogr::ensure(primaryMonitor != nullptr, "Failed to get primary monitor");

        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        clogr::ensure(mode != nullptr, "Couldn't get video mode");

        if(width <= 0)
            width = static_cast<int>(mode->width * 0.75);

        if(height <= 0)
            height = static_cast<int>(mode->height * 0.75);

        m_width = width;
        m_height = height;

        GLFWmonitor* monitor = options.fullscreen ? primaryMonitor : nullptr;
        m_handle = glfwCreateWindow(width, height, options.title, monitor, nullptr);
        clogr::ensure(m_handle != nullptr, "Failed to create GLFW window");

        glfwSetWindowUserPointer(m_handle, this);

        glfwSetKeyCallback(        m_handle, keyCallback);
        glfwSetMouseButtonCallback(m_handle, mouseButtonCallback);
        glfwSetScrollCallback(     m_handle, scrollCallback);
        glfwSetCursorPosCallback(  m_handle, cursorPosCallback);
        glfwSetWindowSizeCallback( m_handle, windowSizeCallback);
        glfwSetWindowCloseCallback(m_handle, windowCloseCallback);
        glfwSetCharCallback(       m_handle, charCallback);
        glfwSetDropCallback(       m_handle, dropCallback);

        VkSurfaceKHR rawSurface;
        const VkResult err = glfwCreateWindowSurface(context->getVkInstance(), m_handle, nullptr, &rawSurface);
        clogr::ensure(err == VK_SUCCESS, "Failed to create vulkan window surface");
        m_surface = rawSurface;
    }

    void VkWindow::close()
    {
        clogr::abort("not implemented");
    }

    void VkWindow::pollEvents(const std::function<void(Event&)> callback)
    {
        m_events.clear();
        glfwPollEvents();
        for(auto& event : m_events)
        {
            callback(event);
        }
    }

    uint32_t VkWindow::getWidth()
    {
        return m_width;
    }

    uint32_t VkWindow::getHeight()
    {
        return m_height;
    }

    glm::ivec2 VkWindow::getSize()
    {
        clogr::abort("not implemented");
    }

    void VkWindow::setWidth(uint32_t width)
    {
        clogr::abort("not implemented");
    }

    void VkWindow::setHeight(uint32_t height)
    {
        clogr::abort("not implemented");
    }

    void VkWindow::setSize(glm::ivec2 size)
    {
        clogr::abort("not implemented");
    }

    std::string VkWindow::getTitle()
    {
        clogr::abort("not implemented");
    }

    void VkWindow::setTitle(std::string title)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkWindow::setCursorLocked(bool locked)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkWindow::setCursorVisible(bool visible)
    {
        clogr::ensure(false, "not implemented");
    }

    vk::SurfaceKHR VkWindow::getSurface() const
    {
        return m_surface;
    }

    void VkWindow::keyCallback(GLFWwindow* windowGLFW, int key, int scancode, int action, int mods)
    {
        const auto window = static_cast<VkWindow*>(glfwGetWindowUserPointer(windowGLFW));
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            auto event = Event();
            event.type = Event::Type::KeyDown;
            event.data = Event::KeyEvent{ VkConvert::keyCode(key), action == GLFW_REPEAT };
            window->m_events.emplace_back(event);
        }
        else if (action == GLFW_RELEASE)
        {
            auto event = Event();
            event.type = Event::Type::KeyUp;
            event.data = Event::KeyEvent{ VkConvert::keyCode(key), false };
            window->m_events.emplace_back(event);
        }
    }

    void VkWindow::mouseButtonCallback(GLFWwindow* windowGLFW, int button, int action, int mods)
    {
        const auto window = static_cast<VkWindow*>(glfwGetWindowUserPointer(windowGLFW));
        if (action == GLFW_PRESS)
        {
            auto event = Event();
            event.type = Event::Type::MouseButtonDown;
            event.data = Event::MouseButtonEvent{ VkConvert::mouseButton(button), 0, 0, 0 };
            window->m_events.emplace_back(event);
        }
        else if (action == GLFW_RELEASE)
        {
            auto event = Event();
            event.type = Event::Type::MouseButtonUp;
            event.data = Event::MouseButtonEvent{ VkConvert::mouseButton(button), 0, 0, 0 };
            window->m_events.emplace_back(event);
        }
    }

    void VkWindow::cursorPosCallback(GLFWwindow* windowGLFW, const double xPos, const double yPos)
    {
        const auto window = static_cast<VkWindow*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::MouseMotion;
        event.data = Event::MouseMotionEvent{ static_cast<float>(xPos), static_cast<float>(yPos) };
        window->m_events.emplace_back(event);
    }

    void VkWindow::windowSizeCallback(GLFWwindow* windowGLFW, const int width, const int height)
    {
        const auto window = static_cast<VkWindow*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::WindowResize;
        event.data = Event::WindowResizeEvent{ width, height };
        window->m_width = width;
        window->m_height = height;
        window->m_events.emplace_back(event);
    }

    void VkWindow::windowCloseCallback(GLFWwindow* windowGLFW)
    {
        const auto window = static_cast<VkWindow*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::Quit;
        event.data = std::monostate{};
        window->m_events.emplace_back(event);
    }

    void VkWindow::charCallback(GLFWwindow* windowGLFW, const unsigned int codepoint)
    {
        const auto window = static_cast<VkWindow*>(glfwGetWindowUserPointer(windowGLFW));

        auto event = Event();
        event.type = Event::Type::TextInput;
        event.data = Event::TextInputEvent{ static_cast<uint32_t>(codepoint) };
        window->m_events.emplace_back(event);
    }

    void VkWindow::scrollCallback(GLFWwindow* windowGLFW, const double xOffset, const double yOffset)
    {
        const auto window = static_cast<VkWindow*>(glfwGetWindowUserPointer(windowGLFW));

        auto event = Event();
        event.type = Event::Type::MouseWheel;
        event.data = Event::MouseWheelEvent{ static_cast<int>(xOffset), static_cast<int>(yOffset) };
        window->m_events.emplace_back(event);
    }

    void VkWindow::dropCallback(GLFWwindow* windowGLFW, int pathCount, const char** paths)
    {
        const auto window = static_cast<VkWindow*>(glfwGetWindowUserPointer(windowGLFW));
        for (int i = 0; i < pathCount; ++i)
        {
            const char* path = paths[i];

            auto event = Event();
            event.type = Event::Type::DropFile;
            event.data = Event::DropFileEvent{ std::string(path) };
            window->m_events.emplace_back(event);
        }
    }
}
