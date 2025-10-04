#pragma once
#include <vector>

#include "texture.h"

namespace Neon::RHI
{
struct FramebufferDescription
{
    Texture* depthTarget{};
    std::vector<Texture*> colorTargets{};

    template<std::same_as<Texture*>... Colors>
    explicit FramebufferDescription(Texture* depth, Colors... colors) : depthTarget(depth), colorTargets{colors...}
    {

    }
};
}
