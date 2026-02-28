#include "clogr.h"
#include "context.h"
#include "shaderCompiler.h"
#include "window.h"
#include "timers/scopeTimer.h"

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

    const auto computeSource = R"(
        struct PushConstants
        {
            uint width;
            uint height;
        }

        [[vk::push_constant]]
        PushConstants pc;

        RWTexture2D<float4> outputImage;

        [shader("compute")]
        [numthreads(8, 8, 1)]
        void computeMain(uint3 id : SV_DispatchThreadID)
        {
            if (id.x >= pc.width || id.y >= pc.height)
                return;

            float cx = (float)pc.width  * 0.5;
            float cy = (float)pc.height * 0.5;
            float maxDist = sqrt(cx * cx + cy * cy);

            float dx = (float)id.x - cx;
            float dy = (float)id.y - cy;
            float t  = sqrt(dx * dx + dy * dy) / maxDist;

            outputImage[id.xy] = float4(1.0 - t, t * 0.5, t, 1.0);
        }
    )";

    const auto computeEntryPoints = ShaderCompiler::compile({ .source = computeSource });

    Rc<Shader> computeShader{};
    for (const auto& ep : computeEntryPoints)
        if (ep.stage == ShaderStage::Compute) computeShader = device->createShader(ep);

    ComputePipelineDesc computeDesc{};
    computeDesc.shader = computeShader;
    const auto computePipeline = device->createPipeline(computeDesc);


    const auto shaderSource = R"(
        struct VS_INPUT
        {
            float3 position : POSITION;
            float3 normal   : NORMAL;
            float2 texCoord : TEXCOORD0;
        };

        struct VS_OUTPUT
        {
            float4 position : SV_POSITION;
            float2 texCoord : TEXCOORD0;
            float3 normal   : NORMAL;
        };

        struct UniformData
        {
            float4x4 modelMat;
            float4   tint;
        }

        ConstantBuffer<UniformData> data;

        [shader("vertex")]
        VS_OUTPUT vertexMain(VS_INPUT input)
        {
            VS_OUTPUT output;
            output.position = mul(data.modelMat, float4(input.position, 1.0));
            output.texCoord = input.texCoord;
            output.normal   = input.normal;
            return output;
        }

        Texture2D    diffuseTexture;
        SamplerState samplerState;

        struct PushConstants { float green; }
        [[vk::push_constant]]
        PushConstants pc;

        [shader("fragment")]
        float4 fragmentMain(VS_OUTPUT input) : SV_TARGET
        {
            float4 tex = diffuseTexture.Sample(samplerState, input.texCoord);
            return float4(tex.r * data.tint.r, pc.green, tex.b * data.tint.b, 1.0);
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
    desc.shaders        = { vertexShader, fragmentShader };
    desc.primitiveType  = PrimitiveType::TriangleList;
    desc.rasterizerState = { .cullMode = CullMode::None };
    desc.depthState     = { .enableDepthTest = false };
    desc.colorAttachments = {
        ColorAttachmentDesc{
            .format = PixelFormat::R8G8B8A8Unorm,
            .blend  = BlendState::opaque()
        }
    };

    const auto pipeline = device->createPipeline(desc);

    struct Vertex { glm::vec3 pos, normal; glm::vec2 texCoord; };

    const std::vector<Vertex> vertices = {
        {{-1,-1,0},{0,0,1},{0,0}},
        {{ 1,-1,0},{0,0,1},{1,0}},
        {{ 1, 1,0},{0,0,1},{1,1}},
        {{-1, 1,0},{0,0,1},{0,1}},
    };
    const std::vector<uint32_t> indices = {0,1,2, 0,2,3};

    struct UniformData { glm::mat4 mvp; glm::vec4 tint; };

    const auto vertexBuffer  = device->createBuffer({BufferUsage::Vertex,  vertices.size() * sizeof(Vertex)});
    const auto indexBuffer   = device->createBuffer({BufferUsage::Index,   indices.size()  * sizeof(uint32_t)});
    auto       uniformBuffer = device->createBuffer({BufferUsage::Uniform, sizeof(UniformData)});

    constexpr uint32_t kTextureSize = 512;

    auto textureDesc = TextureDesc::Texture2D(
        kTextureSize, kTextureSize,
        PixelFormat::R8G8B8A8Unorm,
        TextureUsage::Sampled | TextureUsage::Storage,
        ~0u
    );
    auto texture     = device->createTexture(textureDesc);
    auto textureView = device->createTextureView(texture);

    auto sampler = device->createSampler({
        .minFilter = TextureFilter::Linear,
        .magFilter = TextureFilter::Linear,
    });

    {
        const auto cmd = device->acquireCommandList(QueueType::Graphics);
        cmd->begin();

        cmd->updateBuffer(vertexBuffer, vertices);
        cmd->updateBuffer(indexBuffer,  indices);

        UniformData ubo = {glm::mat4(1.0f), glm::vec4(0.5f)};
        cmd->updateBuffer(uniformBuffer, ubo);

        struct ComputePush { uint32_t width, height; };

        auto computePass = cmd->beginComputePass();
        computePass->setPipeline(computePipeline);
        computePass->setImage("outputImage", textureView, ResourceAccess::WriteOnly);

        ComputePush cp{ kTextureSize, kTextureSize };
        computePass->pushConstants(&cp, sizeof(cp));

        computePass->dispatch(
            (kTextureSize + 7) / 8,
            (kTextureSize + 7) / 8,
            1
        );

        computePass->end();

        cmd->generateMipmaps(texture);

        device->submit(cmd);
    }

    struct PushConstants { float green; };

    bool  running = true;
    float mouseX  = 0.0f;

    while (running)
    {
        auto events = window->pollEvents();
        for (auto& event : events)
        {
            if (event.type == Event::Type::Quit)
                running = false;
            if (event.type == Event::Type::WindowResize)
                swapchain->resize(window->width(), window->height());
            if (event.type == Event::Type::MouseMotion)
                mouseX = event.as<Event::MouseMotionEvent>().x;
        }

        auto cmd        = device->acquireCommandList(QueueType::Graphics);
        auto backBuffer = swapchain->acquireNextImage();

        cmd->begin();

        RenderPassDesc renderPassDesc;
        renderPassDesc.colorAttachments.push_back({
            backBuffer, LoadOp::Clear, StoreOp::Store,
            ClearColorFloat{0.39f, 0.58f, 0.93f, 1.0f}
        });

        auto renderPass = cmd->beginRenderPass(renderPassDesc);

        renderPass->setPipeline(pipeline);
        renderPass->setUniformBuffer("data",           uniformBuffer);
        renderPass->setTexture("diffuseTexture",       textureView);
        renderPass->setSampler("samplerState",         sampler);

        PushConstants pc{ mouseX / static_cast<float>(window->width()) };
        renderPass->pushConstants(pc);

        renderPass->setVertexBuffer(0, vertexBuffer);
        renderPass->setIndexBuffer(indexBuffer, IndexFormat::UInt32);
        renderPass->drawIndexed(indices.size());

        renderPass->end();
        device->submit(cmd);

        swapchain->present();
    }

    device->waitIdle();
}