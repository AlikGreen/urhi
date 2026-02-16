#pragma once
#include <cstdint>
#include "textureView.h"

namespace urhi
{
class Swapchain
{
public:
    Swapchain() = default;
    virtual ~Swapchain() = default;

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator= (const Swapchain&) = delete;

    virtual void resize(uint32_t width, uint32_t height) = 0;
    [[nodiscard]] virtual uint32_t acquireNextImage() = 0;
    [[nodiscard]] virtual const std::vector<grl::Rc<TextureView>>& getTextureViews() const = 0;

    virtual void present(uint32_t imageIndex) = 0;
};
}
