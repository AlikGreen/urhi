#pragma once

#include <imgui.h>
#include <neonRHI/neonRHI.h>
#include <neonCore/memory.h>

namespace Neon::RHI
{
class ImGuiBackend
{
public:
    struct InitInfo
    {
        Device *device;
        Framebuffer *framebuffer; // or swapchain format / render pass info
    };

    explicit ImGuiBackend(const InitInfo &initInfo);
    ~ImGuiBackend();

    void newFrame();
    void render(ImDrawData *drawData, CommandList *cmdList);

    void onResize(Framebuffer *newFramebuffer); // optional

private:
    void createFont();
    void createPipeline();
    void updateProjection(const ImDrawData *drawData, CommandList* cmdList) const;

    void ensureBuffers(int vertexCount, int indexCount);
    void uploadDrawData(ImDrawData *drawData);
    void setupRenderState(ImDrawData *drawData, CommandList *cmdList);

private:
    Device* m_device;
    Framebuffer* m_framebuffer;

    Box<Pipeline> m_pipeline;
    Box<Buffer> m_vertexBuffer;
    Box<Buffer> m_indexBuffer;
    Box<Buffer> m_projUniformBuffer;

    Box<Texture> m_fontTexture;
    Box<TextureView> m_fontTextureView;
    Box<Sampler> m_fontSampler;

    size_t m_vertexBufferSize;
    size_t m_indexBufferSize;
};
}
