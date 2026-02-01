#pragma once

namespace Neon::RHI
{
    class Window;

    struct SwapchainDesc
    {
        Rc<Window> window;
    };
}
