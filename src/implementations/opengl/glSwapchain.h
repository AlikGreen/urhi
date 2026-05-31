#pragma once

#include "swapchain.h"
#include "glDevice.h"
#include "glTextureView.h"
#include "glWindow.h"
#include "descriptions/swapchainDesc.h"

namespace urhi
{
class GlSwapchain final : public Swapchain
{
public:
    explicit GlSwapchain(const SwapchainDesc& desc);
    ~GlSwapchain() override;
    void resize(uint32_t width, uint32_t height) override;

    [[nodiscard]] grl::Rc<TextureView> acquireNextImage() override;

    void present() override;
private:
    uint32_t m_imageIndex = 0;
    grl::Rc<GlDevice> m_device;
    grl::Rc<GlWindow> m_window;

    grl::Rc<GlTextureView> m_backBufferView;
};
}
