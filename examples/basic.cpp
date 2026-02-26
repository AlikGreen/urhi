#include "clogr.h"
#include "context.h"
#include "shaderCompiler.h"
#include "window.h"
#include "timers/scopeTimer.h"

std::vector<uint8_t> generateRadialGradientRgba8(uint32_t width, uint32_t height)
{
    std::vector<uint8_t> data;
    data.resize(static_cast<size_t>(width) * height * 4);

    float cx = (float)width * 0.5f;
    float cy = (float)height * 0.5f;
    float maxDist = std::sqrt(cx * cx + cy * cy);

    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            float dx = (float)x - cx;
            float dy = (float)y - cy;
            float d = std::sqrt(dx * dx + dy * dy);
            float t = d / maxDist; // 0..1

            uint8_t r = static_cast<uint8_t>((1.0f - t) * 255.0f);
            uint8_t g = static_cast<uint8_t>((t) * 128.0f);
            uint8_t b = static_cast<uint8_t>((t) * 255.0f);

            size_t idx = (static_cast<size_t>(y) * width + x) * 4;
            data[idx + 0] = r;
            data[idx + 1] = g;
            data[idx + 2] = b;
            data[idx + 3] = 0xFF;
        }
    }

    return data;
}

int main()
{
    using namespace urhi;
    using namespace grl;

    const auto context = Context::create(BackendAPI::Vulkan);

    const auto window = context->createWindow({
        .title = "Example",
        .width = 800,
        .height = 600,
    });

    const auto device = context->createDevice({
        .window = window
    });

    auto swapchain = context->createSwapchain({
        .window = window,
        .device = device,
        .presentMode = PresentMode::NoVSync,
    });

     const auto shaderSource = R"(
         // Vertex shader
         struct VS_INPUT
         {
             float3 position : POSITION;
             float3 normal : NORMAL;
             float2 texCoord : TEXCOORD0;
         };

         struct VS_OUTPUT
         {
             float4 position : SV_POSITION;
             float2 texCoord : TEXCOORD0;
             float3 normal : NORMAL;
         };

        cbuffer UniformData
        {
            float4x4 modelMat;
            float4 tint;
        }

         [shader("vertex")]
         VS_OUTPUT vertexMain(VS_INPUT input)
         {
             VS_OUTPUT output;
             output.position = mul(modelMat, float4(input.position, 1.0));
             output.texCoord = input.texCoord;
             output.normal = input.normal;
             return output;
         }

         // Fragment (pixel) shader
         Texture2D diffuseTexture;
         SamplerState samplerState;

         [shader("fragment")]
         float4 fragmentMain(VS_OUTPUT input) : SV_TARGET
         {
             float4 tex = diffuseTexture.Sample(samplerState, input.texCoord);
             return float4(tex.rgb*tint.rgb, 1.0);
         }
     )";

     const auto entryPoints = ShaderCompiler::compile({
         .source = shaderSource
     });

     Rc<Shader> vertexShader{};
     Rc<Shader> fragmentShader{};

    for(const auto& ep : entryPoints)
    {
        if(ep.stage == ShaderStage::Vertex)   vertexShader = device->createShader(ep);
        if(ep.stage == ShaderStage::Fragment) fragmentShader = device->createShader(ep);
    }

    GraphicsPipelineDesc desc{};
    desc.vertexShader = vertexShader;
    desc.vertexShader = vertexShader,
    desc.fragmentShader = fragmentShader,
    desc.primitiveType = PrimitiveType::TriangleList,
    desc.rasterizerState = { .cullMode = CullMode::None },
    desc.depthState = { .enableDepthTest = false },
    desc.colorAttachments = {
        ColorAttachmentDesc
        {
            .format = PixelFormat::R8G8B8A8Unorm,
            .blend = BlendState::opaque()
        }
    };

    const auto pipeline = device->createPipeline(desc);

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    const std::vector<Vertex> vertices =
        {
        {{-1, -1, 0}, {0, 0, 1}, {0, 0}},
        {{ 1, -1, 0}, {0, 0, 1}, {1, 0}},
        {{ 1,  1, 0}, {0, 0, 1}, {1, 1}},
        {{-1,  1, 0}, {0, 0, 1}, {0, 1}},
    };

    const std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    struct UniformData
    {
        glm::mat4 mvp;
        glm::vec4 tint;
    };

    const auto vertexBuffer  = device->createBuffer({BufferUsage::Vertex, vertices.size() * sizeof(Vertex)});
    const auto indexBuffer   = device->createBuffer({BufferUsage::Index, indices.size() * sizeof(uint32_t)});
    auto uniformBuffer = device->createBuffer({BufferUsage::Uniform, sizeof(UniformData)});

    constexpr uint32_t kTextureSize = 512;

    auto textureDesc = TextureDesc::Texture2D(kTextureSize, kTextureSize, PixelFormat::R8G8B8A8Unorm, TextureUsage::Sampled, ~0u);
    auto texture = device->createTexture(textureDesc);
    auto textureView = device->createTextureView(texture);

    auto sampler = device->createSampler({
        .minFilter = TextureFilter::Linear,
        .magFilter = TextureFilter::Linear,
    });

    auto textureData = generateRadialGradientRgba8(kTextureSize, kTextureSize);

    {
        const auto cmd = device->acquireCommandList(QueueType::Graphics); // TODO add cross queue sync
        cmd->begin();

        cmd->updateBuffer(vertexBuffer, vertices);
        cmd->updateBuffer(indexBuffer, indices);

        UniformData ubo = {glm::mat4(1.0f), glm::vec4(0.5f)};
        cmd->updateBuffer(uniformBuffer, ubo);

        cmd->updateTexture({.texture = texture, .data = textureData.data(), .width = kTextureSize, .height = kTextureSize});
        cmd->generateMipmaps(texture);

        device->submit(cmd);
    }

    // Readback test
    {
        const auto cmd = device->acquireCommandList(QueueType::Graphics);
        cmd->begin();

        TextureReadbackDesc readbackDesc{};
        readbackDesc.texture = texture;
        readbackDesc.width = 1;
        readbackDesc.height = 1;
        readbackDesc.x = 256;
        readbackDesc.y = 256;
        auto readbackRes = cmd->readback(readbackDesc);

        device->submit(cmd);

        readbackRes->wait();
        uint8_t r = readbackRes->at<uint8_t>(0);
        uint8_t g = readbackRes->at<uint8_t>(1);
        uint8_t b = readbackRes->at<uint8_t>(2);

        clogr::info("Color: {}, {}, {}", r, g, b);
    }


    bool running = true;

    while (running)
    {
        auto events = window->pollEvents();

        for(const auto& event : events)
        {
            if(event.type == Event::Type::Quit)
            {
                running = false;
            }
            if(event.type == Event::Type::WindowResize)
            {
                swapchain->resize(window->getWidth(), window->getHeight());
            }
        }

        auto cmd = device->acquireCommandList(QueueType::Graphics);
        auto backBuffer = swapchain->acquireNextImage();

        cmd->begin();

        RenderPassDesc renderPassDesc;
        renderPassDesc.colorAttachments.push_back({
            backBuffer,
            LoadOp::Clear,
            StoreOp::Store,
            ClearColorFloat{0.39f, 0.58f, 0.93f, 1.0f}
        });

        auto renderPass = cmd->beginRenderPass(renderPassDesc);

        renderPass->setPipeline(pipeline);

        renderPass->setUniformBuffer("UniformData", uniformBuffer);
        renderPass->setTexture("diffuseTexture", textureView);
        renderPass->setSampler("samplerState", sampler);

        renderPass->setVertexBuffer(0, vertexBuffer);
        renderPass->setIndexBuffer(indexBuffer, IndexFormat::UInt32);
        renderPass->drawIndexed(indices.size());

        renderPass->end();
        device->submit(cmd);

        swapchain->present();
    }

    device->waitIdle();
}
