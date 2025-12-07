#pragma once
#include "depthState.h"
#include "inputLayout.h"
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
        FillMode fillMode = FillMode::Fill;
        CullMode cullMode = CullMode::None;
        InputLayout inputLayout{};
        DepthState depthState{};
        RenderTargetsDescription targetsDescription{};
        bool enableScissorTest = false;
    };
}
