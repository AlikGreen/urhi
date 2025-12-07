#pragma once
#include "glm/vec2.hpp"

namespace Neon::RHI
{
class Framebuffer
{
public:
    virtual ~Framebuffer() = default;

    [[nodiscard]] virtual uint32_t getWidth() const = 0;
    [[nodiscard]] virtual uint32_t getHeight() const = 0;
};
}
