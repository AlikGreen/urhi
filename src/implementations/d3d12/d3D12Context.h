#pragma once
#include "context.h"

namespace urhi
{
class D3D12Context final : public Context
{
public:
    grl::Rc<Device> createDevice(const DeviceDesc &desc) override;
    grl::Rc<Window> createWindow(const WindowDesc &desc) override;

    grl::Rc<Swapchain> createSwapchain(const SwapchainDesc &desc) override;

    clogr::Logger& logger() override;
};
}
