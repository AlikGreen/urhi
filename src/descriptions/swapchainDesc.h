#pragma once
#include "enums/colorSpace.h"
#include "enums/presentMode.h"

namespace urhi
{
    class Window;
    class Device;

    struct SwapchainDesc
    {
        grl::Rc<Window> window;
        grl::Rc<Device> device;

        PresentMode presentMode = PresentMode::VSync;
        PixelFormat format      = PixelFormat::RGBA8UNorm;
        ColorSpace colorSpace   = ColorSpace::Srgb;

        uint32_t preferredImageCount = 3;

        uint32_t width  = 0;
        uint32_t height = 0;
    };
}
