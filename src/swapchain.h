#pragma once
#include <cstdint>

namespace Neon::RHI
{
class Swapchain
{
public:
    virtual ~Swapchain() = default;

    virtual void resize(uint32_t width, uint32_t height) = 0;
    virtual uint32_t acquireNextImage() = 0;

    virtual void present(uint32_t imageIndex) = 0;
};
}
