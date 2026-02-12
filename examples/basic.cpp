
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
//
//     const auto shaderSource = R"(
//         // Vertex shader
//         struct VS_INPUT
//         {
//             float3 position : POSITION;
//             float3 normal : NORMAL;
//             float2 texCoord : TEXCOORD0;
//         };
//
//         struct VS_OUTPUT
//         {
//             float4 position : SV_POSITION;
//             float2 texCoord : TEXCOORD0;
//             float3 normal : NORMAL;
//         };
//
//         [shader("vertex")]
//         VS_OUTPUT vertexMain(VS_INPUT input)
//         {
//             VS_OUTPUT output;
//             output.position = float4(input.position, 1.0);
//             output.texCoord = input.texCoord;
//             output.normal = input.normal;
//             return output;
//         }
//
//         // Fragment (pixel) shader
//         Texture2D diffuseTexture;
//         SamplerState samplerState;
//
//         [shader("fragment")]
//         float4 fragmentMain(VS_OUTPUT input) : SV_TARGET
//         {
//             float3 n = normalize(input.normal);
//             float3 lightDir = normalize(float3(0.57735, 0.57735, 0.57735));
//             float diff = max(dot(n, lightDir), 0.0);
//             float4 tex = diffuseTexture.Sample(samplerState, input.texCoord);
//             float3 color = tex.rgb * diff;
//             return float4(color, tex.a);
//         }
//     )";
//
//     const auto spirvShader = ShaderCompiler::compile({
//         .source = shaderSource
//     });
//
//     const auto shader = device->createShader(spirvShader);
//     shader->compile();
//
//     InputLayout vertexLayout;
//     vertexLayout.addVertexBuffer<glm::vec3>(0);     // position
//     vertexLayout.addVertexAttribute<glm::vec3>(0, 0); // position attr
//     vertexLayout.addVertexAttribute<glm::vec3>(0, 1); // normal attr
//     vertexLayout.addVertexAttribute<glm::vec2>(0, 2); // texCoord attr
//
//     auto pipeline = device->createPipeline({
//         .shader = shader,
//         .inputLayout = vertexLayout,
//         .rasterizerState = { .cullMode = CullMode::Back },
//         .depthState = { .enableDepthTest = true },
//         .blendState = { .enableBlend = true },
//     });
//
//     auto vertexBuffer  = device->createVertexBuffer();
//     auto indexBuffer   = device->createIndexBuffer();
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

    // Prepare vertex data
    // struct Vertex
    // {
    //     glm::vec3 pos;
    //     glm::vec3 normal;
    //     glm::vec2 texCoord;
    // };
    //
    // std::vector<Vertex> vertices = {
    //     {{-1, -1, 0}, {0, 0, 1}, {0, 0}},
    //     {{ 1, -1, 0}, {0, 0, 1}, {1, 0}},
    //     {{ 1,  1, 0}, {0, 0, 1}, {1, 1}},
    //     {{-1,  1, 0}, {0, 0, 1}, {0, 1}},
    // };
    //
    // std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
    //
    // struct UniformData
    // {
    //     glm::mat4 mvp;
    // };

    {
        auto cmdList = device->acquireCommandList(QueueType::Transfer);
        cmdList->begin();

        // cmdList->reserveBuffer(vertexBuffer, vertices.size() * sizeof(Vertex));
        // cmdList->updateBuffer(vertexBuffer, vertices);
        //
        // cmdList->reserveBuffer(indexBuffer, indices.size() * sizeof(uint32_t));
        // cmdList->updateBuffer(indexBuffer, indices);
        //
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

        device->submit(cmdList);
    }

    // Render loop

    bool running = true;

    while (running)
    {
        for(const auto& event : window->pollEvents())
        {
            if(event.type == Event::Type::Quit)
                running = false;
        }

        auto cmdList = device->acquireCommandList(QueueType::Graphics);
        auto imageIndex = swapchain->acquireNextImage();

        cmdList->begin();
        // // Begin render pass - implicit state management
        // RenderPassDesc renderPassDesc;
        // renderPassDesc.colorAttachments.push_back({
        //     swapchain->getTextureViews()[imageIndex],
        //     LoadOp::Clear,
        //     StoreOp::Store,
        //     {0.2f, 0.2f, 0.2f, 1.0f}
        // });
        //
        // cmdList->setPipeline(pipeline);
        // cmdList->beginRenderPass(renderPassDesc);
        //
        // // cmdList->setUniformBuffer("transformBuffer", uniformBuffer);
        // cmdList->setTexture("diffuseTexture", textureView);
        // cmdList->setSampler("samplerState", sampler);
        //
        // cmdList->setVertexBuffer(0, vertexBuffer);
        // cmdList->setIndexBuffer(indexBuffer, IndexFormat::UInt32);
        // cmdList->drawIndexed(indices.size());
        //
        // cmdList->endRenderPass();
        device->submit(cmdList);

        swapchain->present(imageIndex);
    }
}
