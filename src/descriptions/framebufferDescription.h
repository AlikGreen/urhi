#pragma once
#include <utility>
#include <vector>
#include <neonCore/neonCore.h>

#include "textureView.h"

namespace Neon::RHI
{
struct FramebufferDescription
{
    Rc<TextureView> depthTarget{};
    std::vector<Rc<TextureView>> colorTargets{};

    FramebufferDescription() = default;

    template<std::same_as<Rc<TextureView>>... Colors>
    explicit FramebufferDescription(Rc<TextureView> depth, Colors... colors) : depthTarget(std::move(depth)), colorTargets{colors...}
    {

    }
};
}
