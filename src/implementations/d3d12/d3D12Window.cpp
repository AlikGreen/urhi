#include "d3D12Window.h"

#include "d3D12Convert.h"
#include "validation.h"

namespace urhi
{
    D3D12Window::D3D12Window(const WindowDesc &desc)
    {
        const int suc = glfwInit();
        URHI_VALIDATE(suc != 0, "Failed to initialize GLFW");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, desc.resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        m_width = desc.width;
        m_height = desc.height;

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        URHI_VALIDATE(primaryMonitor != nullptr, "Failed to get primary monitor - returned null");

        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        URHI_VALIDATE(mode != nullptr, "Failed get video mode - glfwGetVideoMode returned null");

        if(m_width <= 0)
            m_width = static_cast<int>(mode->width * 0.75);

        if(m_height <= 0)
            m_height = static_cast<int>(mode->height * 0.75);


        GLFWmonitor* monitor = desc.fullscreen ? primaryMonitor : nullptr;
        m_window = glfwCreateWindow(m_width, m_height, desc.title, monitor, nullptr);
        URHI_VALIDATE(m_window != nullptr, "Failed to create GLFW window - glfwCreateWindow returned null");

        glfwSetWindowUserPointer(m_window, this);

        glfwSetKeyCallback(        m_window, keyCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetScrollCallback(     m_window, scrollCallback);
        glfwSetCursorPosCallback(  m_window, cursorPosCallback);
        glfwSetWindowSizeCallback( m_window, windowSizeCallback);
        glfwSetWindowCloseCallback(m_window, windowCloseCallback);
        glfwSetCharCallback(       m_window, charCallback);
        glfwSetDropCallback(       m_window, dropCallback);

        m_hwnd = glfwGetWin32Window(m_window);
    }

    void D3D12Window::show()
    {
        glfwShowWindow(m_window);
    }

    void D3D12Window::hide()
    {
        glfwHideWindow(m_window);
    }

    void D3D12Window::close()
    {
        if(m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
    }

    std::vector<Event> D3D12Window::pollEvents()
    {
        m_events.clear();
        glfwPollEvents();
        return m_events;
    }

    int32_t D3D12Window::width()
    {
        return m_width;
    }

    int32_t D3D12Window::height()
    {
        return m_height;
    }

    void D3D12Window::width(const int32_t width)
    {
        glfwSetWindowSize(m_window, width, m_height);
        m_width = width;
    }

    void D3D12Window::height(const int32_t height)
    {
        glfwSetWindowSize(m_window, m_width, height);
        m_height = height;
    }

    std::string D3D12Window::title()
    {
        return glfwGetWindowTitle(m_window);
    }

    void D3D12Window::title(const std::string &title)
    {
        glfwSetWindowTitle(m_window, title.c_str());
    }

    void D3D12Window::setCursorLocked(const bool locked)
    {
        m_locked = locked;
        updateCursorState();
    }

    void D3D12Window::setCursorVisible(const bool visible)
    {
        m_visible = visible;
        updateCursorState();
    }

    void D3D12Window::updateCursorState() const
    {
        if(m_locked && m_visible)
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        if(m_locked && !m_visible)
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if(!m_locked && m_visible)
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if(!m_locked && !m_visible)
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void D3D12Window::keyCallback(GLFWwindow* windowGLFW, int key, int scancode, int action, int mods)
    {
        const auto window = static_cast<D3D12Window*>(glfwGetWindowUserPointer(windowGLFW));
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            auto event = Event();
            event.type = Event::Type::KeyDown;
            event.data = Event::KeyEvent{ D3D12Convert::keyCode(key), action == GLFW_REPEAT };
            window->m_events.emplace_back(event);
        }
        else if (action == GLFW_RELEASE)
        {
            auto event = Event();
            event.type = Event::Type::KeyUp;
            event.data = Event::KeyEvent{ D3D12Convert::keyCode(key), false };
            window->m_events.emplace_back(event);
        }
    }

    void D3D12Window::mouseButtonCallback(GLFWwindow* windowGLFW, int button, int action, int mods)
    {
        const auto window = static_cast<D3D12Window*>(glfwGetWindowUserPointer(windowGLFW));
        if (action == GLFW_PRESS)
        {
            auto event = Event();
            event.type = Event::Type::MouseButtonDown;
            event.data = Event::MouseButtonEvent{ D3D12Convert::mouseButton(button), 0, 0, 0 };
            window->m_events.emplace_back(event);
        }
        else if (action == GLFW_RELEASE)
        {
            auto event = Event();
            event.type = Event::Type::MouseButtonUp;
            event.data = Event::MouseButtonEvent{ D3D12Convert::mouseButton(button), 0, 0, 0 };
            window->m_events.emplace_back(event);
        }
    }

    void D3D12Window::cursorPosCallback(GLFWwindow* windowGLFW, const double xPos, const double yPos)
    {
        const auto window = static_cast<D3D12Window*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::MouseMotion;
        event.data = Event::MouseMotionEvent{ static_cast<float>(xPos), static_cast<float>(yPos) };
        window->m_events.emplace_back(event);
    }

    void D3D12Window::windowSizeCallback(GLFWwindow* windowGLFW, const int width, const int height)
    {
        const auto window = static_cast<D3D12Window*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::WindowResize;
        event.data = Event::WindowResizeEvent{ width, height };
        window->m_width = width;
        window->m_height = height;
        window->m_events.emplace_back(event);
    }

    void D3D12Window::windowCloseCallback(GLFWwindow* windowGLFW)
    {
        const auto window = static_cast<D3D12Window*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::Quit;
        event.data = std::monostate{};
        window->m_events.emplace_back(event);
    }

    void D3D12Window::charCallback(GLFWwindow* windowGLFW, const unsigned int codepoint)
    {
        const auto window = static_cast<D3D12Window*>(glfwGetWindowUserPointer(windowGLFW));

        auto event = Event();
        event.type = Event::Type::TextInput;
        event.data = Event::TextInputEvent{ static_cast<uint32_t>(codepoint) };
        window->m_events.emplace_back(event);
    }

    void D3D12Window::scrollCallback(GLFWwindow* windowGLFW, const double xOffset, const double yOffset)
    {
        const auto window = static_cast<D3D12Window*>(glfwGetWindowUserPointer(windowGLFW));

        auto event = Event();
        event.type = Event::Type::MouseWheel;
        event.data = Event::MouseWheelEvent{ static_cast<int>(xOffset), static_cast<int>(yOffset) };
        window->m_events.emplace_back(event);
    }

    void D3D12Window::dropCallback(GLFWwindow* windowGLFW, const int pathCount, const char** paths)
    {
        const auto window = static_cast<D3D12Window*>(glfwGetWindowUserPointer(windowGLFW));
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
