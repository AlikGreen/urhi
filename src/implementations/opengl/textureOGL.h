#pragma once

#include "texture.h"
#include "descriptions/textureDescription.h"
#include "descriptions/textureUploadDescription.h"
#include "glad/gl.h"

namespace Neon::RHI
{
class TextureOGL final : public Texture
{
public:
    explicit TextureOGL(const TextureDescription &description);
    ~TextureOGL() override;

    void bind(uint32_t binding) const;

    [[nodiscard]] uint32_t getWidth() const override;
    [[nodiscard]] uint32_t getHeight() const override;
    [[nodiscard]] uint32_t getDepth() const override;

    [[nodiscard]] uint32_t getMipLevels() const override;
    [[nodiscard]] uint32_t getArrayLayers() const override;
    [[nodiscard]] PixelFormat getFormat() const override;
    [[nodiscard]] TextureType getType() const override;

    [[nodiscard]] GLenum getGLType() const;
    [[nodiscard]] GLuint getHandle() const;

    void generateMipmaps() const;
    void setData(const TextureUploadDescription &uploadDescription) const;

private:
    uint32_t width{}, height{}, depth{};
    uint32_t numMipmaps{}, arrayLayers{};
    PixelFormat format{};
    TextureType type{};

    GLuint handle{};
};
}
