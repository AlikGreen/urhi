#include "glWindow.h"

#include "clogr.h"
#include "validation.h"
#include "glContext.h"
#include "glConvert.h"
#include "GLFW/glfw3.h"

namespace urhi
{
    GlWindow::GlWindow(const WindowDesc &options, const GlContext *context)
    {
        const int suc = glfwInit();
        URHI_VALIDATE(suc != 0, "Failed to initialize GLFW");

        glfwWindowHint(GLFW_RESIZABLE, options.resizable ? GLFW_TRUE : GLFW_FALSE);

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        m_width = options.width;
        m_height = options.height;

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        URHI_VALIDATE(primaryMonitor != nullptr, "Failed to get primary monitor - returned null");

        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        URHI_VALIDATE(mode != nullptr, "Failed get video mode - glfwGetVideoMode returned null");

        if(m_width <= 0)
            m_width = static_cast<int>(mode->width * 0.75);

        if(m_height <= 0)
            m_height = static_cast<int>(mode->height * 0.75);


        GLFWmonitor* monitor = options.fullscreen ? primaryMonitor : nullptr;
        m_handle = glfwCreateWindow(m_width, m_height, options.title, monitor, nullptr);
        URHI_VALIDATE(m_handle != nullptr, "Failed to create GLFW window - glfwCreateWindow returned null");

        glfwSetWindowUserPointer(m_handle, this);

        glfwSetKeyCallback(        m_handle, keyCallback);
        glfwSetMouseButtonCallback(m_handle, mouseButtonCallback);
        glfwSetScrollCallback(     m_handle, scrollCallback);
        glfwSetCursorPosCallback(  m_handle, cursorPosCallback);
        glfwSetWindowSizeCallback( m_handle, windowSizeCallback);
        glfwSetWindowCloseCallback(m_handle, windowCloseCallback);
        glfwSetCharCallback(       m_handle, charCallback);
        glfwSetDropCallback(       m_handle, dropCallback);

        glfwMakeContextCurrent(m_handle);

        const int version = gladLoadGL(glfwGetProcAddress);
        URHI_VALIDATE(version != 0, "Failed to initialize OpenGL context");
    }

    void GlWindow::show()
    {
        glfwShowWindow(m_handle);
    }

    void GlWindow::hide()
    {
        glfwHideWindow(m_handle);
    }

    void GlWindow::close()
    {
        if(m_handle)
        {
            glfwDestroyWindow(m_handle);
            m_handle = nullptr;
        }
    }

    std::vector<Event> GlWindow::pollEvents()
    {
        m_events.clear();
        glfwPollEvents();
        return m_events;
    }

    int32_t GlWindow::width()
    {
        return m_width;
    }

    int32_t GlWindow::height()
    {
        return m_height;
    }

    void GlWindow::width(const int32_t width)
    {
        glfwSetWindowSize(m_handle, width, m_height);
        m_width = width;
    }

    void GlWindow::height(const int32_t height)
    {
        glfwSetWindowSize(m_handle, m_width, height);
        m_height = height;
    }

    std::string GlWindow::title()
    {
        return glfwGetWindowTitle(m_handle);
    }

    void GlWindow::title(const std::string& title)
    {
        glfwSetWindowTitle(m_handle, title.c_str());
    }

    void GlWindow::setCursorLocked(const bool locked)
    {
        m_locked = locked;
        updateCursorState();
    }

    void GlWindow::setCursorVisible(bool visible)
    {
        m_visible = visible;
        updateCursorState();
    }

    void GlWindow::updateCursorState() const
    {
        if(m_locked && m_visible)
            glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        if(m_locked && !m_visible)
            glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if(!m_locked && m_visible)
            glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if(!m_locked && !m_visible)
            glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void GlWindow::keyCallback(GLFWwindow* windowGLFW, int key, int scancode, int action, int mods)
    {
        const auto window = static_cast<GlWindow*>(glfwGetWindowUserPointer(windowGLFW));
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            auto event = Event();
            event.type = Event::Type::KeyDown;
            event.data = Event::KeyEvent{ GlConvert::keyCode(key), action == GLFW_REPEAT };
            window->m_events.emplace_back(event);
        }
        else if (action == GLFW_RELEASE)
        {
            auto event = Event();
            event.type = Event::Type::KeyUp;
            event.data = Event::KeyEvent{ GlConvert::keyCode(key), false };
            window->m_events.emplace_back(event);
        }
    }

    void GlWindow::mouseButtonCallback(GLFWwindow* windowGLFW, int button, int action, int mods)
    {
        const auto window = static_cast<GlWindow*>(glfwGetWindowUserPointer(windowGLFW));
        if (action == GLFW_PRESS)
        {
            auto event = Event();
            event.type = Event::Type::MouseButtonDown;
            event.data = Event::MouseButtonEvent{ GlConvert::mouseButton(button), 0, 0, 0 };
            window->m_events.emplace_back(event);
        }
        else if (action == GLFW_RELEASE)
        {
            auto event = Event();
            event.type = Event::Type::MouseButtonUp;
            event.data = Event::MouseButtonEvent{ GlConvert::mouseButton(button), 0, 0, 0 };
            window->m_events.emplace_back(event);
        }
    }

    void GlWindow::cursorPosCallback(GLFWwindow* windowGLFW, const double xPos, const double yPos)
    {
        const auto window = static_cast<GlWindow*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::MouseMotion;
        event.data = Event::MouseMotionEvent{ static_cast<float>(xPos), static_cast<float>(yPos) };
        window->m_events.emplace_back(event);
    }

    void GlWindow::windowSizeCallback(GLFWwindow* windowGLFW, const int width, const int height)
    {
        const auto window = static_cast<GlWindow*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::WindowResize;
        event.data = Event::WindowResizeEvent{ width, height };
        window->m_width = width;
        window->m_height = height;
        window->m_events.emplace_back(event);
    }

    void GlWindow::windowCloseCallback(GLFWwindow* windowGLFW)
    {
        const auto window = static_cast<GlWindow*>(glfwGetWindowUserPointer(windowGLFW));
        auto event = Event();
        event.type = Event::Type::Quit;
        event.data = std::monostate{};
        window->m_events.emplace_back(event);
    }

    void GlWindow::charCallback(GLFWwindow* windowGLFW, const unsigned int codepoint)
    {
        const auto window = static_cast<GlWindow*>(glfwGetWindowUserPointer(windowGLFW));

        auto event = Event();
        event.type = Event::Type::TextInput;
        event.data = Event::TextInputEvent{ static_cast<uint32_t>(codepoint) };
        window->m_events.emplace_back(event);
    }

    void GlWindow::scrollCallback(GLFWwindow* windowGLFW, const double xOffset, const double yOffset)
    {
        const auto window = static_cast<GlWindow*>(glfwGetWindowUserPointer(windowGLFW));

        auto event = Event();
        event.type = Event::Type::MouseWheel;
        event.data = Event::MouseWheelEvent{ static_cast<int>(xOffset), static_cast<int>(yOffset) };
        window->m_events.emplace_back(event);
    }

    void GlWindow::dropCallback(GLFWwindow* windowGLFW, int pathCount, const char** paths)
    {
        const auto window = static_cast<GlWindow*>(glfwGetWindowUserPointer(windowGLFW));
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
