#pragma once
#include "memory.h"
#include "device.h"
#include "descriptions/samplerDescription.h"

namespace Neon::RHI
{
class WindowOGL;
class DeviceOGL final : public Device
{
public:
    explicit DeviceOGL(WindowOGL* window);

    Pipeline* createPipeline(const GraphicsPipelineDescription& description) override;
    Pipeline* createPipeline(const ComputePipelineDescription& description) override;

    CommandList* createCommandList() override;

    Buffer* createIndexBuffer() override;
    Buffer* createUniformBuffer() override;
    Buffer* createVertexBuffer() override;

    Texture* createTexture(const TextureDescription& description) override;
    Sampler* createSampler(const SamplerDescription& description) override;
    TextureView* createTextureView(const TextureViewDescription& description) override;

    void submit(CommandList* commandList) override;
    void swapBuffers() override;

    Framebuffer* getSwapChainFramebuffer() override;
protected:
    Shader* createShaderFromSpirvImpl(std::unordered_map<ShaderType, std::vector<uint32_t>> shadersSpirv) override;
private:
    WindowOGL* window;
};
}
