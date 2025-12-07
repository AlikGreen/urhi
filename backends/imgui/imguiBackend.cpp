#include "imguiBackend.h"
#include "imguiShader.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp"

namespace Neon::RHI
{
    ImGuiBackend::ImGuiBackend(const InitInfo &initInfo)
    {
        m_device = initInfo.device;
        m_framebuffer = initInfo.framebuffer;
        m_window = initInfo.window;

        m_projUniformBuffer = Box<Buffer>(m_device->createUniformBuffer());

        CommandList* commandList = m_device->createCommandList();

        commandList->begin();
        commandList->reserveBuffer(m_projUniformBuffer.get(), sizeof(glm::mat4));
        m_device->submit(commandList);

        createPipeline();
        createFont();
    }

    ImGuiBackend::~ImGuiBackend() = default;

    void ImGuiBackend::newFrame() const
    {
        auto io = ImGui::GetIO();
        io.DisplaySize = ImVec2(
            static_cast<float>(m_window->getWidth()),
            static_cast<float>(m_window->getHeight())
        );

        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }

    void ImGuiBackend::render(ImDrawData *drawData, CommandList *cmdList)
    {
        if(drawData == nullptr || drawData->TotalVtxCount == 0)
            return;

        const size_t vertexDataSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
        const size_t indexDataSize = drawData->TotalIdxCount * sizeof(uint32_t);

        if(m_vertexBuffer == nullptr ||  m_vertexBufferSize < vertexDataSize)
        {
            m_vertexBuffer  = Box<Buffer>(m_device->createVertexBuffer());
            cmdList->reserveBuffer(m_vertexBuffer.get(), vertexDataSize);
            m_vertexBufferSize = vertexDataSize;
        }

        if(m_indexBuffer == nullptr ||  m_indexBufferSize < indexDataSize)
        {
            m_indexBuffer  = Box<Buffer>(m_device->createIndexBuffer());
            cmdList->reserveBuffer(m_indexBuffer.get(), indexDataSize);
            m_indexBufferSize = indexDataSize;
        }

        std::vector<ImDrawVert> vertices{};
        vertices.reserve(drawData->TotalVtxCount);

        std::vector<uint32_t> indices{};
        indices.reserve(drawData->TotalIdxCount);

        std::vector<uint32_t> listIndexOffsets{};
        listIndexOffsets.resize(drawData->CmdListsCount);

        uint32_t vertexOffset = 0;
        uint32_t indexOffset  = 0;

        for(int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList *cmdListImGui = drawData->CmdLists[n];
            listIndexOffsets[n] = indexOffset;

            for(int v = 0; v < cmdListImGui->VtxBuffer.Size; v++)
                vertices.push_back(cmdListImGui->VtxBuffer[v]);

            for(int i = 0; i < cmdListImGui->IdxBuffer.Size; i++)
                indices.push_back(static_cast<uint32_t>(cmdListImGui->IdxBuffer[i]) + vertexOffset);

            vertexOffset += cmdListImGui->VtxBuffer.Size;
            indexOffset  += cmdListImGui->IdxBuffer.Size;
        }

        cmdList->updateBuffer(m_vertexBuffer.get(), vertices);
        cmdList->updateBuffer(m_indexBuffer.get(), indices);

        cmdList->setPipeline(m_pipeline.get());
        cmdList->setFramebuffer(m_framebuffer);
        cmdList->setVertexBuffer(0, m_vertexBuffer.get());
        cmdList->setIndexBuffer(m_indexBuffer.get(), IndexFormat::UInt32);

        updateProjection(drawData, cmdList);

        cmdList->setTexture("ImGuiTexture", m_fontTextureView.get());
        cmdList->setSampler("ImGuiTexture", m_fontSampler.get());

        const ImVec2 clipOff   = drawData->DisplayPos;
        const ImVec2 clipScale = drawData->FramebufferScale;

        for(int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList *cmdListImGui = drawData->CmdLists[n];
            const uint32_t baseIndex = listIndexOffsets[n];

            for(int cmd_i = 0; cmd_i < cmdListImGui->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd &pcmd = cmdListImGui->CmdBuffer[cmd_i];

                if(pcmd.UserCallback)
                {
                    pcmd.UserCallback(cmdListImGui, &pcmd);
                    continue;
                }

                ImVec4 clipRect;
                clipRect.x = (pcmd.ClipRect.x - clipOff.x) * clipScale.x;
                clipRect.y = (pcmd.ClipRect.y - clipOff.y) * clipScale.y;
                clipRect.z = (pcmd.ClipRect.z - clipOff.x) * clipScale.x;
                clipRect.w = (pcmd.ClipRect.w - clipOff.y) * clipScale.y;

                if(clipRect.x >= clipRect.z || clipRect.y >= clipRect.w)
                    continue;

                ScissorRect scissor{};
                scissor.x      = static_cast<int>(clipRect.x);
                scissor.y      = static_cast<int>(clipRect.y);
                scissor.width  = static_cast<int>(clipRect.z - clipRect.x);
                scissor.height = static_cast<int>(clipRect.w - clipRect.y);

                cmdList->setScissor(scissor);

                cmdList->drawIndexed(pcmd.ElemCount, 1, baseIndex + pcmd.IdxOffset);
            }
        }
    }

    void ImGuiBackend::setFramebuffer(Framebuffer *newFramebuffer)
    {
        m_framebuffer = newFramebuffer;
    }

    void ImGuiBackend::createFont()
    {
        const ImGuiIO &io = ImGui::GetIO();
        ImFontAtlas *atlas = io.Fonts;

        unsigned char *pixels = nullptr;
        int width = 0;
        int height = 0;
        atlas->GetTexDataAsRGBA32(&pixels, &width, &height);

        TextureDescription texDesc{};
        texDesc.dimensions.x = width;
        texDesc.dimensions.y = height;
        texDesc.numMipmaps = 1;
        texDesc.format = PixelFormat::R8G8B8A8Unorm;
        texDesc.usage = TextureUsage::Sampler;

        m_fontTexture = Box<Texture>(m_device->createTexture(texDesc));
        TextureUploadDescription uploadDesc{};

        uploadDesc.data = pixels;
        uploadDesc.size.x = width;
        uploadDesc.size.y = height;
        uploadDesc.pixelLayout = PixelLayout::RGBA;
        uploadDesc.pixelType = PixelType::UnsignedByte;

        // Upload pixels to the texture with a command list or staging buffer
        CommandList* cmdList = m_device->createCommandList();

        cmdList->begin();
        cmdList->updateTexture(m_fontTexture.get(), uploadDesc);
        m_device->submit(cmdList);

        TextureViewDescription viewDesc;
        viewDesc.target = m_fontTexture.get();

        m_fontTextureView = Box<TextureView>(m_device->createTextureView(viewDesc));

        SamplerDescription samplerDesc{};
        samplerDesc.minFilter = TextureFilter::Linear;
        samplerDesc.magFilter = TextureFilter::Linear;
        samplerDesc.mipmapFilter = MipmapFilter::Linear;
        samplerDesc.wrapMode.x = TextureWrap::ClampToEdge;
        samplerDesc.wrapMode.y = TextureWrap::ClampToEdge;
        samplerDesc.wrapMode.z = TextureWrap::ClampToEdge;

        m_fontSampler = Box<Sampler>(m_device->createSampler(samplerDesc));

        io.Fonts->TexID = m_fontTextureView.get();
    }

    void ImGuiBackend::createPipeline()
    {
        const auto shader = m_device->createShaderFromSource(vertexShaderSource);

        shader->compile();

        InputLayout vertexInputState{};
        vertexInputState.addVertexBuffer<ImDrawVert>(0);
        vertexInputState.addVertexAttribute<glm::vec2>(0, 0);
        vertexInputState.addVertexAttribute<glm::vec2>(0, 1);
        vertexInputState.addVertexAttribute<uint32_t>(0, 2);

        DepthState depthState{};
        depthState.hasDepthTarget  = false; // or true if your framebuffer has depth, but:
        depthState.enableDepthTest = false;

        const RenderTargetsDescription targetsDesc{};

        RasterizerState rasterState{};
        rasterState.cullMode = CullMode::None;
        rasterState.enableScissorTest = true;

        BlendState blendState{};
        blendState.enableBlend = true;

        GraphicsPipelineDescription pipelineDescription{};
        pipelineDescription.depthState         = depthState;
        pipelineDescription.shader             = shader;
        pipelineDescription.inputLayout		   = vertexInputState;
        pipelineDescription.targetsDescription = targetsDesc;
        pipelineDescription.rasterizerState    = rasterState;
        pipelineDescription.blendState         = blendState;

        m_pipeline = Box<Pipeline>(m_device->createPipeline(pipelineDescription));
    }

    void ImGuiBackend::updateProjection(const ImDrawData *drawData, CommandList* cmdList) const
    {
        const ImVec2 displayPos    = drawData->DisplayPos;
        const ImVec2 displaySize   = drawData->DisplaySize;
        // ImVec2 framebufferScale = drawData->FramebufferScale;

        const float L = displayPos.x;
        const float R = displayPos.x + displaySize.x;
        const float T = displayPos.y;
        const float B = displayPos.y + displaySize.y;

        // Note: bottom = B, top = T → inverts Y so that ImGui's top-left coords work.
        glm::mat4 projMatrix = glm::ortho(L, R, B, T, -1.0f, 1.0f);

        cmdList->updateBuffer(m_projUniformBuffer.get(), projMatrix);
        cmdList->setUniformBuffer("ImGuiProjection", m_projUniformBuffer.get());
    }

}
