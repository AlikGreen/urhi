#pragma once
#include <cstdint>
#include "texture.h"

namespace urhi
{
class Swapchain
{
public:
    virtual ~Swapchain() = default;

    virtual void resize(uint32_t width, uint32_t height) = 0;
    [[nodiscard]] virtual uint32_t acquireNextImage() = 0;
    [[nodiscard]] virtual const std::vector<grl::Rc<Texture>>& getTextures() const = 0;

    virtual void present(uint32_t imageIndex) = 0;
};
}
