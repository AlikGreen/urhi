#include "window.h"

#include "implementations/opengl/windowOGL.h"

namespace Neon::RHI
{
    Rc<Window> Window::createWindow(const WindowCreationOptions& creationOptions, const BackendAPI backendApi)
    {
        switch (backendApi)
        {
            case BackendAPI::OpenGL:
                return makeRc<WindowOGL>(creationOptions);
        }

        return nullptr;
    }
}
