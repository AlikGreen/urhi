#include "d3D12Swapchain.h"

namespace urhi
{
    D3D12Swapchain::D3D12Swapchain(const SwapchainDesc &desc, D3D12Context* context, ID3D12CommandQueue* graphicsQueue)
        : m_context(*context), m_graphicsQueue(graphicsQueue)
    {
        m_device = std::dynamic_pointer_cast<D3D12Device>(desc.device);
        m_window = std::dynamic_pointer_cast<D3D12Window>(desc.window);

        resize(desc.width, desc.height);
    }

    void D3D12Swapchain::resize(const uint32_t width, const uint32_t height)
    {
        m_swapchain = nullptr;
        m_width = width;
        m_height = height;

        DXGI_SWAP_CHAIN_DESC1 desc{};
        desc.Width       = width;
        desc.Height      = height;
        desc.Format      = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferCount = 3;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.SampleDesc  = { 1, 0 };

        ComPtr<IDXGISwapChain1> tmp;
        auto hr = m_context.dxgiFactory()->CreateSwapChainForHwnd(m_graphicsQueue, m_window->hwnd(), &desc, nullptr, nullptr, &tmp);
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to create swapchain for window");

        hr = tmp->QueryInterface(IID_PPV_ARGS(&m_swapchain));
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to create swapchain");
    }

    grl::Rc<TextureView> D3D12Swapchain::acquireNextImage()
    {
        return nullptr;
    }

    void D3D12Swapchain::present()
    {

    }
}
