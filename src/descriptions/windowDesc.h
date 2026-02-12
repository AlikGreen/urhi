#pragma once

namespace urhi
{
struct WindowDesc
{
    const char* title{};
    int width = -1;
    int height = -1;
    bool vsync = false;
    bool fullscreen = false;
    bool resizable = true;
};
}
