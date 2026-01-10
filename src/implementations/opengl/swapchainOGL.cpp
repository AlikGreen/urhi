#include "swapchainOGL.h"

#include <fstream>

#include "blitShader.h"
#include "debug.h"
#include "deviceOGL.h"
#include "framebufferOGL.h"
#include "shaderCompiler.h"
#include "window.h"
#include "glad/gl.h"

namespace Neon::RHI
{
    SwapchainOGL::SwapchainOGL(const SwapchainDescription &desc, DeviceOGL* device)
    {
        std::vector<glm::vec2> quadPositions =
        {
            {-1, -1 },
            { 1, -1 },
            { 1,  1 },
            {-1,  1 }
        };

        std::vector<uint32_t> quadIndices =
        {
            0, 1, 2,
            0, 2, 3
        };

        this->device = device;
        window = desc.window;
        width = window->getWidth();
        height = window->getHeight();

        ShaderCompileDescription compileDesc{};
        compileDesc.path = "blitShader.slang";
        compileDesc.source = blitShaderSource;

        std::vector<uint32_t> spriv = ShaderCompiler::compile(compileDesc);
        Rc<Shader> shader = device->createShaderFromSpirv(spriv);
        shader->compile();

        InputLayout vertexInputState{};
        vertexInputState.addVertexBuffer<glm::vec2>(0);
        vertexInputState.addVertexAttribute<glm::vec2>(0, 0);

        DepthState depthState{};
        depthState.hasDepthTarget  = false;
        depthState.enableDepthTest = false;

        RasterizerState rasterizerState{};
        rasterizerState.cullMode = CullMode::Back;

        const RenderTargetsDescription targetsDesc{};

        BlendState blendState{};
        blendState.enableBlend = false;

        GraphicsPipelineDescription pipelineDescription{};
        pipelineDescription.shader             = shader;
        pipelineDescription.inputLayout		   = vertexInputState;
        pipelineDescription.targetsDescription = targetsDesc;
        pipelineDescription.depthState         = depthState;
        pipelineDescription.rasterizerState    = rasterizerState;
        pipelineDescription.blendState         = blendState;

        pipeline = device->createPipeline(pipelineDescription);

        resize(width, height);

        vertexBuffer = device->createVertexBuffer();
        indexBuffer = device->createIndexBuffer();

        Rc<CommandList> cl = device->createCommandList();

        cl->begin();

        cl->reserveBuffer(vertexBuffer, quadPositions.size() * sizeof(glm::vec2));
        cl->updateBuffer(vertexBuffer, quadPositions);

        cl->reserveBuffer(indexBuffer, quadIndices.size() * sizeof(uint32_t));
        cl->updateBuffer(indexBuffer, quadIndices);

        device->submit(cl);
    }

    uint32_t SwapchainOGL::acquireNextImage()
    {
        return 0;
    }

    void SwapchainOGL::present(const uint32_t imageIndex)
    {
        Debug::ensure(imageIndex < textures.size(), "SwapchainOGL::present(): image index out of range");
        const Rc<TextureView>& texture = textureViews[imageIndex];
        const Rc<Sampler>& sampler = samplers[imageIndex];

        const Rc<CommandList> commandList = device->createCommandList();

        commandList->begin();

        commandList->setPipeline(pipeline);
        commandList->setFramebuffer(makeRc<FramebufferOGL>(window));

        commandList->setIndexBuffer(indexBuffer, IndexFormat::UInt32);
        commandList->setVertexBuffer(0, vertexBuffer);

        commandList->setTexture("blitTexture", texture);
        commandList->setSampler("blitTexture", sampler);

        commandList->clearColorTarget(0, {0, 0, 0, 1});

        commandList->drawIndexed(6);

        device->submit(commandList);

        window->swapBuffers();

        // Maybe fence things
    }

    const std::vector<Rc<Texture>> & SwapchainOGL::getTextures() const
    {
        return textures;
    }

    void SwapchainOGL::resize(const uint32_t width, const uint32_t height)
    {
        this->width = width;
        this->height = height;

        const TextureDescription colDesc = TextureDescription::Texture2D(
                window->getWidth(),
                window->getHeight(),
                PixelFormat::R8G8B8A8Unorm,
                TextureUsage::ColorTarget);

        const Rc<Texture> colTex = device->createTexture(colDesc);
        textures.clear();
        textures.push_back(colTex);

        textureViews.clear();
        const TextureViewDescription viewDesc(colTex);
        textureViews.push_back(device->createTextureView(viewDesc));

        samplers.clear();
        SamplerDescription samplerDesc{};
        samplers.push_back(device->createSampler(samplerDesc));
    }
}
