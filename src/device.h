#pragma once
#include <string>

#include "commandList.h"
#include "pipeline.h"
#include "shader.h"

#include "descriptions/graphicsPipelineDescription.h"

#include "buffer.h"
#include "swapchain.h"
#include "texture.h"
#include "descriptions/computePipelineDescription.h"
#include "descriptions/framebufferDescription.h"
#include "descriptions/samplerDescription.h"
#include "descriptions/swapchainDescription.h"
#include "descriptions/textureDescription.h"
#include "descriptions/textureViewDescription.h"
#include "enums/shaderType.h"

namespace Neon::RHI
{
class Device
{
public:
    virtual ~Device() = default;

    virtual Pipeline* createPipeline(const GraphicsPipelineDescription& description) = 0;
    virtual Pipeline* createPipeline(const ComputePipelineDescription& description) = 0;

    virtual Swapchain* createSwapchain(const SwapchainDescription& description) = 0;

    virtual CommandList* createCommandList() = 0;

    virtual Texture* createTexture(const TextureDescription& description) = 0;
    virtual Sampler* createSampler(const SamplerDescription& description) = 0;
    virtual TextureView* createTextureView(const TextureViewDescription& description) = 0;

    virtual Framebuffer* createFramebuffer(const FramebufferDescription& description) = 0;

    Shader* createShaderFromSource(const std::string &source, const std::string &filepath = "");

    virtual Buffer* createIndexBuffer() = 0;
    virtual Buffer* createUniformBuffer() = 0;
    virtual Buffer* createVertexBuffer() = 0;

    virtual void submit(CommandList* commandList) = 0;
protected:
    virtual Shader* createShaderFromSpirvImpl(std::unordered_map<ShaderType, std::vector<uint32_t>> spirv) = 0;
};
}
