#pragma once
#include "device.h"
#include "descriptions/samplerDescription.h"

namespace Neon::RHI
{
class WindowOGL;
class DeviceOGL final : public Device
{
public:
    Pipeline* createPipeline(const GraphicsPipelineDescription& description) override;
    Pipeline* createPipeline(const ComputePipelineDescription& description) override;

    CommandList* createCommandList() override;

    Swapchain* createSwapchain(const SwapchainDescription &description) override;

    Buffer* createIndexBuffer() override;
    Buffer* createUniformBuffer() override;
    Buffer* createVertexBuffer() override;

    Texture* createTexture(const TextureDescription& description) override;
    Sampler* createSampler(const SamplerDescription& description) override;
    TextureView* createTextureView(const TextureViewDescription& description) override;

    Framebuffer* createFramebuffer(const FramebufferDescription &description) override;

    void submit(CommandList* commandList) override;
protected:
    Shader* createShaderFromSpirvImpl(std::unordered_map<ShaderType, std::vector<uint32_t>> shadersSpirv) override;
};
}
