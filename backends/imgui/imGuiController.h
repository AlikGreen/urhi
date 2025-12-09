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
        Device *device;
        Framebuffer *framebuffer; // or swapchain format / render pass info
        Window *window;
    };

    explicit ImGuiController(const InitInfo &initInfo);
    ~ImGuiController();

    void newFrame();
    void endFrame();

    static void processEvent(const Event &e);

    void setFramebuffer(Framebuffer *newFramebuffer);
private:
    void createFont() const;
    void createPipeline();

    void updateBuffers(const Box<CommandList> &cmdList);
    void updateProjection(const ImDrawData *drawData, const Box<CommandList> &cmdList) const;
    [[nodiscard]] ScissorRect calculateScissorRect(const ImDrawCmd &drawCmd) const;

    static int toImGuiMouseButton(MouseButton button);
    static ImGuiKey toImGuiKey(KeyCode key);
    static ImGuiKeyChord toImGuiMods(KeyMod mod);

    Device* m_device;
    Framebuffer* m_framebuffer;
    Window* m_window;
    ImDrawData* m_drawData{};

    Box<Pipeline> m_pipeline;
    Box<Buffer> m_vertexBuffer;
    Box<Buffer> m_indexBuffer;
    Box<Buffer> m_projUniformBuffer;

    size_t m_vertexBufferSize{};
    size_t m_indexBufferSize{};

    float m_mouseWheel{};
};
}
