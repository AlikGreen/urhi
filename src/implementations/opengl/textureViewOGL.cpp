#include "textureViewOGL.h"

#include "convertOGL.h"
#include "textureOGL.h"
#include "debug.h"

namespace Neon::RHI
{
    TextureViewOGL::TextureViewOGL(const TextureViewDescription &description)
        : width(description.target->getWidth())
        , height(description.target->getHeight())
        , depth(description.target->getDepth())
        , mipLevels(description.mipLevels)
        , baseMipLevel(description.baseMipLevel)
        , arrayLayers(description.arrayLayers)
        , format(description.format)
        , target(description.target)
    {
        Debug::ensure(description.target != nullptr, "TextureViewOGL created with null target texture");
        Debug::ensure(description.mipLevels > 0, "Texture view mipLevels must be > 0");
        Debug::ensure(description.arrayLayers > 0, "Texture view arrayLayers must be > 0");

        const auto tex = std::dynamic_pointer_cast<TextureOGL>(description.target);
        Debug::ensure(tex != nullptr, "TextureViewOGL target is not a TextureOGL");

        if (format == PixelFormat::Invalid)
            format = tex->getFormat();

        Debug::ensure(format != PixelFormat::Invalid, "Texture view has invalid pixel format after resolution");

        glGenTextures(1, &handle);

        glTextureView(handle,
                      tex->getGLType(),
                      tex->getHandle(),
                      ConvertOGL::pixelFormatToGL(format),
                      description.baseMipLevel,
                      description.mipLevels,
                      description.baseArrayLayer,
                      description.arrayLayers);


        glTextureParameteri(handle, GL_TEXTURE_BASE_LEVEL, 0);
        glTextureParameteri(handle, GL_TEXTURE_MAX_LEVEL, static_cast<int>(mipLevels) - 1);
    }

    TextureViewOGL::~TextureViewOGL()
    {
        Debug::ensure(handle != 0, "Deleting invalid texture view");
        glDeleteTextures(1, &handle);
    }

    void TextureViewOGL::bind(const uint32_t binding) const
    {
        Debug::ensure(handle != 0, "Binding null texture view");
        glBindTextureUnit(binding, handle);
    }

    void TextureViewOGL::bindImage(const uint32_t binding, const ImageAccess access) const
    {
        Debug::ensure(handle != 0, "bindImage on null texture view");
        Debug::ensure(format != PixelFormat::Invalid, "bindImage on texture view with invalid format");

        const GLenum glAccess = ConvertOGL::imageAccessToGL(access);
        Debug::ensure(glAccess != 0, "Invalid ImageAccess value {}", static_cast<int>(access));

        const GLenum glFormat = ConvertOGL::pixelFormatToGL(format);
        Debug::ensure(glFormat != 0, "Unsupported pixel format for image load/store: {}", static_cast<int>(format));

        const bool layered = target->getType() == TextureType::Texture2DArray ||
                             target->getType() == TextureType::Texture3D ||
                             target->getType() == TextureType::TextureCube ||
                             target->getType() == TextureType::TextureCubeArray;

        glBindImageTexture(
            binding,
            handle,
            static_cast<GLint>(baseMipLevel),
            layered,
            0,
            glAccess,
            glFormat);
    }

    uint32_t TextureViewOGL::getWidth() const
    {
        Debug::ensure(handle != 0, "getWidth called on invalid texture view");
        return width;
    }

    uint32_t TextureViewOGL::getHeight() const
    {
        Debug::ensure(handle != 0, "getHeight called on invalid texture view");
        return height;
    }

    uint32_t TextureViewOGL::getDepth() const
    {
        Debug::ensure(handle != 0, "getDepth called on invalid texture view");
        return depth;
    }

    uint32_t TextureViewOGL::getMipLevels() const
    {
        Debug::ensure(handle != 0, "getMipLevels called on invalid texture view");
        return mipLevels;
    }

    uint32_t TextureViewOGL::getArrayLayers() const
    {
        Debug::ensure(handle != 0, "getArrayLayers called on invalid texture view");
        return arrayLayers;
    }

    PixelFormat TextureViewOGL::getFormat() const
    {
        Debug::ensure(handle != 0, "getFormat called on invalid texture view");
        return format;
    }

    Rc<Texture> TextureViewOGL::getTarget() const
    {
        return target;
    }

    GLuint TextureViewOGL::getHandle() const
    {
        Debug::ensure(handle != 0, "getHandle called on null texture view");
        return handle;
    }
}