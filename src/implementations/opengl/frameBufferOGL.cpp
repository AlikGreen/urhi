#include "frameBufferOGL.h"

namespace Neon::RHI
{
    FrameBufferOGL::FrameBufferOGL(const GLuint existing) : handle(existing)
    {
    }

    void FrameBufferOGL::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
    }
}
