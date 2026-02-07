#pragma once
#include <grl/grl.h>

#include "blendState.h"
#include "depthState.h"
#include "inputLayout.h"
#include "rasterizerState.h"
#include "shader.h"
#include "enums/primitiveType.h"
#include "renderTargetsDesc.h"

namespace urhi
{
    struct GraphicsPipelineDesc
    {
        grl::Rc<Shader> shader = nullptr;
        PrimitiveType primitiveType = PrimitiveType::TriangleList;
        InputLayout inputLayout{};

        RasterizerState rasterizerState{};
        DepthState depthState{};
        BlendState blendState{};

        RenderTargetsDesc targetsDescription{};
    };
}
