#pragma once
#include "depthState.h"
#include "shader.h"
#include "enums/primitiveType.h"
#include "enums/fillMode.h"
#include "enums/cullMode.h"
#include "renderTargetsDescription.h"
#include "descriptions/vertexInputState.h"
#include "util/memory.h"

namespace NRHI
{
    struct GraphicsPipelineDescription
    {
        Shader* shader = nullptr;
        PrimitiveType primitiveType = PrimitiveType::TriangleList;
        FillMode fillMode = FillMode::Fill;
        CullMode cullMode = CullMode::None;
        VertexInputState vertexInputState{};
        DepthState depthState{};
        RenderTargetsDescription targetsDescription{};
    };
}
