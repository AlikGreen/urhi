#include "glShader.h"

#include "glContext.h"
#include "glConvert.h"
#include "spirv_hlsl.hpp"
#include "validation.h"


namespace urhi
{
    std::unordered_map<uint32_t, std::string> GlShader::m_glslCache{};

    GlShader::GlShader(GlDevice* device, const ShaderEntryPoint& entryPoint)
    : m_device(device), m_entryPoint(entryPoint)
    {
        // TODO fix this whole thing. the name setting and reflection is not robust
        spirv_cross::CompilerGLSL compiler(entryPoint.spirv);

        spirv_cross::CompilerGLSL::Options opts;
        opts.version = 460;
        opts.es = false;
        opts.emit_push_constant_as_uniform_buffer = true;

        compiler.set_common_options(opts);

        reflect(compiler, entryPoint.stage);

        const std::string glsl = getOrCompileGlsl(compiler, entryPoint.spirv);

        const GLenum glStage = GlConvert::shaderStage(entryPoint.stage);
        m_handle = glCreateShader(glStage);
        const char* src = glsl.c_str();
        glShaderSource(m_handle, 1, &src, nullptr);
        glCompileShader(m_handle);

        int success;
        glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(m_handle, 512, nullptr, infoLog);
            URHI_VALIDATE(false, "Error compiling shader:\n{}", infoLog);
        }
    }

    GlShader::~GlShader()
    {
        glDeleteShader(m_handle);
    }

    void GlShader::reflect(spirv_cross::CompilerGLSL& compiler, ShaderStage stage)
    {
        compiler.build_combined_image_samplers();
        reflectCombinedSamplers(compiler);
        reflectUbos(compiler);
    }

   void GlShader::reflectCombinedSamplers(const spirv_cross::CompilerGLSL& compiler)
    {
        for (auto& combined : compiler.get_combined_image_samplers())
        {
            CombinedSamplerInfo info;
            info.textureUnit     = compiler.get_decoration(combined.combined_id, spv::DecorationBinding);
            info.combinedName    = compiler.get_name(combined.combined_id);
            info.texNameHash     = NameRegistry::getHash(compiler.get_name(combined.image_id));
            info.samplerNameHash = NameRegistry::getHash(compiler.get_name(combined.sampler_id));
            m_combinedSamplers.push_back(info);
        }

        auto resources = compiler.get_shader_resources();

        for (auto& image : resources.storage_images)
        {
            StorageImageInfo info{};
            info.unit = compiler.get_decoration(image.id, spv::DecorationBinding);
            info.name = compiler.get_name(image.id);

            spirv_cross::Bitset flags = compiler.get_buffer_block_flags(image.id);

            if(flags.get(spv::DecorationNonWritable))
                info.access = ResourceAccess::ReadOnly;
            if(flags.get(spv::DecorationNonReadable))
                info.access = ResourceAccess::WriteOnly;
            else
                info.access = ResourceAccess::ReadWrite;

            m_storageImage.push_back(info);
        }
    }

    void GlShader::reflectUbos(spirv_cross::CompilerGLSL& compiler)
    {
        auto resources = compiler.get_shader_resources();

        for (const auto& ubo : resources.uniform_buffers)
        {
            BufferReflection refl;
            refl.instanceName = compiler.get_name(ubo.id);
            refl.binding      = compiler.get_decoration(ubo.id, spv::DecorationBinding);
            refl.access       = ResourceAccess::ReadOnly;

            m_ubos.push_back(refl);
        }

        for (auto& ssbo : resources.storage_buffers)
        {
            BufferReflection refl;
            refl.instanceName = compiler.get_name(ssbo.id);
            refl.binding      = compiler.get_decoration(ssbo.id, spv::DecorationBinding);

            spirv_cross::Bitset flags = compiler.get_buffer_block_flags(ssbo.id);
            if      (flags.get(spv::DecorationNonWritable)) refl.access = ResourceAccess::ReadOnly;
            else if (flags.get(spv::DecorationNonReadable)) refl.access = ResourceAccess::WriteOnly;
            else                                               refl.access = ResourceAccess::ReadWrite;

            m_ssbos.push_back(refl);
        }

        for (auto& pc : resources.push_constant_buffers)
        {
            BufferReflection refl;
            refl.instanceName = compiler.get_name(pc.id);
            refl.binding      = 0;
            refl.access       = ResourceAccess::ReadOnly;

            compiler.set_name(pc.base_type_id, kPushConstantBlockName);

            m_pushConstant = refl;
        }

    }

    std::string GlShader::getOrCompileGlsl(spirv_cross::CompilerGLSL& compiler, const std::vector<uint32_t>& spirv) const
    {
        uint32_t hash = grl::Hash::fnv1a32(std::as_bytes(std::span(spirv)));
        grl::Hash::hashCombine(hash, compilerVersion);

        if (const auto it = m_glslCache.find(hash); it != m_glslCache.end())
            return it->second;

        if (const auto cached = tryLoadDisk(hash))
            return m_glslCache[hash] = *cached;

        try
        {
            const std::string glsl = compiler.compile();
            saveDisk(hash, glsl);
            return m_glslCache[hash] = glsl;
        }
        catch (const spirv_cross::CompilerError& e)
        {
            URHI_VALIDATE(false, "Failed to compile shader: {}", e.what());
            return "";
        }
    }

    std::optional<std::string> GlShader::tryLoadDisk(const uint32_t hash) const
    {
        const std::string fileName = fmt::format("{:X}", hash) + ".glsl";
        const auto path = m_device->context()->cachePath() / fileName;
        auto result = grl::File::read(path.string());

        if(result.has_value())
            return result.value();

        return std::nullopt;
    }

    void GlShader::saveDisk(const uint32_t hash, std::string glsl) const
    {
        const std::string fileName = fmt::format("{:X}", hash) + ".glsl";
        const auto path = m_device->context()->cachePath() / fileName;

        grl::File::write(path.string(), glsl);
    }
}
