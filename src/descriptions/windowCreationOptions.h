#pragma once

namespace NRHI
{
struct WindowCreationOptions
{
    const char* title{};
    int width = -1;
    int height = -1;
    bool vsync = false;
    bool fullscreen = false;
    bool resizable = true;
};
}
