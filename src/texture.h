#pragma once
#include <cstdint>

#include "enums/pixelFormat.h"
#include "enums/textureType.h"

namespace urhi
{
class Texture
{
public:
    Texture() = default;
    virtual ~Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator= (const Texture&) = delete;

    uint32_t width()  const { return width(0); };
    uint32_t height() const { return height(0); };
    uint32_t depth()  const { return depth(0); };

    [[nodiscard]] virtual uint32_t width(uint32_t mip) const = 0;
    [[nodiscard]] virtual uint32_t height(uint32_t mip) const = 0;
    [[nodiscard]] virtual uint32_t depth(uint32_t mip) const = 0;

    [[nodiscard]] virtual uint32_t mipLevelCount() const = 0;
    [[nodiscard]] virtual PixelFormat format() const = 0;
    [[nodiscard]] virtual TextureType type() const = 0;
};
}
