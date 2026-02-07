#include "window.h"

#include "implementations/opengl/windowOGL.h"

namespace urhi
{
    grl::Rc<Window> Window::createWindow(const WindowCreationOptions& creationOptions, const BackendAPI backendApi)
    {
        switch (backendApi)
        {
            case BackendAPI::OpenGL:
                return grl::makeRc<WindowOGL>(creationOptions);
        }

        return nullptr;
    }
}
