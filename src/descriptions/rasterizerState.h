#pragma once
#include "enums/cullMode.h"
#include "enums/fillMode.h"

namespace Neon::RHI
{
    struct RasterizerState
    {
        FillMode fillMode = FillMode::Fill;
        CullMode cullMode = CullMode::None;
        bool enableScissorTest = false;
    };
}
