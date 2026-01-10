#pragma once
#include <string>

#include "commandList.h"
#include "pipeline.h"
#include "shader.h"

#include "descriptions/graphicsPipelineDescription.h"

#include "buffer.h"
#include "sampler.h"
#include "swapchain.h"
#include "texture.h"
#include "descriptions/computePipelineDescription.h"
#include "descriptions/framebufferDescription.h"
#include "descriptions/samplerDescription.h"
#include "descriptions/swapchainDescription.h"
#include "descriptions/textureDescription.h"
#include "descriptions/textureViewDescription.h"
#include "neonCore/neonCore.h"

namespace Neon::RHI
{
class Device
{
public:
    virtual ~Device() = default;

    virtual Rc<Pipeline> createPipeline(const GraphicsPipelineDescription& description) = 0;
    virtual Rc<Pipeline> createPipeline(const ComputePipelineDescription& description) = 0;

    virtual Rc<Swapchain> createSwapchain(const SwapchainDescription& description) = 0;

    virtual Rc<CommandList> createCommandList() = 0;

    virtual Rc<Texture> createTexture(const TextureDescription& description) = 0;
    virtual Rc<Sampler> createSampler(const SamplerDescription& description) = 0;
    virtual Rc<TextureView> createTextureView(const TextureViewDescription& description) = 0;

    virtual Rc<Framebuffer> createFramebuffer(const FramebufferDescription& description) = 0;

    virtual Rc<Shader> createShaderFromSpirv(std::vector<uint32_t> spirv) = 0;

    virtual Rc<Buffer> createIndexBuffer() = 0;
    virtual Rc<Buffer> createUniformBuffer() = 0;
    virtual Rc<Buffer> createVertexBuffer() = 0;

    virtual void submit(const Rc<CommandList>& commandList) = 0;
};
}
