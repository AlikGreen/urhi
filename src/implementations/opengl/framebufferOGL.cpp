#include "framebufferOGL.h"

#include "textureOGL.h"
#include "textureViewOGL.h"

namespace Neon::RHI
{
    FramebufferOGL::FramebufferOGL(const FramebufferDescription &description)
    {
        glGenFramebuffers(1, &handle);
        glBindFramebuffer(GL_FRAMEBUFFER, handle);

        const auto* depth = dynamic_cast<TextureViewOGL *>(description.depthTarget);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth->getHandle(), 0);

        for(int i = 0; i < description.colorTargets.size() && i < 32; i++)
        {
            const auto* texture = dynamic_cast<TextureViewOGL *>(description.colorTargets[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, texture->getHandle(), 0);
        }

        width = depth->getWidth();
        height = depth->getHeight();
    }

    uint32_t FramebufferOGL::getWidth() const
    {
        return width;
    }

    uint32_t FramebufferOGL::getHeight() const
    {
        return height;
    }

    void FramebufferOGL::bind(const GLenum target) const
    {
        glBindFramebuffer(target, handle);
    }

    GLuint FramebufferOGL::getHandle() const
    {
        return handle;
    }
}
