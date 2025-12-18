#pragma once

#include <imgui.h>
#include <neonRHI/neonRHI.h>
#include <neonCore/neonCore.h>

namespace Neon::RHI
{
class ImGuiController
{
public:
    struct InitInfo
    {
        Rc<Device> device;
        Rc<Framebuffer> framebuffer; // or swapchain format / render pass info
        Rc<Window> window;
    };

    explicit ImGuiController(const InitInfo &initInfo);
    ~ImGuiController();

    void newFrame();
    void endFrame();

    static void processEvent(const Event &e);

    void setFramebuffer(const Rc<Framebuffer>& newFramebuffer);
    void updateTextures(const ImDrawData* drawData) const;
private:
    ImTextureID createTexture(ImTextureData* texData) const;
    void destroyTexture(const ImTextureData* texData) const;

    void createPipeline();

    void updateBuffers(const Rc<CommandList> &cmdList);
    void updateProjection(const ImDrawData *drawData, const Rc<CommandList> &cmdList) const;
    [[nodiscard]] ScissorRect calculateScissorRect(const ImDrawCmd &drawCmd) const;

    static int toImGuiMouseButton(MouseButton button);
    static ImGuiKey toImGuiKey(KeyCode key);
    static ImGuiKeyChord toImGuiMods(KeyMod mod);

    Rc<Device> m_device;
    Rc<Framebuffer> m_framebuffer;
    Rc<Window> m_window;
    ImDrawData* m_drawData{};

    Rc<Pipeline> m_pipeline;
    Rc<Buffer> m_vertexBuffer;
    Rc<Buffer> m_indexBuffer;
    Rc<Buffer> m_projUniformBuffer;

    size_t m_vertexBufferSize{};
    size_t m_indexBufferSize{};

    float m_mouseWheel{};
};
}
