#include "clogr.h"
#include "context.h"
#include "shaderCompiler.h"
#include "window.h"

#include "imgui/imGuiController.h"

int main()
{
    using namespace urhi;
    using namespace grl;

    const auto context = Context::create(BackendAPI::Vulkan);

    const auto window = context->createWindow({
        .title = "ImGui Example",
        .width = 1280,
        .height = 720,
    });

    const auto device = context->createDevice({
        .window = window
    });

    auto swapchain = context->createSwapchain({
        .window  = window,
        .device  = device,
        .presentMode = PresentMode::NoVSync,
    });

    ImGui::CreateContext();

    ImGuiController imGui({
        .device = device,
        .window = window,
    });

    const auto shaderSource = R"(
        struct VS_INPUT
        {
            float3 position : POSITION;
            float2 texCoord : TEXCOORD0;
        };

        struct VS_OUTPUT
        {
            float4 position : SV_POSITION;
            float2 texCoord : TEXCOORD0;
        };

        [shader("vertex")]
        VS_OUTPUT vertexMain(VS_INPUT input)
        {
            VS_OUTPUT output;
            output.position = float4(input.position, 1.0);
            output.texCoord = input.texCoord;
            return output;
        }

        Texture2D    sceneTexture;
        SamplerState sceneSampler;

        [shader("fragment")]
        float4 fragmentMain(VS_OUTPUT input) : SV_TARGET
        {
            return sceneTexture.Sample(sceneSampler, input.texCoord);
        }
    )";

    const auto entryPoints = ShaderCompiler::compile({ .source = shaderSource });

    Rc<Shader> vertexShader{}, fragmentShader{};
    for (const auto& ep : entryPoints)
    {
        if (ep.stage == ShaderStage::Vertex)   vertexShader   = device->createShader(ep);
        if (ep.stage == ShaderStage::Fragment) fragmentShader = device->createShader(ep);
    }

    GraphicsPipelineDesc desc{};
    desc.shaders         = { vertexShader, fragmentShader };
    desc.primitiveType   = PrimitiveType::TriangleList;
    desc.rasterizerState = { .cullMode = CullMode::None };
    desc.depthState      = { .enableDepthTest = false };
    desc.colorAttachments = {
        ColorAttachmentDesc{
            .format = PixelFormat::R8G8B8A8Unorm,
            .blend  = BlendState::opaque()
        }
    };

    const auto pipeline = device->createPipeline(desc);

    struct Vertex { glm::vec3 pos; glm::vec2 texCoord; };

    const std::vector<Vertex> vertices = {
        {{-1,-1,0},{0,0}},
        {{ 1,-1,0},{1,0}},
        {{ 1, 1,0},{1,1}},
        {{-1, 1,0},{0,1}},
    };

    const std::vector<uint32_t> indices = {0,1,2, 0,2,3};

    const auto vertexBuffer = device->createBuffer({BufferUsage::Vertex, vertices.size() * sizeof(Vertex)});
    const auto indexBuffer  = device->createBuffer({BufferUsage::Index,  indices.size()  * sizeof(uint32_t)});

    auto sampler = device->createSampler({
        .minFilter = TextureFilter::Linear,
        .magFilter = TextureFilter::Linear,
    });

    {
        const auto cmd = device->acquireCommandList(QueueType::Graphics);
        cmd->begin();
        cmd->updateBuffer(vertexBuffer, vertices);
        cmd->updateBuffer(indexBuffer,  indices);
        device->submit(cmd);
    }

    bool  running    = true;
    bool  showDemo   = true;
    float clearColor[3] = { 0.39f, 0.58f, 0.93f };

    while (running)
    {
        auto events = window->pollEvents();
        for (auto& event : events)
        {
            imGui.processEvent(event);

            if (event.type == Event::Type::Quit)
                running = false;
            if (event.type == Event::Type::WindowResize)
            {
                swapchain->resize(window->width(), window->height());
            }
        }

        imGui.newFrame();

        ImGui::Begin("Controls");
        ImGui::Checkbox("Show Demo Window", &showDemo);
        ImGui::ColorEdit3("Clear Color", clearColor);
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        ImGui::End();

        if (showDemo)
            ImGui::ShowDemoWindow(&showDemo);

        imGui.endFrame();

        auto cmd        = device->acquireCommandList(QueueType::Graphics);
        auto backBuffer = swapchain->acquireNextImage();

        cmd->begin();

        RenderPassDesc renderPassDesc{};
        renderPassDesc.colorAttachments.push_back({
            backBuffer, LoadOp::Clear, StoreOp::Store,
            ClearColorFloat{ clearColor[0], clearColor[1], clearColor[2], 1.0f }
        });

        auto renderPass = cmd->beginRenderPass(renderPassDesc);

        renderPass->setPipeline(pipeline);
        renderPass->setTexture("sceneTexture", imGui.getFramebufferTextureView());
        renderPass->setSampler("sceneSampler", sampler);
        renderPass->setVertexBuffer(0, vertexBuffer);
        renderPass->setIndexBuffer(indexBuffer, IndexFormat::UInt32);
        renderPass->drawIndexed(indices.size());

        renderPass->end();
        device->submit(cmd);

        swapchain->present();
    }

    ImGui::DestroyContext();
}
