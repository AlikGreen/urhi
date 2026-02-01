#include "deviceOGL.h"

#include <iostream>
#include <glad/gl.h>

#include "bufferOGL.h"
#include "commandListOGL.h"
#include "pipelineOGL.h"
#include "samplerOGL.h"
#include "shaderCompiler.h"
#include "shaderOGL.h"
#include "swapchainOGL.h"
#include "textureOGL.h"
#include "textureViewOGL.h"

namespace Neon::RHI
{
    Rc<Pipeline> DeviceOGL::createPipeline(const GraphicsPipelineDesc& desc)
    {
        return makeRc<PipelineOGL>(desc);
    }

    Rc<Pipeline> DeviceOGL::createPipeline(const ComputePipelineDesc& description)
    {
        return makeRc<PipelineOGL>(description);
    }

    Rc<CommandList> DeviceOGL::createCommandList()
    {
        return makeRc<CommandListOGL>(this);
    }

    Rc<Swapchain> DeviceOGL::createSwapchain(const SwapchainDesc& desc)
    {
        return makeRc<SwapchainOGL>(desc, this);
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

    Rc<Texture> DeviceOGL::createTexture(const TextureDesc& desc)
    {
        return makeRc<TextureOGL>(desc);
    }

    Rc<Sampler> DeviceOGL::createSampler(const SamplerDesc& desc)
    {
        return makeRc<SamplerOGL>(desc);
    }

    Rc<TextureView> DeviceOGL::createTextureView(const TextureViewDesc& desc)
    {
        return makeRc<TextureViewOGL>(desc);
    }


    Rc<Shader> DeviceOGL::createShader(CompiledShader shader)
    {
        return makeRc<ShaderOGL>(shader);
    }

    void DeviceOGL::endFrame()
    {
        std::vector<uint32_t> keysToRemove;
        for(auto [key, fb] : m_cachedFbs)
        {
            if(fb.framesSinceUsed > 3)
                keysToRemove.push_back(key);

            fb.framesSinceUsed++;
        }

        for (uint32_t k : keysToRemove)
        {
            glDeleteFramebuffers(1, &m_cachedFbs.at(k).fbo);
            m_cachedFbs.erase(k);
        }
    }

    template <class T>
    void hashCombine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }


    size_t DeviceOGL::hashRenderPass(const RenderPassDesc& desc)
    {
        size_t seed = 0;

        for (const auto& att : desc.colorAttachments)
        {
            const auto* glTexture = dynamic_cast<TextureViewOGL*>(att.texture.get());

            hashCombine(seed, glTexture->getHandle());
            hashCombine(seed, glTexture->getWidth());
            hashCombine(seed, glTexture->getHeight());
            hashCombine(seed, glTexture->getFormat());
        }

        if (desc.depthAttachment.texture != nullptr)
        {
            const auto* glTexture = dynamic_cast<TextureViewOGL*>(desc.depthAttachment.texture.get());

            hashCombine(seed, glTexture->getHandle());
            hashCombine(seed, glTexture->getWidth());
            hashCombine(seed, glTexture->getHeight());
            hashCombine(seed, glTexture->getFormat());
        }

        return seed;
    }

    GLuint DeviceOGL::getOrCreateFb(const RenderPassDesc &desc)
    {
        const uint32_t hash = hashRenderPass(desc);

        if(m_cachedFbs.contains(hash))
        {
            m_cachedFbs[hash].framesSinceUsed = 0;
            return m_cachedFbs.at(hash).fbo;
        }

        GLuint fbo = 0;
        glCreateFramebuffers(1, &fbo);

        for(size_t i = 0; i < desc.colorAttachments.size(); i++)
        {
            const auto* glTexture = dynamic_cast<TextureViewOGL*>(desc.colorAttachments[i].texture.get());
            glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0+i, glTexture->getHandle(), 0);
        }

        if (desc.depthAttachment.texture != nullptr)
        {
            const auto* depthTexture = dynamic_cast<TextureViewOGL*>(desc.depthAttachment.texture.get());
            glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, depthTexture->getHandle(), 0);
        }

        constexpr GLenum drawBuf = GL_COLOR_ATTACHMENT0;
        glNamedFramebufferDrawBuffers(fbo, 1, &drawBuf);

        m_cachedFbs[hash] = {fbo, 0};
        return fbo;
    }

    void DeviceOGL::submit(const Rc<CommandList>& commandList)
    {
        dynamic_cast<CommandListOGL*>(commandList.get())->executeCommands();
    }
}
