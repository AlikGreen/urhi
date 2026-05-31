#include "glTexture.h"

#include <glad/gl.h>

#include "validation.h"
#include "glConvert.h"
#include "glDevice.h"

namespace urhi
{
    GlTexture::GlTexture(GlDevice* device, const TextureDesc &desc)
        : m_width(desc.width), m_height(desc.height), m_depth(desc.depth),
        m_format(desc.format), m_type(desc.type),
        m_device(device)
    {
        m_mipLevels = std::min(desc.maxMipLevels, static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1);

        glGenTextures(1, &m_handle);

        const auto format = GlConvert::internalFormat(m_format);

        switch (m_type)
        {
            case TextureType::Texture1D:
                glBindTexture(GL_TEXTURE_1D, m_handle);
                glTexStorage1D(GL_TEXTURE_1D, m_mipLevels, format, m_width);
                glBindTexture(GL_TEXTURE_1D, 0);
                break;
            case TextureType::Texture2D:
            case TextureType::TextureCube:
                glBindTexture(GL_TEXTURE_2D, m_handle);
                glTexStorage2D(GL_TEXTURE_2D, m_mipLevels, format, m_width, m_height);
                glBindTexture(GL_TEXTURE_1D, 0);
                break;
            case TextureType::Texture3D:
            case TextureType::Texture2DArray:
            case TextureType::TextureCubeArray:
                glBindTexture(GL_TEXTURE_3D, m_handle);
                glTexStorage3D(GL_TEXTURE_3D, m_mipLevels, format, m_width, m_height, m_depth);
                glBindTexture(GL_TEXTURE_1D, 0);
                break;
            default:
                break;
        }
    }


    GlTexture::~GlTexture()
    {
        glDeleteTextures(1, &m_handle);
    }

    uint32_t GlTexture::width(const uint32_t mip) const
    {
        return std::max(1u, m_width >> mip);
    }

    uint32_t GlTexture::height(const uint32_t mip) const
    {
        return std::max(1u, m_height >> mip);
    }

    uint32_t GlTexture::depth(const uint32_t mip) const
    {
        return std::max(1u, m_depth >> mip);
    }

    uint32_t GlTexture::mipLevelCount() const
    {
        return m_mipLevels;
    }

    PixelFormat GlTexture::format() const
    {
        return m_format;
    }

    TextureType GlTexture::type() const
    {
        return m_type;
    }

    bool GlTexture::isSwapchainTexture()
    {
        return m_swapchainTexture;
    }
}
