#include "glSwapchain.h"

#include "clogr.h"
#include "glTextureView.h"
#include "GLFW/glfw3.h"

namespace urhi
{
    GlSwapchain::GlSwapchain(const SwapchainDesc& desc)
    {
        m_device = std::dynamic_pointer_cast<GlDevice>(desc.device);
        m_window = std::dynamic_pointer_cast<GlWindow>(desc.window);

        m_backBufferView = grl::makeRc<GlTextureView>();
        m_backBufferView->m_texture = grl::makeRc<GlTexture>();

        uint32_t width = desc.width;
        uint32_t height = desc.height;

        if(width == 0) width = m_window->width();
        if(height == 0) height = m_window->height();

        resize(width, height);
    }

    GlSwapchain::~GlSwapchain()
    {

    }

    void GlSwapchain::resize(const uint32_t width, const uint32_t height)
    {
        m_backBufferView->m_texture->m_width = width;
        m_backBufferView->m_texture->m_height = width;
    }

    grl::Rc<TextureView> GlSwapchain::acquireNextImage()
    {
        return m_backBufferView;
    }

    void GlSwapchain::present()
    {
        glfwSwapBuffers(m_window->handle());
    }
}
