#pragma once
#include "enums/cullMode.h"
#include "enums/fillMode.h"

namespace urhi
{
    struct RasterizerState
    {
        FillMode fillMode = FillMode::Fill;
        CullMode cullMode = CullMode::None;
        bool enableScissorTest = false;
    };
}
