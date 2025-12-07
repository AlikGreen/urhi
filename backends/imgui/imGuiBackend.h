#pragma once

#include <imgui.h>
#include <neonRHI/neonRHI.h>
#include <neonCore/neonCore.h>

namespace Neon::RHI
{
class ImGuiBackend
{
public:
    struct InitInfo
    {
        Device *device;
        Framebuffer *framebuffer; // or swapchain format / render pass info
        Window *window;
    };

    explicit ImGuiBackend(const InitInfo &initInfo);
    ~ImGuiBackend();

    void newFrame() const;
    void render(ImDrawData *drawData, CommandList *cmdList);

    void setFramebuffer(Framebuffer *newFramebuffer); // optional

private:
    void createFont();
    void createPipeline();
    void updateProjection(const ImDrawData *drawData, CommandList* cmdList) const;

    Device* m_device;
    Framebuffer* m_framebuffer;
    Window* m_window;

    Box<Pipeline> m_pipeline;
    Box<Buffer> m_vertexBuffer;
    Box<Buffer> m_indexBuffer;
    Box<Buffer> m_projUniformBuffer;

    Box<Texture> m_fontTexture;
    Box<TextureView> m_fontTextureView;
    Box<Sampler> m_fontSampler;

    size_t m_vertexBufferSize{};
    size_t m_indexBufferSize{};
};
}
