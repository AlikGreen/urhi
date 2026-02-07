#pragma once

#include "texture.h"
#include "descriptions/textureDesc.h"
#include "descriptions/textureUploadDesc.h"
#include "glad/gl.h"

namespace urhi
{
class TextureOGL final : public Texture
{
public:
    explicit TextureOGL(const TextureDesc &description);
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
    void setData(const TextureUploadDesc& uploadDescription) const;


private:
    uint32_t width{}, height{}, depth{};
    uint32_t numMipmaps{}, arrayLayers{};
    PixelFormat format{};
    TextureType type{};

    GLuint handle{};
};
}
