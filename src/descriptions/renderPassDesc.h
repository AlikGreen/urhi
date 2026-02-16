#pragma once
#include <vector>
#include <grl/mem.h>

#include "textureView.h"
#include "enums/loadOp.h"
#include "enums/storeOp.h"
#include <glm/glm.hpp>

#include "clearValue.h"
#include "rect2D.h"

namespace urhi
{
struct ColorAttachment
{
    grl::Rc<TextureView> target = nullptr;

    LoadOp loadOp = LoadOp::Clear;
    StoreOp storeOp = StoreOp::Store;

    ClearValue clearValue = ClearColorFloat{0.0f, 0.0f, 0.0f, 1.0f};
};

struct DepthStencilAttachment
{
    grl::Rc<TextureView> target = nullptr;

    LoadOp loadOp = LoadOp::Clear;
    StoreOp storeOp = StoreOp::Store;
    float clearDepth = 1.0f;
    uint32_t clearStencil = 0;
};


struct RenderPassDesc
{
    DepthStencilAttachment depthAttachment{};
    std::vector<ColorAttachment> colorAttachments{};
    Rect2D renderArea;
};
}
