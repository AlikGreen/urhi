#pragma once

#include <imgui.h>
#include <urhi/urhi.h>

namespace urhi
{
class ImGuiController
{
public:
    struct InitInfo
    {
        grl::Rc<Device> device;
        grl::Rc<Window> window;
    };

    explicit ImGuiController(const InitInfo &initInfo);

    void newFrame();
    void endFrame();

    static void processEvent(const Event &e);

    void updateTextures(const ImDrawData* drawData) const;

    [[nodiscard]] grl::Rc<Texture> getFramebufferTexture() const;
    [[nodiscard]] grl::Rc<TextureView> getFramebufferTextureView() const;
private:
    ImTextureID createTexture(ImTextureData* texData) const;
    void destroyTexture(const ImTextureData* texData) const;

    void createPipeline();

    void resizeRenderTexture(uint32_t width, uint32_t height);
    void updateBuffers();
    void updateProjection(const ImDrawData *drawData, const grl::Rc<RenderPass> &renderPass) const;
    [[nodiscard]] Rect2D calculateScissorRect(const ImDrawCmd &drawCmd) const;

    static int toImGuiMouseButton(MouseButton button);
    static ImGuiKey toImGuiKey(KeyCode key);
    static ImGuiKeyChord toImGuiMods(KeyMod mod);

    grl::Rc<Device> m_device;
    grl::Rc<Texture> m_framebufferTexture;
    grl::Rc<Window> m_window;
    grl::Rc<TextureView> m_renderTexture;
    ImDrawData* m_drawData{};

    grl::Rc<Pipeline> m_pipeline;
    grl::Rc<Buffer> m_vertexBuffer;
    grl::Rc<Buffer> m_indexBuffer;

    size_t m_vertexBufferSize{};
    size_t m_indexBufferSize{};

    float m_mouseWheel{};
};
}
