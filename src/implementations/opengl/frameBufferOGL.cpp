#include "frameBufferOGL.h"

namespace NRHI
{
    FrameBufferOGL::FrameBufferOGL(const GLuint existing) : handle(existing)
    {
    }

    void FrameBufferOGL::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
    }
}
