#pragma once

namespace Neon::RHI
{
    class Window;

    struct SwapchainDescription
    {
        Rc<Window> window;
    };
}
