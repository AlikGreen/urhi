#pragma once

namespace urhi
{
    class Window;

    struct SwapchainDesc
    {
        grl::Rc<Window> window;
    };
}
