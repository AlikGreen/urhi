#pragma once
#include "texture.h"
#include "descriptions/textureDescription.h"
#include "descriptions/textureUploadDescription.h"

namespace Neon::RHI
{
class TextureOGL final : public Texture
{
public:
    explicit TextureOGL(const TextureDescription &description);

    void bind(uint32_t binding) const;

    uint32_t getWidth() override;
    uint32_t getHeight() override;
    uint32_t getDepth() override;

    void setData(TextureUploadDescription uploadDescription) const;
private:
    uint32_t width{}, height{}, depth{};
    uint32_t handle{};
    TextureDescription description;
};
}
