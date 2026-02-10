#pragma once

#include "device.h"
#include "glad/gl.h"

namespace urhi
{
class WindowOGL;
class DeviceOGL final : public Device
{
public:
    grl::Rc<Pipeline> createPipeline(const GraphicsPipelineDesc& desc) override;
    grl::Rc<Pipeline> createPipeline(const ComputePipelineDesc& desc) override;

    grl::Rc<CommandList> createCommandList() override;

    grl::Rc<Swapchain> createSwapchain(const SwapchainDesc &desc) override;

    grl::Rc<Buffer> createIndexBuffer() override;
    grl::Rc<Buffer> createVertexBuffer() override;
    grl::Rc<Buffer> createUniformBuffer() override;
    grl::Rc<Buffer> createStorageBuffer() override;

    grl::Rc<Texture> createTexture(const TextureDesc& desc) override;
    grl::Rc<Sampler> createSampler(const SamplerDesc& desc) override;
    grl::Rc<TextureView> createTextureView(const TextureViewDesc& desc) override;

    grl::Rc<Shader> createShader(CompiledShader shader) override;

    void endFrame();

    GLuint getOrCreateFb(const RenderPassDesc& desc);

    void submit(const grl::Rc<CommandList>& commandList) override;
private:
    static size_t hashRenderPass(const RenderPassDesc &desc);

private:
    struct CachedFb
    {
        GLuint fbo;
        uint32_t framesSinceUsed;
    };

    std::unordered_map<uint32_t, CachedFb> m_cachedFbs;
};
}
