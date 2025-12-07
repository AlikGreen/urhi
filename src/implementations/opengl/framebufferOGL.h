#pragma once
#include "frameBuffer.h"
#include <glad/glad.h>

#include "descriptions/framebufferDescription.h"

namespace Neon::RHI
{
class FramebufferOGL final : public Framebuffer
{
public:
    explicit FramebufferOGL();
    explicit FramebufferOGL(const FramebufferDescription& description);

    [[nodiscard]] uint32_t getWidth() const override;
    [[nodiscard]] uint32_t getHeight() const override;

    void bind() const;
private:
    GLuint handle{};
    uint32_t width{};
    uint32_t height{};
};
}
