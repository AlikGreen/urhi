#include "textureViewOGL.h"

#include "convertOGL.h"
#include "textureOGL.h"

namespace Neon::RHI
{
    TextureViewOGL::TextureViewOGL(const TextureViewDescription &description)
    {
        const auto tex = dynamic_cast<TextureOGL*>(description.target);
        glCreateTextures(tex->getType(), 1, &handle);

        glTextureView(handle,
                      tex->getType(),
                      tex->getHandle(),
                      ConvertOGL::pixelFormatToGL(tex->getFormat()),
                      description.baseMipLevel,
                      description.mipLevels,
                      description.baseArrayLayer,
                      description.arrayLayers);
    }

    void TextureViewOGL::bind(const uint32_t binding) const
    {
        glBindTextureUnit(binding, handle);
    }
}
