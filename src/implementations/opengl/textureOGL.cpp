#include "textureOGL.h"

#include <format>
#include <glad/gl.h>

#include "convertOGL.h"
#include "debug.h"
#include "descriptions/textureUploadDescription.h"

namespace Neon::RHI
{
    TextureOGL::TextureOGL(const TextureDescription &description)
    {
        width = description.width;
        height = description.height;
        depth = description.depth;

        Debug::ensure(width > 0, "Texture width must be greater than 0");
        Debug::ensure(height > 0, "Texture height must be greater than 0");
        Debug::ensure(depth > 0, "Texture depth must be greater than 0");

        format = description.format;

        if(description.numMipmaps > 0)
            numMipmaps = description.numMipmaps;
        else
            numMipmaps = static_cast<uint32_t>(log2(std::max(width, height)))+1;

        arrayLayers = description.arrayLayers;

        type = description.type;

        const GLenum internalFormat = ConvertOGL::pixelFormatToGL(format);

        glCreateTextures(getGLType(), 1, &handle);


        glTextureParameteri(handle, GL_TEXTURE_MAX_LEVEL, static_cast<int>(numMipmaps) - 1);

        if (type == TextureType::Texture1D)
        {
            glTextureStorage1D(handle, static_cast<int>(numMipmaps),
                               internalFormat,
                               static_cast<int>(width));
        }
        else if (type == TextureType::Texture2D || type == TextureType::TextureCube)
        {
            glTextureStorage2D(handle, static_cast<int>(numMipmaps),
                               internalFormat,
                               static_cast<int>(width), static_cast<int>(height));
        }
        else if (type == TextureType::Texture3D)
        {
            glTextureStorage3D(handle, static_cast<int>(numMipmaps),
                               internalFormat,
                               static_cast<int>(width), static_cast<int>(height), static_cast<int>(depth));
        }
        else if (type == TextureType::Texture2DArray || type == TextureType::TextureCubeArray)
        {
            glTextureStorage3D(handle, static_cast<int>(numMipmaps),
                internalFormat,
                static_cast<int>(width), static_cast<int>(height), static_cast<int>(arrayLayers));
        }
    }

    TextureOGL::~TextureOGL()
    {
        glDeleteTextures(1, &handle);
    }

    void TextureOGL::bind(const uint32_t binding) const
    {
        glBindTextureUnit(binding, handle);
    }


    uint32_t TextureOGL::getWidth() const
    {
        return width;
    }

    uint32_t TextureOGL::getHeight() const
    {
        return height;
    }

    uint32_t TextureOGL::getDepth() const
    {
        return depth;
    }

    uint32_t TextureOGL::getMipLevels() const
    {
        return numMipmaps;
    }

    uint32_t TextureOGL::getArrayLayers() const
    {
        return arrayLayers;
    }

    PixelFormat TextureOGL::getFormat() const
    {
        return format;
    }

    TextureType TextureOGL::getType() const
    {
        return type;
    }

    GLenum TextureOGL::getGLType() const
    {
        return ConvertOGL::textureTypeToGL(type);
    }

    GLuint TextureOGL::getHandle() const
    {
        return handle;
    }

    void TextureOGL::generateMipmaps() const
    {
        glGenerateTextureMipmap(handle);
    }

    void TextureOGL::setData(const TextureUploadDescription &uploadDescription) const
    {
        const GLenum uploadType = ConvertOGL::pixelTypeToGL(uploadDescription.pixelType);
        const GLenum uploadFormat = ConvertOGL::pixelLayoutToGL(uploadDescription.pixelLayout);

        if (type == TextureType::Texture2D)
        {
            glTextureSubImage2D(handle,
                static_cast<GLint>(uploadDescription.mipLevel),
                static_cast<GLint>(uploadDescription.x),
                static_cast<GLint>(uploadDescription.y),
                static_cast<GLsizei>(uploadDescription.width),
                static_cast<GLsizei>(uploadDescription.height),
                uploadFormat,
                uploadType,
                uploadDescription.data);
            return;
        }

        if (type == TextureType::Texture3D)
        {
            glTextureSubImage3D(handle,
                static_cast<GLint>(uploadDescription.mipLevel),
                static_cast<GLint>(uploadDescription.x),
                static_cast<GLint>(uploadDescription.y),
                static_cast<GLint>(uploadDescription.z),
                static_cast<GLsizei>(uploadDescription.width),
                static_cast<GLsizei>(uploadDescription.height),
                static_cast<GLsizei>(uploadDescription.depth),
                uploadFormat,
                uploadType,
                uploadDescription.data);

            return;
        }

        if (type == TextureType::TextureCube || type == TextureType::TextureCubeArray || type == TextureType::Texture2DArray)
        {
            glTextureSubImage3D(handle,
                static_cast<GLint>(uploadDescription.mipLevel),
                static_cast<GLint>(uploadDescription.x),
                static_cast<GLint>(uploadDescription.y),
                static_cast<GLint>(uploadDescription.baseLayer),
                static_cast<GLsizei>(uploadDescription.width),
                static_cast<GLsizei>(uploadDescription.height),
                static_cast<GLsizei>(uploadDescription.layerCount),
                uploadFormat,
                uploadType,
                uploadDescription.data);
            return;
        }

        Debug::ensure(false, "Texture type unknown");
    }
}
