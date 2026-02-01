#pragma once
#include <neonCore/neonCore.h>

#include "blendState.h"
#include "depthState.h"
#include "inputLayout.h"
#include "rasterizerState.h"
#include "shader.h"
#include "enums/primitiveType.h"
#include "renderTargetsDesc.h"

namespace Neon::RHI
{
    struct GraphicsPipelineDesc
    {
        Rc<Shader> shader = nullptr;
        PrimitiveType primitiveType = PrimitiveType::TriangleList;
        InputLayout inputLayout{};

        RasterizerState rasterizerState{};
        DepthState depthState{};
        BlendState blendState{};

        RenderTargetsDesc targetsDescription{};
    };
}
