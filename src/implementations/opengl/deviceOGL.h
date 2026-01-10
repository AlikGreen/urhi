#pragma once

#include "device.h"
#include "descriptions/samplerDescription.h"

namespace Neon::RHI
{
    class WindowOGL;
    class DeviceOGL final : public Device
    {
    public:
        Rc<Pipeline> createPipeline(const GraphicsPipelineDescription& description) override;
        Rc<Pipeline> createPipeline(const ComputePipelineDescription& description) override;

        Rc<CommandList> createCommandList() override;

        Rc<Swapchain> createSwapchain(const SwapchainDescription &description) override;

        Rc<Buffer> createIndexBuffer() override;
        Rc<Buffer> createUniformBuffer() override;
        Rc<Buffer> createVertexBuffer() override;

        Rc<Texture> createTexture(const TextureDescription& description) override;
        Rc<Sampler> createSampler(const SamplerDescription& description) override;
        Rc<TextureView> createTextureView(const TextureViewDescription& description) override;
        Rc<Framebuffer> createFramebuffer(const FramebufferDescription &description) override;

        Rc<Shader> createShaderFromSpirv(std::vector<uint32_t> spirv) override;

        void submit(const Rc<CommandList>& commandList) override;;
    };
}