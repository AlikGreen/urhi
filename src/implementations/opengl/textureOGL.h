#pragma once
#include "texture.h"
#include "descriptions/textureDescription.h"

namespace NRHI
{
class TextureOGL final : public Texture
{
public:
    explicit TextureOGL(const TextureDescription &description);

    void bind(uint32_t binding) const;

    uint32_t getWidth() override;
    uint32_t getHeight() override;
    uint32_t getDepth() override;

    void setData(const void* data) const;
private:
    uint32_t width{}, height{}, depth{};
    uint32_t handle{};
    TextureDescription description;
};
}
