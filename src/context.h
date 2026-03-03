#pragma once
#include <grl/mem.h>

#include "device.h"
#include "logger.h"
#include "swapchain.h"
#include "descriptions/deviceDesc.h"
#include "descriptions/windowDesc.h"
#include "enums/backendAPI.h"

namespace urhi
{
class Context
{
public:
    Context() = default;
    virtual ~Context() = default;

    Context(const Context&) = delete;
    Context& operator= (const Context&) = delete;

    virtual grl::Rc<Device>    createDevice(const DeviceDesc& desc)       = 0;
    virtual grl::Rc<Window>    createWindow(const WindowDesc& desc)       = 0;
    virtual grl::Rc<Swapchain> createSwapchain(const SwapchainDesc& desc) = 0;

    virtual clogr::Logger& logger() = 0;

    static grl::Rc<Context> create(BackendAPI api);
};
}
