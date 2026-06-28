#pragma once
#include <mutex>
#include <glad/gl.h>

#include "device.h"
#include "logger.h"
#include "descriptions/deviceDesc.h"
#include "enums/queueType.h"


namespace urhi
{
    class GlCommandList;
    class GlContext;

class GlDevice final : public Device
{
public:
    explicit GlDevice(const DeviceDesc& desc, GlContext* context);
    ~GlDevice() override;

    static bool isDepthFormatSupported(GLenum internalFormat);
    static void onDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, const char *message);

    grl::Rc<Pipeline> createPipeline(const GraphicsPipelineDesc &desc) override;
    grl::Rc<Pipeline> createPipeline(const ComputePipelineDesc &desc) override;

    grl::Rc<CommandList> acquireCommandList(QueueType queueType) override;

    grl::Rc<Texture> createTexture(const TextureDesc &desc) override;
    grl::Rc<Sampler> createSampler(const SamplerDesc &desc) override;
    grl::Rc<TextureView> createTextureView(const TextureViewDesc &desc) override;

    grl::Rc<Buffer> createBuffer(const BufferDesc& desc) override;
    grl::Rc<Shader> createShader(const ShaderEntryPoint &entryPoint) override;

    void waitIdle() override;

    void submit(const grl::Rc<CommandList> &cmdList) override;

    [[nodiscard]] clogr::Logger& logger() const;
    GlContext* context() const { return m_context; }

    GLuint pushConstantsUbo() const { return m_pushConstantsUbo; }
    GLuint blitReadFbo() const { return m_blitReadFbo; }
    GLuint blitWriteFbo() const { return m_blitWriteFbo; }

    GLuint getOrCreateFramebuffer(const RenderPassDesc &renderPass);

    GLenum resolveDepthFormat(PixelFormat requested) const;

    float maxAnisotropy() const { return m_maxAnisotropy; }
private:
    static uint32_t hashRenderPass(const RenderPassDesc& renderPass);

private:
    friend class GlSwapchain;
    static constexpr uint32_t kQueueTypes = 3;

    GlContext* m_context;

    std::unordered_map<uint32_t, GLuint> m_framebufferCache{};
    std::vector<grl::Rc<GlCommandList>> m_commandLists{};
    GLuint m_pushConstantsUbo;

    GLuint m_blitReadFbo;
    GLuint m_blitWriteFbo;

    bool m_supportsD24;
    bool m_supportsD32F;
    bool m_supportsD32FStencil;

    float m_maxAnisotropy = 0.0f;
};
}
