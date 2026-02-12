#pragma once
#include <grl/mem.h>

#include "device.h"
#include "descriptions/deviceDesc.h"
#include "descriptions/windowDesc.h"
#include "enums/backendAPI.h"

namespace urhi
{
class Context
{
public:
    virtual ~Context() = default;

    virtual grl::Rc<Device>    createDevice(const DeviceDesc& desc)       = 0;
    virtual grl::Rc<Window>    createWindow(const WindowDesc& desc)       = 0;
    virtual grl::Rc<Swapchain> createSwapchain(const SwapchainDesc& desc) = 0;

    static grl::Rc<Context> create(BackendAPI api);
};
}
