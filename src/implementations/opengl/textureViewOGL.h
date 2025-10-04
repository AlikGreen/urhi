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
private:
    GLuint handle{};
};
}
