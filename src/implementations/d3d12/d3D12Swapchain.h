#pragma once
#include "d3D12Context.h"
#include "d3D12Device.h"
#include "d3D12Window.h"
#include "swapchain.h"
#include "descriptions/swapchainDesc.h"

namespace urhi
{
class D3D12Swapchain final : public Swapchain
{
public:
    explicit D3D12Swapchain(const SwapchainDesc& desc, D3D12Context* context, ID3D12CommandQueue* graphicsQueue);
    void resize(uint32_t width, uint32_t height) override;

    [[nodiscard]] grl::Rc<TextureView> acquireNextImage() override;
    void present() override;
private:
    ComPtr<IDXGISwapChain> m_swapchain;

    D3D12Context& m_context;
    grl::Rc<D3D12Device> m_device;
    grl::Rc<D3D12Window> m_window;
    ID3D12CommandQueue* m_graphicsQueue;

    uint32_t m_width{}, m_height{};

    std::vector<grl::Rc<Texture>> m_textures{};
    std::vector<grl::Rc<TextureView>> m_textureViews{};

    uint32_t m_frameIndex = 0;
    uint32_t m_maxFramesInFlight{};
};
}
