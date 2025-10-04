#include "framebufferOGL.h"

#include "textureOGL.h"

namespace Neon::RHI
{
    FramebufferOGL::FramebufferOGL(const GLuint existing) : handle(existing)
    {

    }

    FramebufferOGL::FramebufferOGL(const FramebufferDescription &description)
    {
        glGenFramebuffers(1, &handle);
        glBindFramebuffer(GL_FRAMEBUFFER, handle);

        const auto* depth = dynamic_cast<TextureOGL *>(description.depthTarget);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth->getHandle(), 0);

        for(int i = 0; i < description.colorTargets.size() && i < 32; i++)
        {
            const auto* texture = dynamic_cast<TextureOGL *>(description.colorTargets[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, texture->getHandle(), 0);
        }
    }

    void FramebufferOGL::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
    }
}
