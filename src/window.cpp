#include "window.h"

#include "implementations/opengl/windowOGL.h"

namespace NRHI
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
