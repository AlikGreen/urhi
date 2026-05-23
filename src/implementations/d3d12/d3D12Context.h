#pragma once
#include "context.h"
#include "d3D12Helper.h"

namespace urhi
{
    class D3D12Device;

    class D3D12Context final : public Context
{
public:
    explicit D3D12Context(const ContextDesc& desc);
    grl::Rc<Device> createDevice(const DeviceDesc &desc) override;
    grl::Rc<Window> createWindow(const WindowDesc &desc) override;

    grl::Rc<Swapchain> createSwapchain(const SwapchainDesc &desc) override;

    clogr::Logger& logger() override;

    IDXGIFactory6* dxgiFactory() const { return m_factory.Get(); }
    std::filesystem::path cachePath() { return m_cachePath; }
private:
    ComPtr<IDXGIFactory6> m_factory;
    grl::Rc<D3D12Device> m_device;
    std::filesystem::path m_cachePath;
};
}
