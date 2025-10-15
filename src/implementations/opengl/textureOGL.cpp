#include "textureOGL.h"

#include <format>
#include <glad/glad.h>

#include "convertOGL.h"
#include "descriptions/textureUploadDescription.h"

namespace Neon::RHI
{
    TextureOGL::TextureOGL(const TextureDescription &description)
    {
        width = description.dimensions.x;
        height = description.dimensions.y;
        depth = description.dimensions.z;

        format = description.format;

        if(description.numMipmaps > 0)
            numMipmaps = description.numMipmaps;
        else
            numMipmaps = static_cast<uint32_t>(log2(std::max(width, height)))+1;

        arrayLayers = description.arrayLayers;

        type = ConvertOGL::textureTypeToGLType(description.type);

        glCreateTextures(type, 1, &handle);
        glTextureParameteri(handle, GL_TEXTURE_MAX_LEVEL, static_cast<int>(numMipmaps) - 1);

        if (type == GL_TEXTURE_1D)
        {
            glTextureStorage1D(handle, static_cast<int>(numMipmaps),
                               ConvertOGL::pixelFormatToGL(description.format),
                               static_cast<int>(width));
        }
        else if (type == GL_TEXTURE_2D)
        {
            glTextureStorage2D(handle, static_cast<int>(numMipmaps),
                               ConvertOGL::pixelFormatToGL(description.format),
                               static_cast<int>(width), static_cast<int>(height));
        } else if (type == GL_TEXTURE_2D_ARRAY || type == GL_TEXTURE_3D)
        {
            glTextureStorage3D(handle, static_cast<int>(numMipmaps),
                               ConvertOGL::pixelFormatToGL(description.format),
                               static_cast<int>(width), static_cast<int>(height), static_cast<int>(depth));
        }
    }

    void TextureOGL::bind(const uint32_t binding) const
    {
        glBindTextureUnit(binding, handle);
    }


    uint32_t TextureOGL::getWidth()
    {
        return width;
    }

    uint32_t TextureOGL::getHeight()
    {
        return height;
    }

    uint32_t TextureOGL::getDepth()
    {
        return depth;
    }

    uint32_t TextureOGL::getMipLevels()
    {
        return numMipmaps;
    }

    uint32_t TextureOGL::getArrayLayers()
    {
        return arrayLayers;
    }

    PixelFormat TextureOGL::getFormat()
    {
        return format;
    }

    GLenum TextureOGL::getType() const
    {
        return type;
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

        if (type == GL_TEXTURE_2D)
        {
            glTextureSubImage2D(handle,
                static_cast<int>(uploadDescription.mipLevel),
                static_cast<int>(uploadDescription.offset.x),
                static_cast<int>(uploadDescription.offset.y),
                static_cast<int>(uploadDescription.size.x),
                static_cast<int>(uploadDescription.size.y),
                uploadFormat, uploadType,
                uploadDescription.data);
        } else
        {
            glTextureSubImage3D(handle,
                static_cast<int>(uploadDescription.mipLevel),
                static_cast<int>(uploadDescription.offset.x),
                static_cast<int>(uploadDescription.offset.y),
                static_cast<int>(uploadDescription.offset.z),
                static_cast<int>(uploadDescription.size.x),
                static_cast<int>(uploadDescription.size.y),
                static_cast<int>(uploadDescription.size.z),
                uploadFormat, uploadType,
                uploadDescription.data);
        }
    }
}
