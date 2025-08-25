#include "deviceOGL.h"
#include <glad/glad.h>

#include "bufferOGL.h"
#include "commandListOGL.h"
#include "frameBufferOGL.h"
#include "graphicsPipelineOGL.h"
#include "samplerOGL.h"
#include "shaderOGL.h"
#include "textureOGL.h"
#include "windowOGL.h"

namespace NRHI
{
    DeviceOGL::DeviceOGL(WindowOGL *window) : window(window)
    {

    }

    GraphicsPipeline* DeviceOGL::createGraphicsPipeline(GraphicsPipelineDescription graphicsPipelineDescription)
    {
        return new GraphicsPipelineOGL(graphicsPipelineDescription);
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

    Texture* DeviceOGL::createTexture(TextureDescription textureDescription)
    {
        return new TextureOGL(textureDescription);
    }

    Sampler* DeviceOGL::createSampler(SamplerDescription samplerDescription)
    {
        return new SamplerOGL(samplerDescription);
    }

    void DeviceOGL::submit(CommandList* commandList)
    {
        dynamic_cast<CommandListOGL*>(commandList)->executeCommands();
    }

    void DeviceOGL::swapBuffers()
    {
        window->swapBuffers();
    }

    FrameBuffer* DeviceOGL::getSwapChainFrameBuffer()
    {
        return new FrameBufferOGL(0);
    }


    Shader* DeviceOGL::createShaderFromSpirvImpl(std::unordered_map<ShaderType, std::vector<uint32_t>> shadersSpirv)
    {
        return new ShaderOGL(shadersSpirv);
    }
}
