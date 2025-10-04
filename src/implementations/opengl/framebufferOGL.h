#pragma once
#include "frameBuffer.h"
#include <glad/glad.h>

#include "descriptions/framebufferDescription.h"

namespace Neon::RHI
{
class FramebufferOGL final : public Framebuffer
{
public:
    explicit FramebufferOGL(GLuint existing);
    explicit FramebufferOGL(const FramebufferDescription& description);

    void bind() const;
private:
    GLuint handle{};
};
}
