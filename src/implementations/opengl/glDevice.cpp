#include "glDevice.h"

#include "glBuffer.h"
#include "validation.h"
#include "glCommandListEmitter.h"
#include "glTextureView.h"

#include "clogr.h"
#include "glContext.h"
#include "glPipeline.h"
#include "glShader.h"
#include "glWindow.h"
#include "enums/queueType.h"
#include "glCommandList.h"
#include "glTexture.h"
#include "glSampler.h"

namespace urhi
{
    GlDevice::GlDevice(const DeviceDesc& desc, GlContext* context)
        : m_context(context)
    {
        m_supportsD24       = isDepthFormatSupported(GL_DEPTH_COMPONENT24);
        m_supportsD32F      = isDepthFormatSupported(GL_DEPTH_COMPONENT32F);
        m_supportsD32FStencil = isDepthFormatSupported(GL_DEPTH32F_STENCIL8);

        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &m_maxAnisotropy);

        glCreateBuffers(1, &m_pushConstantsUbo);
        glNamedBufferStorage(m_pushConstantsUbo, 256, nullptr, GL_DYNAMIC_STORAGE_BIT);
        // glNamedBufferData(m_pushConstantsUbo, 256, nullptr, GL_DYNAMIC_DRAW);

        glCreateFramebuffers(1, &m_blitReadFbo);
        glCreateFramebuffers(1, &m_blitWriteFbo);


        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // callback on same thread as the GL call — easier to get a stack trace

        glDebugMessageCallback([](
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam)
        {
            onDebugMessage(source, type, id, severity, message);
        }, this);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
            0, nullptr, GL_FALSE);
    }

    void GlDevice::onDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, const char* message)
    {
        auto sourceStr = [source]() -> const char* {
            switch (source) {
                case GL_DEBUG_SOURCE_API:             return "API";
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Window System";
                case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
                case GL_DEBUG_SOURCE_THIRD_PARTY:     return "Third Party";
                case GL_DEBUG_SOURCE_APPLICATION:     return "Application";
                default:                              return "Other";
            }
        }();

        auto typeStr = [type]() -> const char* {
            switch (type) {
                case GL_DEBUG_TYPE_ERROR:               return "Error";
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated";
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined Behaviour";
                case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
                case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
                case GL_DEBUG_TYPE_MARKER:              return "Marker";
                default:                                return "Other";
            }
        }();

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:
                clogr::error("[GL][{}][{}] {}", sourceStr, typeStr, message);
            URHI_VALIDATE(false, "High severity GL error");
            break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                clogr::warn("[GL][{}][{}] {}", sourceStr, typeStr, message);
            break;
            case GL_DEBUG_SEVERITY_LOW:
                clogr::info("[GL][{}][{}] {}", sourceStr, typeStr, message);
            break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                break;
            default:
                break;
        }
    }

    bool GlDevice::isDepthFormatSupported(const GLenum internalFormat)
    {
        // Available in GL 4.2+ and ES 3.0+
        GLint supported = GL_NONE;
        glGetInternalformativ(GL_TEXTURE_2D, internalFormat,
            GL_INTERNALFORMAT_SUPPORTED, 1, &supported);
        return supported == GL_TRUE;
    }

    GlDevice::~GlDevice()
    {
        glDeleteBuffers(1, &m_pushConstantsUbo);

        glDeleteFramebuffers(1, &m_blitReadFbo);
        glDeleteFramebuffers(1, &m_blitWriteFbo);
    }

    grl::Rc<Pipeline> GlDevice::createPipeline(const GraphicsPipelineDesc &desc)
    {
        return grl::makeRc<GlPipeline>(this, desc);
    }

    grl::Rc<Pipeline> GlDevice::createPipeline(const ComputePipelineDesc &desc)
    {
        return grl::makeRc<GlPipeline>(this, desc);
    }

    grl::Rc<CommandList> GlDevice::acquireCommandList(QueueType queueType)
    {
        for(auto& cmd : m_commandLists)
        {
            if(!cmd->inUse())
            {
                cmd->reset();
                return cmd;
            }
        }

        URHI_WARNING(m_commandLists.size() < 128, "Lots of command lists allocated ({})", m_commandLists.size());

        const auto cmd = grl::makeRc<GlCommandList>(this);
        m_commandLists.push_back(cmd);

        return m_commandLists.back();
    }

    grl::Rc<Texture> GlDevice::createTexture(const TextureDesc &desc)
    {
        return grl::makeRc<GlTexture>(this, desc);
    }

    grl::Rc<Sampler> GlDevice::createSampler(const SamplerDesc &desc)
    {
        return grl::makeRc<GlSampler>(this, desc);
    }

    grl::Rc<TextureView> GlDevice::createTextureView(const TextureViewDesc &desc)
    {
        return grl::makeRc<GlTextureView>(this, desc);
    }

    grl::Rc<Shader> GlDevice::createShader(const ShaderEntryPoint &entryPoint)
    {
        return grl::makeRc<GlShader>(this, entryPoint);
    }

    grl::Rc<Buffer> GlDevice::createBuffer(const BufferDesc &desc)
    {
        URHI_VALIDATE(desc.size != 0, "Invalid buffer size ({}) - buffer size must be greater than 0 and less than vram available", desc.size);
        URHI_VALIDATE(desc.usage != BufferUsage::None, "Invalid buffer usage - buffer usage must not be BufferUsage::None");
        URHI_VALIDATE(!(hasFlag(desc.usage, BufferUsage::Static) && hasFlag(desc.usage, BufferUsage::Dynamic)), "Invalid buffer usage - buffer usage cannot have BufferUsage::Dynamic and BufferUsage::Static");

        // if ((hasFlag(desc.usage, BufferUsage::Uniform) && !hasFlag(desc.usage, BufferUsage::Static)) || hasFlag(desc.usage, BufferUsage::Dynaimic))

        return grl::makeRc<GlBuffer>(this, desc);
    }

    void GlDevice::waitIdle()
    {

    }

    void GlDevice::submit(const grl::Rc<CommandList> &cmdList)
    {
        const auto glCmd = dynamic_cast<GlCommandList*>(cmdList.get());
        glCmd->submit();

        auto commandStream = glCmd->commands();
        GlCommandListEmitter emitter{ this, commandStream };
        for (auto& cmd : commandStream->commands())
            std::visit([&](auto& c) { emitter.emit(c); }, cmd);
    }

    clogr::Logger & GlDevice::logger() const
    {
        return m_context->logger();
    }

    GLuint GlDevice::getOrCreateFramebuffer(const RenderPassDesc &renderPass)
    {
        URHI_VALIDATE(!renderPass.colorAttachments.empty() || renderPass.depthAttachment.has_value(), "RenderPassDesc has no attachments — must have at least one color or depth attachment");

        for (const auto& att : renderPass.colorAttachments)
        {
            const auto* glTex = static_cast<const GlTextureView*>(att.target.get());
            if (glTex->handle() == 0)
                return 0;
        }

        if (renderPass.depthAttachment.has_value())
        {
            const auto* glTex = static_cast<const GlTextureView*>(renderPass.depthAttachment->target.get());
            if (glTex->handle() == 0)
                return 0;
        }

        const uint32_t rpHash = hashRenderPass(renderPass);

        const auto it = m_framebufferCache.find(rpHash);
        if (it != m_framebufferCache.end())
        {
            const GLenum status = glCheckNamedFramebufferStatus(it->second, GL_FRAMEBUFFER);
            if (status == GL_FRAMEBUFFER_COMPLETE)
                return it->second;

            glDeleteFramebuffers(1, &it->second);
            m_framebufferCache.erase(it);
        }

        GLuint fb = 0;
        glCreateFramebuffers(1, &fb);

        URHI_VALIDATE(fb != 0,
            "glCreateFramebuffers returned handle 0 — driver may be out of resources");

        for (size_t i = 0; i < renderPass.colorAttachments.size(); i++)
        {
            const auto* glTex = static_cast<const GlTextureView*>(renderPass.colorAttachments[i].target.get());
            glNamedFramebufferTexture(fb, GL_COLOR_ATTACHMENT0 + i, glTex->handle(), 0);
        }

        if (renderPass.depthAttachment.has_value())
        {
            const auto* glTex = static_cast<const GlTextureView*>(renderPass.depthAttachment->target.get());
            glNamedFramebufferTexture(fb, GL_DEPTH_ATTACHMENT, glTex->handle(), 0);
        }

        // Required for MRT — default state only enables COLOR_ATTACHMENT0
        if (!renderPass.colorAttachments.empty())
        {
            GLenum drawBuffers[8]; // GL guarantees at least 8
            for (size_t i = 0; i < renderPass.colorAttachments.size(); i++)
                drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
            glNamedFramebufferDrawBuffers(fb, static_cast<GLsizei>(renderPass.colorAttachments.size()), drawBuffers);
        }

        const GLenum status = glCheckNamedFramebufferStatus(fb, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            URHI_VALIDATE(false,
                "Newly created framebuffer {} (hash {:#010x}) is incomplete (status: {:#06x})",
                fb, rpHash, static_cast<uint32_t>(status));

            glDeleteFramebuffers(1, &fb);
            return 0;
        }

        m_framebufferCache[rpHash] = fb;
        return fb;
    }

    GLenum GlDevice::resolveDepthFormat(const PixelFormat requested) const
    {
        switch (requested)
        {
            case PixelFormat::Depth24PlusStencil8:
                    if (m_supportsD32FStencil) return GL_DEPTH32F_STENCIL8;
            return GL_DEPTH24_STENCIL8;

            case PixelFormat::Depth32Float:
                if (m_supportsD32F) return GL_DEPTH_COMPONENT32F;
            // Fall back to D24
            if (m_supportsD24) return GL_DEPTH_COMPONENT24;
            return GL_DEPTH_COMPONENT16;

            case PixelFormat::Depth16UNorm:
                return GL_DEPTH_COMPONENT16; // always supported

            default:
                URHI_VALIDATE(false, "Not a depth format");
            return GL_NONE;
        }
    }

    uint32_t GlDevice::hashRenderPass(const RenderPassDesc& renderPass)
    {
        uint32_t seed = 0;
        grl::Hash::hashCombine(seed, renderPass.colorAttachments.size());
        grl::Hash::hashCombine(seed, renderPass.depthAttachment.has_value());

        for (uint8_t i = 0; i < renderPass.colorAttachments.size(); i++)
        {
            const auto glTex = static_cast<GlTextureView*>(
                renderPass.colorAttachments[i].target.get()
            );
            grl::Hash::hashCombine(seed, glTex->handle());
        }

        if (renderPass.depthAttachment.has_value())
        {
            const auto glTex = static_cast<GlTextureView*>(
                renderPass.depthAttachment->target.get()
            );
            grl::Hash::hashCombine(seed, glTex->handle());
        }

        return seed;
    }
}
