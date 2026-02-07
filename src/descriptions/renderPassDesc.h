#pragma once
#include <vector>
#include <grl/mem.h>

#include "textureView.h"
#include "enums/loadOp.h"
#include "enums/storeOp.h"
#include <glm/glm.hpp>

namespace urhi
{
struct ColorAttachment
{
     grl::Rc<TextureView> texture = nullptr;

    LoadOp loadOp = LoadOp::Clear;
    StoreOp storeOp = StoreOp::Store;

    glm::vec4 clearColor { 0.0f, 0.0f, 0.0f, 1.0f };
};

struct DepthStencilAttachment
{
    grl::Rc<TextureView> texture = nullptr;

    LoadOp depthLoadOp = LoadOp::Clear;
    StoreOp depthStoreOp = StoreOp::Store;
    float clearDepth = 1.0f;

    LoadOp stencilLoadOp = LoadOp::Clear;
    StoreOp stencilStoreOp = StoreOp::Store;
    uint32_t clearStencil = 0;
};


struct RenderPassDesc
{
    DepthStencilAttachment depthAttachment{};
    std::vector<ColorAttachment> colorAttachments{};
};
}
