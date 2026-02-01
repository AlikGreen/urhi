#pragma once

#include "commandList.h"
#include "pipeline.h"
#include "shader.h"

#include "buffer.h"
#include "sampler.h"
#include "shaderCompiler.h"
#include "swapchain.h"
#include "texture.h"
#include "neonCore/neonCore.h"

#include "descriptions/graphicsPipelineDesc.h"
#include "descriptions/computePipelineDesc.h"
#include "descriptions/samplerDesc.h"
#include "descriptions/swapchainDesc.h"
#include "descriptions/textureDesc.h"
#include "descriptions/textureViewDesc.h"

namespace Neon::RHI
{
class Device
{
public:
    virtual ~Device() = default;

    virtual Rc<Pipeline> createPipeline(const GraphicsPipelineDesc& desc) = 0;
    virtual Rc<Pipeline> createPipeline(const ComputePipelineDesc& desc) = 0;

    virtual Rc<Swapchain> createSwapchain(const SwapchainDesc& desc) = 0;

    virtual Rc<CommandList> createCommandList() = 0;

    virtual Rc<Texture> createTexture(const TextureDesc& description) = 0;
    virtual Rc<Sampler> createSampler(const SamplerDesc& description) = 0;
    virtual Rc<TextureView> createTextureView(const TextureViewDesc& desc) = 0;

    virtual Rc<Shader> createShader(CompiledShader shader) = 0;

    virtual Rc<Buffer> createIndexBuffer() = 0;
    virtual Rc<Buffer> createUniformBuffer() = 0;
    virtual Rc<Buffer> createVertexBuffer() = 0;

    virtual void submit(const Rc<CommandList>& commandList) = 0;
};
}
