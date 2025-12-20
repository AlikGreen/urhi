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
        arrayLayers(description.arrayLayers),
        format(description.format),
        target(description.target)
    {
        const auto& tex = std::dynamic_pointer_cast<TextureOGL>(description.target);

        glGenTextures(1, &handle);

        glTextureView(handle,
                      tex->getType(),
                      tex->getHandle(),
                      ConvertOGL::pixelFormatToGL(tex->getFormat()),
                      description.baseMipLevel,
                      description.mipLevels,
                      description.baseArrayLayer,
                      description.arrayLayers);
    }

    TextureViewOGL::~TextureViewOGL()
    {
        glDeleteTextures(1, &handle);
    }

    void TextureViewOGL::bind(const uint32_t binding) const
    {
        glBindTextureUnit(binding, handle);
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
