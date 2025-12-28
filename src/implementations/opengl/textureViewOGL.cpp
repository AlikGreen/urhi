#include "textureViewOGL.h"

#include "convertOGL.h"
#include "textureOGL.h"

namespace Neon::RHI
{
    TextureViewOGL::TextureViewOGL(const TextureViewDescription &description) :
        width(description.target->getWidth()),
        height(description.target->getHeight()),
        depth(description.target->getDepth()),
        mipLevels(description.mipLevels),
        baseMipLevel(description.baseMipLevel),
        arrayLayers(description.arrayLayers),
        format(description.format),
        target(description.target)
    {
        const auto& tex = std::dynamic_pointer_cast<TextureOGL>(description.target);
        if(format == PixelFormat::Invalid)
            format = tex->getFormat();

        glGenTextures(1, &handle);

        glTextureView(handle,
                      tex->getGLType(),
                      tex->getHandle(),
                      ConvertOGL::pixelFormatToGL(tex->getFormat()),
                      description.baseMipLevel,
                      description.mipLevels,
                      description.baseArrayLayer,
                      description.arrayLayers);

        glTextureParameteri(handle, GL_TEXTURE_BASE_LEVEL, 0);
        glTextureParameteri(handle, GL_TEXTURE_MAX_LEVEL, static_cast<int>(mipLevels) - 1);

        // 2. Set filters to Linear to prevent "Mipmap" lookup defaults
        // This is a fallback in case no Sampler is bound.
        glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 3. Clamp edges (Crucial for Skyboxes)
        glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(handle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    TextureViewOGL::~TextureViewOGL()
    {
        glDeleteTextures(1, &handle);
    }

    void TextureViewOGL::bind(const uint32_t binding) const
    {
        glBindTextureUnit(binding, handle);
    }

    void TextureViewOGL::bindImage(const uint32_t binding, const ImageAccess access) const
    {
        const GLenum glAccess = ConvertOGL::imageAccessToGL(access);

        // If your view knows the format, use that; otherwise fetch from the parent texture.
        const GLenum glFormat = ConvertOGL::pixelFormatToGL(format);
        const bool layered = target->getType() == TextureType::Texture2DArray || target->getType() == TextureType::Texture3D || target->getType() == TextureType::TextureCube || target->getType() == TextureType::TextureCubeArray;

        glBindImageTexture(
            binding,
            static_cast<GLuint>(handle),
            static_cast<GLint>(baseMipLevel),
            layered,
            0,
            glAccess,
            glFormat);
    }

    uint32_t TextureViewOGL::getWidth() const
    {
        return width;
    }

    uint32_t TextureViewOGL::getHeight() const
    {
        return height;
    }

    uint32_t TextureViewOGL::getDepth() const
    {
        return depth;
    }

    uint32_t TextureViewOGL::getMipLevels() const
    {
        return mipLevels;
    }

    uint32_t TextureViewOGL::getArrayLayers() const
    {
        return arrayLayers;
    }

    PixelFormat TextureViewOGL::getFormat() const
    {
        return format;
    }

    GLuint TextureViewOGL::getHandle() const
    {
        return handle;
    }
}
