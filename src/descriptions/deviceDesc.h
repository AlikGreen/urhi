#pragma once
#include <grl/mem.h>

namespace urhi
{
class Window;

struct DeviceDesc
{
    grl::Rc<Window> window = nullptr;
};
}
