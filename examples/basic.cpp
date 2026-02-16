
#include "context.h"
#include "window.h"

int main()
{
    using namespace urhi;
    using namespace grl;

    auto context = Context::create(BackendAPI::Vulkan);

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
        .device = device
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

         [shader("vertex")]
         VS_OUTPUT vertexMain(VS_INPUT input)
         {
             VS_OUTPUT output;
             output.position = float4(input.position, 1.0);
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
             float3 n = normalize(input.normal);
             float3 lightDir = normalize(float3(0.57735, 0.57735, 0.57735));
             float diff = max(dot(n, lightDir), 0.0);
             float4 tex = diffuseTexture.Sample(samplerState, input.texCoord);
             float3 color = tex.rgb * diff;
             return float4(color, tex.a);
         }
     )";

     const auto spirvShader = ShaderCompiler::compile({
         .source = shaderSource
     });

     const auto shader = device->createShader(spirvShader);

    // auto pipeline = device->createPipeline({
    //     .shader = shader,
    //     .rasterizerState = { .cullMode = CullMode::Back },
    //     .depthState = { .enableDepthTest = true },
    //     .blendState = { .enableBlend = true },
    // });


    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    const std::vector<Vertex> vertices = {
        {{-1, -1, 0}, {0, 0, 1}, {0, 0}},
        {{ 1, -1, 0}, {0, 0, 1}, {1, 0}},
        {{ 1,  1, 0}, {0, 0, 1}, {1, 1}},
        {{-1,  1, 0}, {0, 0, 1}, {0, 1}},
    };

    const std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    const auto vertexBuffer  = device->createBuffer({vertices.size() * sizeof(Vertex), BufferUsage::Vertex});
    auto indexBuffer   = device->createBuffer({indices.size() * sizeof(uint32_t), BufferUsage::Index});
//     auto uniformBuffer = device->createUniformBuffer();
//
//     auto textureDesc = TextureDesc::Texture2D(512, 512, PixelFormat::R8G8B8A8Unorm);
//     auto texture = device->createTexture(textureDesc);
//     auto textureView = device->createTextureView(TextureViewDesc(texture));
//
//     auto sampler = device->createSampler({
//         .minFilter = TextureFilter::Linear,
//         .magFilter = TextureFilter::Linear,
//         .mipmapFilter = MipmapFilter::Linear,
//     });

    //
    // struct UniformData
    // {
    //     glm::mat4 mvp;
    // };

    {
        const auto cmd = device->acquireCommandList(QueueType::Transfer);
        cmd->begin();

        cmd->updateBuffer(vertexBuffer, vertices);
        cmd->updateBuffer(indexBuffer, indices);

        // cmdList->reserveBuffer(uniformBuffer, sizeof(UniformData));
        // UniformData ubo = {glm::mat4(1.0f)};
        // cmdList->updateBuffer(uniformBuffer, ubo);

        // TextureUploadDesc uploadDesc;
        // uploadDesc.width = 512;
        // uploadDesc.height = 512;
        // uploadDesc.pixelLayout = PixelLayout::RGBA;
        // uploadDesc.pixelType = PixelType::UnsignedByte;
        // uploadDesc.data = nullptr; // would point to actual data
        // cmdList->updateTexture(texture, uploadDesc);

        device->submit(cmd);
    }

    // Render loop

    bool running = true;

    while (running)
    {
        window->pollEvents([&running](const Event& event)
        {
            if(event.type == Event::Type::Quit)
                running = false;
        });

        auto cmdList = device->acquireCommandList(QueueType::Graphics);
        const uint32_t imageIndex = swapchain->acquireNextImage();

        cmdList->begin();
        // // Begin render pass - implicit state management
        RenderPassDesc renderPassDesc;
        renderPassDesc.colorAttachments.push_back({
            swapchain->getTextureViews()[imageIndex],
            LoadOp::Clear,
            StoreOp::Store,
            ClearColorFloat{0.39f, 0.58f, 0.93f, 1.0f}
        });

        auto renderPass = cmdList->beginRenderPass(renderPassDesc);
        //
        // renderPass->setPipeline(pipeline);
        // renderPass->setUniformBuffer("transformBuffer", uniformBuffer);
        // renderPass->setTexture("diffuseTexture", textureView);
        // renderPass->setSampler("samplerState", sampler);
        //
        // renderPass->setVertexBuffer(0, vertexBuffer);
        // renderPass->setIndexBuffer(indexBuffer, IndexFormat::UInt32);
        // renderPass->drawIndexed(indices.size());
        //
        renderPass->end();
        device->submit(cmdList);

        swapchain->present(imageIndex);
    }
}
