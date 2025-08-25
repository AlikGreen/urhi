#include "textureOGL.h"

#include <glad/glad.h>

#include "convertOGL.h"

namespace NRHI
{
    TextureOGL::TextureOGL(const TextureDescription &description) : description(description)
    {
        width = description.width;
        height = description.height;
        depth = description.depth;

        glGenTextures(1, &handle);

        glBindTexture(GL_TEXTURE_2D, handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<int>(description.numMipmaps-1));
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(ConvertOGL::textureFormatToGL(description.format)), static_cast<int>(width), static_cast<int>(height), 0, GL_NONE, ConvertOGL::textureFormatToGLType(description.format), nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
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

    void TextureOGL::setData(const void *data) const
    {
        glBindTexture(GL_TEXTURE_2D, handle);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(ConvertOGL::textureFormatToGL(description.format)), static_cast<int>(width), static_cast<int>(height), 0, GL_RGBA, ConvertOGL::textureFormatToGLType(description.format), data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
