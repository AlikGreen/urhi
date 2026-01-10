#include "deviceOGL.h"
#include <glad/gl.h>

#include "bufferOGL.h"
#include "commandListOGL.h"
#include "frameBufferOGL.h"
#include "pipelineOGL.h"
#include "samplerOGL.h"
#include "shaderOGL.h"
#include "swapchainOGL.h"
#include "textureOGL.h"
#include "textureViewOGL.h"

namespace Neon::RHI
{
    Rc<Pipeline> DeviceOGL::createPipeline(const GraphicsPipelineDescription& description)
    {
        return makeRc<PipelineOGL>(description);
    }

    Rc<Pipeline> DeviceOGL::createPipeline(const ComputePipelineDescription& description)
    {
        return makeRc<PipelineOGL>(description);
    }

    Rc<CommandList> DeviceOGL::createCommandList()
    {
        return makeRc<CommandListOGL>();
    }

    Rc<Swapchain> DeviceOGL::createSwapchain(const SwapchainDescription& description)
    {
        return makeRc<SwapchainOGL>(description, this);
    }

    Rc<Buffer> DeviceOGL::createIndexBuffer()
    {
        return makeRc<BufferOGL>(GL_ELEMENT_ARRAY_BUFFER);
    }

    Rc<Buffer> DeviceOGL::createUniformBuffer()
    {
        return makeRc<BufferOGL>(GL_UNIFORM_BUFFER);
    }

    Rc<Buffer> DeviceOGL::createVertexBuffer()
    {
        return makeRc<BufferOGL>(GL_ARRAY_BUFFER);
    }

    Rc<Texture> DeviceOGL::createTexture(const TextureDescription& description)
    {
        return makeRc<TextureOGL>(description);
    }

    Rc<Sampler> DeviceOGL::createSampler(const SamplerDescription& description)
    {
        return makeRc<SamplerOGL>(description);
    }

    Rc<TextureView> DeviceOGL::createTextureView(const TextureViewDescription& description)
    {
        return makeRc<TextureViewOGL>(description);
    }

    Rc<Framebuffer> DeviceOGL::createFramebuffer(const FramebufferDescription& description)
    {
        return makeRc<FramebufferOGL>(description);
    }

    Rc<Shader> DeviceOGL::createShaderFromSpirv(std::vector<uint32_t> spirv)
    {
        return makeRc<ShaderOGL>(spirv);
    }

    void DeviceOGL::submit(const Rc<CommandList>& commandList)
    {
        dynamic_cast<CommandListOGL*>(commandList.get())->executeCommands();
    }
}