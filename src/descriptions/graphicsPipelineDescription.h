#pragma once
#include "blendState.h"
#include "depthState.h"
#include "inputLayout.h"
#include "rasterizerState.h"
#include "shader.h"
#include "enums/primitiveType.h"
#include "enums/fillMode.h"
#include "enums/cullMode.h"
#include "renderTargetsDescription.h"


namespace Neon::RHI
{
    struct GraphicsPipelineDescription
    {
        Shader* shader = nullptr;
        PrimitiveType primitiveType = PrimitiveType::TriangleList;
        InputLayout inputLayout{};
        DepthState depthState{};
        RenderTargetsDescription targetsDescription{};
        RasterizerState rasterizerState{};
        BlendState blendState{};
    };
}
