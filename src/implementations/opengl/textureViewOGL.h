#pragma once
#include "textureView.h"
#include "descriptions/textureViewDescription.h"
#include "glad/glad.h"

namespace Neon::RHI
{
class TextureViewOGL final : public TextureView
{
public:
    explicit TextureViewOGL(const TextureViewDescription& description);

    void bind(uint32_t binding) const;

    [[nodiscard]] uint32_t getWidth() const override;
    [[nodiscard]] uint32_t getHeight() const override;
    [[nodiscard]] uint32_t getDepth() const override;

    [[nodiscard]] uint32_t getMipLevels() const override;
    [[nodiscard]] uint32_t getArrayLayers() const override;
    [[nodiscard]] PixelFormat getFormat() const override;

    [[nodiscard]] GLuint getHandle() const;
private:
    GLuint handle{};

    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipLevels;
    uint32_t arrayLayers;
    PixelFormat format;

};
}
