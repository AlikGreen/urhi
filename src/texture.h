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

    [[nodiscard]] virtual uint32_t width() const = 0;
    [[nodiscard]] virtual uint32_t height() const = 0;
    [[nodiscard]] virtual uint32_t depth() const = 0;

    [[nodiscard]] virtual uint32_t mipLevelCount() const = 0;
    [[nodiscard]] virtual uint32_t arrayLayerCount() const = 0;
    [[nodiscard]] virtual PixelFormat format() const = 0;
    [[nodiscard]] virtual TextureType type() const = 0;
};
}
