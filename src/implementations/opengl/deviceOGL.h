#pragma once

#include "device.h"
#include "glad/gl.h"

namespace Neon::RHI
{
class WindowOGL;
class DeviceOGL final : public Device
{
public:
    Rc<Pipeline> createPipeline(const GraphicsPipelineDesc& desc) override;
    Rc<Pipeline> createPipeline(const ComputePipelineDesc& desc) override;

    Rc<CommandList> createCommandList() override;

    Rc<Swapchain> createSwapchain(const SwapchainDesc &desc) override;

    Rc<Buffer> createIndexBuffer() override;
    Rc<Buffer> createUniformBuffer() override;
    Rc<Buffer> createVertexBuffer() override;

    Rc<Texture> createTexture(const TextureDesc& desc) override;
    Rc<Sampler> createSampler(const SamplerDesc& desc) override;
    Rc<TextureView> createTextureView(const TextureViewDesc& desc) override;

    Rc<Shader> createShader(CompiledShader shader) override;

    void endFrame();

    GLuint getOrCreateFb(const RenderPassDesc& desc);

    void submit(const Rc<CommandList>& commandList) override;
private:
    static size_t hashRenderPass(const RenderPassDesc &desc);

    struct CachedFb
    {
        GLuint fbo;
        uint32_t framesSinceUsed;
    };

    std::unordered_map<uint32_t, CachedFb> m_cachedFbs;
};
}
