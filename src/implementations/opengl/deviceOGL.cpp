#include "deviceOGL.h"
#include <glad/glad.h>

#include "bufferOGL.h"
#include "commandListOGL.h"
#include "frameBufferOGL.h"
#include "pipelineOGL.h"
#include "samplerOGL.h"
#include "shaderOGL.h"
#include "textureOGL.h"
#include "textureViewOGL.h"
#include "windowOGL.h"

namespace Neon::RHI
{
    DeviceOGL::DeviceOGL(WindowOGL *window) : window(window)
    {

    }

    Pipeline* DeviceOGL::createPipeline(const GraphicsPipelineDescription& description)
    {
        return new PipelineOGL(description);
    }

    Pipeline * DeviceOGL::createPipeline(const ComputePipelineDescription &description)
    {
        return new PipelineOGL(description);
    }

    CommandList* DeviceOGL::createCommandList()
    {
        return new CommandListOGL();
    }

    Buffer* DeviceOGL::createIndexBuffer()
    {
        return new BufferOGL(GL_ELEMENT_ARRAY_BUFFER);
    }

    Buffer* DeviceOGL::createUniformBuffer()
    {
        return new BufferOGL(GL_UNIFORM_BUFFER);
    }

    Buffer* DeviceOGL::createVertexBuffer()
    {
        return new BufferOGL(GL_ARRAY_BUFFER);
    }

    Texture* DeviceOGL::createTexture(const TextureDescription& description)
    {
        return new TextureOGL(description);
    }

    Sampler* DeviceOGL::createSampler(const SamplerDescription& description)
    {
        return new SamplerOGL(description);
    }

    TextureView * DeviceOGL::createTextureView(const TextureViewDescription &description)
    {
        return new TextureViewOGL(description);
    }

    void DeviceOGL::submit(CommandList* commandList)
    {
        dynamic_cast<CommandListOGL*>(commandList)->executeCommands();
    }

    void DeviceOGL::swapBuffers()
    {
        window->swapBuffers();
    }

    Framebuffer* DeviceOGL::getSwapChainFramebuffer()
    {
        return new FramebufferOGL(0);
    }


    Shader* DeviceOGL::createShaderFromSpirvImpl(std::unordered_map<ShaderType, std::vector<uint32_t>> shadersSpirv)
    {
        return new ShaderOGL(shadersSpirv);
    }
}
