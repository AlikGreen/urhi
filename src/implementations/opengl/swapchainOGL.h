#pragma once
#include "framebufferOGL.h"
#include "swapchain.h"
#include "descriptions/swapchainDescription.h"

namespace Neon::RHI
{
class SwapchainOGL final : public Swapchain
{
public:
    explicit SwapchainOGL(const SwapchainDescription& desc);
    uint32_t acquireNextImage() override;
    void present(uint32_t imageIndex) override;

    void resize(uint32_t width, uint32_t height) override;

private:
    Window* window;
    std::vector<FramebufferOGL> framebuffers{};
    uint32_t width, height;
};
}
