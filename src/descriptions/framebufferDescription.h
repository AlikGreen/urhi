#pragma once
#include <vector>

#include "textureView.h"

namespace Neon::RHI
{
struct FramebufferDescription
{
    TextureView* depthTarget{};
    std::vector<TextureView*> colorTargets{};

    template<std::same_as<TextureView*>... Colors>
    explicit FramebufferDescription(TextureView* depth, Colors... colors) : depthTarget(depth), colorTargets{colors...}
    {

    }
};
}
