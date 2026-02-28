#pragma once
#include <grl/grl.h>

#include "colorAttachmentDesc.h"
#include "depthState.h"
#include "rasterizerState.h"
#include "shader.h"
#include "enums/primitiveType.h"

namespace urhi
{
    struct GraphicsPipelineDesc
    {
        std::vector<grl::Rc<Shader>> shaders{};
        PrimitiveType primitiveType = PrimitiveType::TriangleList;

        RasterizerState rasterizerState{};
        DepthState depthState{};

        std::vector<ColorAttachmentDesc> colorAttachments{};
        std::optional<PixelFormat> depthAttachmentFormat;
    };
}
