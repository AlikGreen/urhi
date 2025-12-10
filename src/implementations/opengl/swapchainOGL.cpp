#include "swapchainOGL.h"

#include "window.h"
#include "glad/glad.h"

namespace Neon::RHI
{
    SwapchainOGL::SwapchainOGL(const SwapchainDescription &desc)
    {
        window = desc.window;
        width = window->getWidth();
        height = window->getHeight();
    }

    uint32_t SwapchainOGL::acquireNextImage()
    {
        return 0;
    }

    void SwapchainOGL::present(const uint32_t imageIndex)
    {
        const FramebufferOGL framebuffer = framebuffers[imageIndex];

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.getHandle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(
            0, 0,
            static_cast<int>(width),
            static_cast<int>(height),
            0, 0,
            static_cast<int>(width),
            static_cast<int>(height),
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        window->swapBuffers();

        // Maybe fence things
    }

    void SwapchainOGL::resize(const uint32_t width, const uint32_t height)
    {
        this->width = width;
        this->height = height;
    }
}
