#include "window.h"

#include "implementations/opengl/windowOGL.h"

namespace Neon::RHI
{
    Window* Window::createWindow(const WindowCreationOptions& creationOptions, const BackendAPI backendApi)
    {
        switch (backendApi)
        {
            case BackendAPI::OpenGL:
                return new WindowOGL(creationOptions);
        }

        return nullptr;
    }
}
