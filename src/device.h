#pragma once

#include "commandList.h"
#include "pipeline.h"
#include "shader.h"

#include "buffer.h"
#include "sampler.h"
#include "shaderCompiler.h"
#include "swapchain.h"
#include "texture.h"

#include "descriptions/graphicsPipelineDesc.h"
#include "descriptions/computePipelineDesc.h"
#include "descriptions/samplerDesc.h"
#include "descriptions/swapchainDesc.h"
#include "descriptions/textureDesc.h"
#include "descriptions/textureViewDesc.h"
#include "enums/queueType.h"

namespace urhi
{
class Device
{
public:
    virtual ~Device() = default;

    virtual grl::Rc<Pipeline> createPipeline(const GraphicsPipelineDesc& desc) = 0;
    virtual grl::Rc<Pipeline> createPipeline(const ComputePipelineDesc& desc) = 0;

    virtual grl::Rc<CommandList> acquireCommandList(QueueType queueType) = 0;

    virtual grl::Rc<Texture> createTexture(const TextureDesc& description) = 0;
    virtual grl::Rc<Sampler> createSampler(const SamplerDesc& description) = 0;
    virtual grl::Rc<TextureView> createTextureView(const TextureViewDesc& desc) = 0;

    virtual grl::Rc<Shader> createShader(CompiledShader shader) = 0;

    virtual grl::Rc<Buffer> createIndexBuffer() = 0;
    virtual grl::Rc<Buffer> createVertexBuffer() = 0;
    virtual grl::Rc<Buffer> createUniformBuffer() = 0;
    virtual grl::Rc<Buffer> createStorageBuffer() = 0;

    virtual void submit(const grl::Rc<CommandList>& commandList) = 0;
};
}
