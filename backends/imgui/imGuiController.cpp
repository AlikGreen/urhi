#include "imGuiController.h"

#include <ImGuizmo.h>

#include <clogr.h>

#include "imGuiExtensions.h"
#include "imguiShader.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "slang/slangCompiler.h"

namespace urhi
{
    ImGuiController::ImGuiController(const InitInfo &initInfo)
    {
        m_device = initInfo.device;
        m_window = initInfo.window;

        ImGuiIO &io = ImGui::GetIO();
        io.BackendPlatformName = "urhi_Platform";
        io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures | ImGuiBackendFlags_RendererHasVtxOffset;
        io.Fonts->TexDesiredFormat = ImTextureFormat_RGBA32;
        io.DisplaySize = ImVec2(
            static_cast<float>(m_window->width()),
            static_cast<float>(m_window->height())
        );

        createPipeline();
    }

    void ImGuiController::newFrame()
    {
        ImGuiIO& io = ImGui::GetIO();

        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        io.MouseWheel = m_mouseWheel;
        m_mouseWheel = 0.0f;

        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
        ImGui::ClearTextureCache();
    }


    void ImGuiController::endFrame()
    {
        ImGui::Render();
        m_drawData = ImGui::GetDrawData();

        updateTextures(m_drawData);
        const ImGuiIO& io = ImGui::GetIO();
        const auto width = static_cast<uint32_t>(io.DisplaySize.x);
        const auto height = static_cast<uint32_t>(io.DisplaySize.y);
        resizeRenderTexture(width, height);

        if(m_drawData == nullptr || m_drawData->TotalVtxCount == 0)
            return;

        const grl::Rc<CommandList> cmdList = m_device->acquireCommandList(QueueType::Graphics);

        cmdList->begin();

        updateBuffers();

        std::vector<ImDrawVert> vertices{};
        vertices.reserve(m_drawData->TotalVtxCount);

        std::vector<uint32_t> indices{};
        indices.reserve(m_drawData->TotalIdxCount);

        std::vector<uint32_t> indexOffsets{};
        indexOffsets.resize(m_drawData->CmdListsCount);

        std::vector<uint32_t> vertexOffsets{};
        vertexOffsets.resize(m_drawData->CmdListsCount);

        uint32_t vertexOffset = 0;
        uint32_t indexOffset  = 0;

        // bottom up caller graph shows this taking a bit
        for(int n = 0; n < m_drawData->CmdListsCount; n++)
        {
            const ImDrawList *cmdListImGui = m_drawData->CmdLists[n];
            indexOffsets[n] = indexOffset;
            vertexOffsets[n] = vertexOffset;

            vertices.insert(
            vertices.end(),
            cmdListImGui->VtxBuffer.begin(),
            cmdListImGui->VtxBuffer.end());

            indices.insert(
                indices.end(),
                cmdListImGui->IdxBuffer.begin(),
                cmdListImGui->IdxBuffer.end());


            vertexOffset += cmdListImGui->VtxBuffer.Size;
            indexOffset  += cmdListImGui->IdxBuffer.Size;
        }

        cmdList->updateBuffer(m_vertexBuffer, vertices);
        cmdList->updateBuffer(m_indexBuffer, indices);

        RenderPassDesc renderPassDesc{};
        ColorAttachment attachment{};
        attachment.target   = m_renderTexture;
        attachment.loadOp   = LoadOp::Clear;
        attachment.storeOp  = StoreOp::Store;
        attachment.clearValue = ClearColorFloat{ 0.0f, 0.0f, 0.0f, 0.0f };

        renderPassDesc.colorAttachments.push_back(attachment);

        auto& renderPass = cmdList->beginRenderPass(renderPassDesc);

        renderPass.setPipeline(m_pipeline);

        renderPass.setVertexBuffer(0, m_vertexBuffer);
        renderPass.setIndexBuffer(m_indexBuffer, IndexFormat::UInt32);

        updateProjection(m_drawData, renderPass);

        for(int n = 0; n < m_drawData->CmdListsCount; n++)
        {
            const ImDrawList* cmdListImGui = m_drawData->CmdLists[n];
            const uint32_t baseVertex = vertexOffsets[n];
            const uint32_t baseIndex  = indexOffsets[n];

            for(int cmd_i = 0; cmd_i < cmdListImGui->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd& pcmd = cmdListImGui->CmdBuffer[cmd_i];

                renderPass.setScissor(calculateScissorRect(pcmd));

                ImGuiImage* image = pcmd.GetTexID();

                clogr::ensure(image != nullptr, "ImGui - Image is null");
                clogr::ensure(image->view != nullptr, "ImGui - Texture View is null");

                if(image->sampler == nullptr)
                {
                    SamplerDesc samplerDesc{};
                    image->sampler = m_device->createSampler(samplerDesc);
                }


                renderPass.setTexture("ImGuiTexture", image->view);
                renderPass.setSampler("ImGuiSampler", image->sampler);

                renderPass.drawIndexed(
                    pcmd.ElemCount,
                    1,
                    baseIndex + pcmd.IdxOffset,
                    static_cast<int32_t>(baseVertex + pcmd.VtxOffset)
                );
            }
        }

        renderPass.end();
        m_device->submit(cmdList);
    }

    void ImGuiController::processEvent(const Event& e)
    {
        ImGuiIO& io = ImGui::GetIO();

        switch (e.type)
        {
            case Event::Type::MouseMotion:
            {
                const auto& motion = std::get<Event::MouseMotionEvent>(e.data);
                io.AddMousePosEvent(motion.x, motion.y);
                break;
            }
            case Event::Type::MouseButtonDown:
            {
                const auto& buttonEvent = std::get<Event::MouseButtonEvent>(e.data);
                const int button = toImGuiMouseButton(buttonEvent.button);
                io.AddMouseButtonEvent(button, true);
                break;
            }
            case Event::Type::MouseButtonUp:
            {
                const auto& buttonEvent = std::get<Event::MouseButtonEvent>(e.data);
                const int button = toImGuiMouseButton(buttonEvent.button);
                io.AddMouseButtonEvent(button, false);
                break;
            }
            case Event::Type::MouseWheel:
            {
                const auto& wheel = std::get<Event::MouseWheelEvent>(e.data);
                io.AddMouseWheelEvent(static_cast<float>(wheel.x), static_cast<float>(wheel.y));
                break;
            }
            case Event::Type::KeyDown:
            {
                const auto& keyEvent = std::get<Event::KeyEvent>(e.data);
                const ImGuiKey key = toImGuiKey(keyEvent.key);
                io.AddKeyEvent(key, true);
                break;
            }
            case Event::Type::KeyUp:
            {
                const auto& keyEvent = std::get<Event::KeyEvent>(e.data);
                const ImGuiKey key = toImGuiKey(keyEvent.key);
                io.AddKeyEvent(key, false);
                break;
            }
            case Event::Type::TextInput:
            {
                const auto& text = std::get<Event::TextInputEvent>(e.data);
                io.AddInputCharacter(text.codepoint);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void ImGuiController::updateTextures(const ImDrawData* drawData) const
    {
        if(drawData == nullptr || drawData->Textures == nullptr)
            return;

        for(ImTextureData* texData : *drawData->Textures)
        {
            switch(texData->Status)
            {
                case ImTextureStatus_WantCreate:
                {
                    ImGuiImage* img = createTexture(texData);
                    texData->SetTexID(img);
                    texData->SetStatus(ImTextureStatus_OK);
                    break;
                }

                case ImTextureStatus_WantDestroy:
                {
                    destroyTexture(texData);
                    texData->SetTexID(ImTextureID_Invalid);
                    texData->SetStatus(ImTextureStatus_Destroyed);
                    break;
                }

                case ImTextureStatus_WantUpdates:
                {
                    // TODO implement partial updates if i care.
                    destroyTexture(texData);
                    ImGuiImage* img = createTexture(texData);
                    texData->SetTexID(img);
                    texData->SetStatus(ImTextureStatus_OK);
                    break;
                }

                default:
                    break;
            }
        }
    }

    grl::Rc<Texture> ImGuiController::getFramebufferTexture() const
    {
        return m_framebufferTexture;
    }

    grl::Rc<TextureView> ImGuiController::getFramebufferTextureView() const
    {
        return m_renderTexture;
    }

    ImTextureID ImGuiController::createTexture(ImTextureData *texData) const
    {
        const void *pixels = texData->GetPixels();
        const int width = texData->Width;
        const int height = texData->Height;

        TextureDesc texDesc{};
        texDesc.width = width;
        texDesc.height = height;
        texDesc.maxMipLevels = 1;
        texDesc.format = PixelFormat::RGBA8UNorm;
        texDesc.usage = TextureUsage::Sampled;

        const grl::Rc<Texture> fontTexture = m_device->createTexture(texDesc);
        TextureUploadDesc uploadDesc{};

        uploadDesc.texture = fontTexture;
        uploadDesc.data = pixels;
        uploadDesc.width = width;
        uploadDesc.height = height;

        // Upload pixels to the texture with a command list or staging buffer
        const grl::Rc<CommandList>& cmdList = m_device->acquireCommandList(QueueType::Graphics);

        cmdList->begin();
        cmdList->updateTexture(uploadDesc);
        m_device->submit(cmdList);

        const grl::Rc<TextureView>& fontTextureView = m_device->createTextureView(fontTexture);

        SamplerDesc samplerDesc{};
        samplerDesc.minFilter = TextureFilter::Linear;
        samplerDesc.magFilter = TextureFilter::Linear;
        samplerDesc.mipmapFilter = MipmapFilter::Linear;
        samplerDesc.addressModeU = AddressMode::ClampToEdge;
        samplerDesc.addressModeV = AddressMode::ClampToEdge;
        samplerDesc.addressModeW = AddressMode::ClampToEdge;

        const grl::Rc<Sampler>& fontSampler = m_device->createSampler(samplerDesc);

        return new ImGuiImage{ fontTextureView, fontSampler };
    }

    void ImGuiController::destroyTexture(const ImTextureData *texData) const
    {
        const ImGuiImage* img = texData->GetTexID();
        delete img;
    }

    void ImGuiController::createPipeline()
    {
        SlangSource source{};
        source.path = "imgui.slang";
        source.source = imGuiShaderSource;

        const auto module = SlangCompiler::compile(source);
        if (!module.has_value())
            clogr::fatal("Failed to compile imgui shader: {}", module.error());

        SlangLinkDesc linkDesc{};
        linkDesc.modules = { module.value() };

        const auto linked = SlangCompiler::link(linkDesc);
        if (!linked.has_value())
            clogr::fatal("Failed to link imgui shader: {}", module.error());

        const auto vertexShader = m_device->createShader(SlangCompiler::extract(linked.value(), ShaderStage::Vertex).value());
        const auto fragmentShader = m_device->createShader(SlangCompiler::extract(linked.value(), ShaderStage::Fragment).value());

        GraphicsPipelineDesc pipelineDescription{};
        pipelineDescription.shaders         = { vertexShader, fragmentShader };
        pipelineDescription.primitiveType   = PrimitiveType::TriangleList;
        pipelineDescription.rasterizerState = { .cullMode = CullMode::None, .enableScissorTest = true };
        pipelineDescription.depthState      = { .hasDepthTarget = false, .enableDepthTest = false };
        pipelineDescription.colorAttachments = {
            ColorAttachmentDesc{
                .format = PixelFormat::RGBA8UNorm,
                .blend  = BlendState::alphaBlend()
            }
        };

        m_pipeline = m_device->createPipeline(pipelineDescription);
    }

    void ImGuiController::resizeRenderTexture(const uint32_t width, const uint32_t height)
    {
        if(m_renderTexture != nullptr && m_renderTexture->texture()->width() == width && m_renderTexture->texture()->height() == height)
            return;

        TextureDesc fbTexDesc{};
        fbTexDesc.width = width;
        fbTexDesc.height = height;
        fbTexDesc.maxMipLevels = 1;
        fbTexDesc.type = TextureType::Texture2D;
        fbTexDesc.usage = TextureUsage::ColorTarget | TextureUsage::Sampled;
        fbTexDesc.format = PixelFormat::RGBA8UNorm;

        m_framebufferTexture = m_device->createTexture(fbTexDesc);
        m_renderTexture = m_device->createTextureView(m_framebufferTexture);
    }

    void ImGuiController::updateBuffers()
    {
        const size_t vertexDataSize = m_drawData->TotalVtxCount * sizeof(ImDrawVert);
        const size_t indexDataSize = m_drawData->TotalIdxCount * sizeof(uint32_t);

        if(m_vertexBuffer == nullptr ||  m_vertexBufferSize < vertexDataSize)
        {
            m_vertexBuffer = m_device->createBuffer({BufferUsage::Vertex, vertexDataSize});
            m_vertexBufferSize = vertexDataSize;
        }

        if(m_indexBuffer == nullptr ||  m_indexBufferSize < indexDataSize)
        {
            m_indexBuffer  = m_device->createBuffer({BufferUsage::Index, indexDataSize});
            m_indexBufferSize = indexDataSize;
        }
    }

    void ImGuiController::updateProjection(const ImDrawData *drawData, RenderPass& renderPass) const
    {
        const ImVec2 displayPos    = drawData->DisplayPos;
        const ImVec2 displaySize   = drawData->DisplaySize;
        // ImVec2 framebufferScale = drawData->FramebufferScale;

        const float L = displayPos.x;
        const float R = displayPos.x + displaySize.x;
        const float T = displayPos.y + displaySize.y;
        const float B = displayPos.y;

        glm::mat4 projMatrix = glm::ortho(L, R, B, T, -1.0f, 1.0f);

        renderPass.pushConstants(projMatrix);
    }

    Rect2D ImGuiController::calculateScissorRect(const ImDrawCmd &drawCmd) const
    {
        const ImVec2 clipOff   = m_drawData->DisplayPos;
        const ImVec2 clipScale = m_drawData->FramebufferScale;

        ImVec4 clipRect;
        clipRect.x = (drawCmd.ClipRect.x - clipOff.x) * clipScale.x;
        clipRect.y = (drawCmd.ClipRect.y - clipOff.y) * clipScale.y;
        clipRect.z = (drawCmd.ClipRect.z - clipOff.x) * clipScale.x;
        clipRect.w = (drawCmd.ClipRect.w - clipOff.y) * clipScale.y;

        if(clipRect.x >= clipRect.z || clipRect.y >= clipRect.w)
            return Rect2D{};

        Rect2D scissor{};
        scissor.x      = std::max(0, static_cast<int>(clipRect.x));
        scissor.y      = std::max(0, static_cast<int>(clipRect.y));
        scissor.width  = static_cast<int>(clipRect.z - clipRect.x);
        scissor.height = static_cast<int>(clipRect.w - clipRect.y);

        return scissor;
    }

    int ImGuiController::toImGuiMouseButton(const MouseButton button)
    {
        switch(button)
        {
            case MouseButton::Left:   return 0;
            case MouseButton::Right:  return 1;
            case MouseButton::Middle: return 2;
            case MouseButton::Side1:  return 3;
            case MouseButton::Side2:  return 4;
            default:                  return -1;
        }
    }

    ImGuiKey ImGuiController::toImGuiKey(const KeyCode key)
    {
        switch(key)
        {
            // Letters
            case KeyCode::A: return ImGuiKey_A;
            case KeyCode::B: return ImGuiKey_B;
            case KeyCode::C: return ImGuiKey_C;
            case KeyCode::D: return ImGuiKey_D;
            case KeyCode::E: return ImGuiKey_E;
            case KeyCode::F: return ImGuiKey_F;
            case KeyCode::G: return ImGuiKey_G;
            case KeyCode::H: return ImGuiKey_H;
            case KeyCode::I: return ImGuiKey_I;
            case KeyCode::J: return ImGuiKey_J;
            case KeyCode::K: return ImGuiKey_K;
            case KeyCode::L: return ImGuiKey_L;
            case KeyCode::M: return ImGuiKey_M;
            case KeyCode::N: return ImGuiKey_N;
            case KeyCode::O: return ImGuiKey_O;
            case KeyCode::P: return ImGuiKey_P;
            case KeyCode::Q: return ImGuiKey_Q;
            case KeyCode::R: return ImGuiKey_R;
            case KeyCode::S: return ImGuiKey_S;
            case KeyCode::T: return ImGuiKey_T;
            case KeyCode::U: return ImGuiKey_U;
            case KeyCode::V: return ImGuiKey_V;
            case KeyCode::W: return ImGuiKey_W;
            case KeyCode::X: return ImGuiKey_X;
            case KeyCode::Y: return ImGuiKey_Y;
            case KeyCode::Z: return ImGuiKey_Z;

            // Number row
            case KeyCode::Num0: return ImGuiKey_0;
            case KeyCode::Num1: return ImGuiKey_1;
            case KeyCode::Num2: return ImGuiKey_2;
            case KeyCode::Num3: return ImGuiKey_3;
            case KeyCode::Num4: return ImGuiKey_4;
            case KeyCode::Num5: return ImGuiKey_5;
            case KeyCode::Num6: return ImGuiKey_6;
            case KeyCode::Num7: return ImGuiKey_7;
            case KeyCode::Num8: return ImGuiKey_8;
            case KeyCode::Num9: return ImGuiKey_9;

            // Basic controls
            case KeyCode::Return:    return ImGuiKey_Enter;
            case KeyCode::Escape:    return ImGuiKey_Escape;
            case KeyCode::Backspace: return ImGuiKey_Backspace;
            case KeyCode::Tab:       return ImGuiKey_Tab;
            case KeyCode::Space:     return ImGuiKey_Space;
            case KeyCode::Delete:    return ImGuiKey_Delete;
            case KeyCode::Insert:    return ImGuiKey_Insert;

            // Arrows
            case KeyCode::Left:  return ImGuiKey_LeftArrow;
            case KeyCode::Right: return ImGuiKey_RightArrow;
            case KeyCode::Up:    return ImGuiKey_UpArrow;
            case KeyCode::Down:  return ImGuiKey_DownArrow;

            // Home / End / PgUp / PgDn
            case KeyCode::Home:     return ImGuiKey_Home;
            case KeyCode::End:      return ImGuiKey_End;
            case KeyCode::PageUp:   return ImGuiKey_PageUp;
            case KeyCode::PageDown: return ImGuiKey_PageDown;

            // Function keys
            case KeyCode::F1:  return ImGuiKey_F1;
            case KeyCode::F2:  return ImGuiKey_F2;
            case KeyCode::F3:  return ImGuiKey_F3;
            case KeyCode::F4:  return ImGuiKey_F4;
            case KeyCode::F5:  return ImGuiKey_F5;
            case KeyCode::F6:  return ImGuiKey_F6;
            case KeyCode::F7:  return ImGuiKey_F7;
            case KeyCode::F8:  return ImGuiKey_F8;
            case KeyCode::F9:  return ImGuiKey_F9;
            case KeyCode::F10: return ImGuiKey_F10;
            case KeyCode::F11: return ImGuiKey_F11;
            case KeyCode::F12: return ImGuiKey_F12;

            // Modifiers as keys
            case KeyCode::LCtrl:  return ImGuiKey_LeftCtrl;
            case KeyCode::RCtrl:  return ImGuiKey_RightCtrl;
            case KeyCode::LShift: return ImGuiKey_LeftShift;
            case KeyCode::RShift: return ImGuiKey_RightShift;
            case KeyCode::LAlt:   return ImGuiKey_LeftAlt;
            case KeyCode::RAlt:   return ImGuiKey_RightAlt;
            case KeyCode::LGui:   return ImGuiKey_LeftSuper;
            case KeyCode::RGui:   return ImGuiKey_RightSuper;

            default:
                return ImGuiKey_None;
        }
    }

    ImGuiKeyChord ImGuiController::toImGuiMods(const KeyMod mod)
    {
        ImGuiKeyChord out = ImGuiMod_None;

        switch(mod)
        {
            case KeyMod::LShift:
            case KeyMod::RShift:
            case KeyMod::Shift:
                out |= ImGuiMod_Shift;
                break;

            case KeyMod::LCtrl:
            case KeyMod::RCtrl:
            case KeyMod::Ctrl:
                out |= ImGuiMod_Ctrl;
                break;

            case KeyMod::LAlt:
            case KeyMod::RAlt:
            case KeyMod::Alt:
                out |= ImGuiMod_Alt;
                break;

            case KeyMod::LGui:
            case KeyMod::RGui:
            case KeyMod::Gui:
                out |= ImGuiMod_Super;
                break;

            default:
                break;
        }

        // Num/Caps/Scroll if you care:
        if(mod == KeyMod::Num)
            out |= ImGuiMod_None; // ImGui doesn't track NumLock as a mod used for shortcuts
        if(mod == KeyMod::Caps)
            out |= ImGuiMod_None; // same here
        if(mod == KeyMod::Scroll)
            out |= ImGuiMod_None;

        return out;
    }
}
