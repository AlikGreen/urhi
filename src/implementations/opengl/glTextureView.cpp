#include "glTextureView.h"

#include <glad/gl.h>

#include "glConvert.h"
#include "glDevice.h"
#include "glTexture.h"

namespace urhi
{
    GlTextureView::GlTextureView(GlDevice *device, const TextureViewDesc &desc)
        : m_device(device), m_baseMipLevel(desc.firstMip), m_baseArrayLayer(desc.firstLayer),
        m_mipLevels(desc.mipCount), m_arrayLayers(desc.layerCount), m_format(desc.format)
    {
        m_texture = std::static_pointer_cast<GlTexture>(desc.texture);

        glGenTextures(1, &m_handle);

        glTextureView(
            m_handle,
            GlConvert::textureType(m_texture->type()),
            m_texture->handle(),
            GlConvert::internalFormat(m_texture->format()),
            m_baseMipLevel, m_mipLevels,
            m_baseArrayLayer, m_arrayLayers
        );

    }

    GlTextureView::GlTextureView(): m_device(nullptr), m_baseMipLevel(0), m_baseArrayLayer(0), m_mipLevels(1),
                                    m_arrayLayers(1), m_format(PixelFormat::Unknown)
    {
        m_texture = grl::makeRc<GlTexture>();
    }

    GlTextureView::~GlTextureView()
    {
        if(m_handle == 0) return;

        glDeleteTextures(1, &m_handle);
    }
}
