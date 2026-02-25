#pragma once

#include "commandList.h"
#include "pipeline.h"
#include "shader.h"

#include "buffer.h"
#include "sampler.h"
#include "texture.h"
#include "descriptions/bufferDesc.h"

#include "descriptions/graphicsPipelineDesc.h"
#include "descriptions/computePipelineDesc.h"
#include "descriptions/samplerDesc.h"
#include "descriptions/shaderEntryPoint.h"
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

    virtual grl::Rc<Texture> createTexture(const TextureDesc& desc) = 0;
    virtual grl::Rc<Sampler> createSampler(const SamplerDesc& desc) = 0;

    grl::Rc<TextureView> createTextureView(const grl::Rc<Texture> &texture) { return createTextureView(TextureViewDesc(texture)); }
    virtual grl::Rc<TextureView> createTextureView(const TextureViewDesc& desc) = 0;

    virtual grl::Rc<Shader> createShader(const ShaderEntryPoint& entryPoint) = 0;
    virtual grl::Rc<Buffer> createBuffer(const BufferDesc& desc) = 0;

    virtual void submit(const grl::Rc<CommandList>& commandList) = 0;
    virtual void waitIdle() = 0;

};
}
