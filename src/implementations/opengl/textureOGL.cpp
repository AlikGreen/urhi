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
            numMipmaps = static_cast<uint32_t>(log(std::max(width, height)))+1;

        arrayLayers = description.arrayLayers;

        type = ConvertOGL::textureTypeToGLType(description.type);

        glCreateTextures(GL_TEXTURE_2D, 1, &handle);

        glTextureParameteri(handle, GL_TEXTURE_MAX_LEVEL, static_cast<int>(numMipmaps) - 1);

        glTextureStorage2D(handle, static_cast<int>(numMipmaps), ConvertOGL::pixelFormatToGL(description.format), static_cast<int>(width), static_cast<int>(height));
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

    void TextureOGL::setData(const TextureUploadDescription uploadDescription) const
    {
        glTextureSubImage2D(handle, 0, 0, 0, static_cast<int>(width), static_cast<int>(height), GL_RGBA, GL_UNSIGNED_BYTE, uploadDescription.data);
    }
}
