#include "glShader.h"

#include <regex>

#include "glContext.h"
#include "glConvert.h"
#include "spirv_glsl.hpp"
#include "validation.h"


namespace urhi
{
    std::unordered_map<uint32_t, std::string> GlShader::m_glslCache{};

    GlShader::GlShader(GlDevice* device, const ShaderEntryPoint& ep)
    : m_device(device), m_reflection(ep.reflection), m_stage(ep.stage)
    {
        const std::string glsl = getOrCompileGlsl(ep.spirvCode);

        const GLenum glStage = GlConvert::shaderStage(ep.stage);
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

    std::vector<uint32_t> stripNonUniform(const std::vector<uint32_t>& spirv)
    {
        std::vector<uint32_t> result;
        result.insert(result.end(), spirv.begin(), spirv.begin() + 5);

        size_t i = 5;
        while (i < spirv.size())
        {
            uint16_t wordCount = spirv[i] >> 16;
            uint16_t opcode    = spirv[i] & 0xFFFF;
            bool skip = false;

            // OpCapability = 17
            if (opcode == 17 && wordCount >= 2)
            {
                uint32_t cap = spirv[i + 1];
                switch (cap)
                {
                    case 5301: // ShaderNonUniformEXT
                    case 5302: // RuntimeDescriptorArrayEXT  <-- THIS WAS MISSING
                    case 5303: // InputAttachmentArrayDynamicIndexingEXT
                    case 5304: // UniformTexelBufferArrayDynamicIndexingEXT
                    case 5305: // StorageTexelBufferArrayDynamicIndexingEXT
                    case 5306: // UniformBufferArrayNonUniformIndexingEXT
                    case 5307: // SampledImageArrayNonUniformIndexingEXT
                    case 5308: // StorageBufferArrayNonUniformIndexingEXT
                    case 5309: // StorageImageArrayNonUniformIndexingEXT
                    case 5310: // InputAttachmentArrayNonUniformIndexingEXT
                    case 5311: // UniformTexelBufferArrayNonUniformIndexingEXT
                    case 5312: // StorageTexelBufferArrayNonUniformIndexingEXT
                        skip = true;
                        break;
                }
            }

            // OpDecorate = 71, OpDecorateId = 332
            if ((opcode == 71 || opcode == 332) && wordCount >= 3)
            {
                if (spirv[i + 2] == 5300) // NonUniformEXT
                    skip = true;
            }

            // OpMemberDecorate = 72
            if (opcode == 72 && wordCount >= 4)
            {
                if (spirv[i + 3] == 5300) // NonUniformEXT
                    skip = true;
            }

            // OpExtension = 10 - strip SPV_EXT_descriptor_indexing
            if (opcode == 10)
            {
                // Extension name is packed into words starting at i+1
                const char* extName = reinterpret_cast<const char*>(&spirv[i + 1]);
                if (std::string(extName) == "SPV_EXT_descriptor_indexing")
                    skip = true;
            }

            if (!skip)
                result.insert(result.end(), spirv.begin() + i, spirv.begin() + i + wordCount);

            i += wordCount;
        }

        return result;
    }

    std::string GlShader::getOrCompileGlsl(const std::vector<uint32_t>& spirv) const
    {
        uint32_t hash = grl::Hash::fnv1a32(std::as_bytes(std::span(spirv)));
        grl::Hash::hashCombine(hash, compilerVersion);

        if (const auto it = m_glslCache.find(hash); it != m_glslCache.end())
            return it->second;

        if (const auto cached = tryLoadDisk(hash))
            return m_glslCache[hash] = *cached;

        try
        {
            auto cleanedSpirv = stripNonUniform(spirv);
            spirv_cross::CompilerGLSL compiler(cleanedSpirv);

            uint32_t idBound = compiler.get_current_id_bound();
            for (uint32_t id = 0; id < idBound; ++id)
            {
                if (compiler.has_decoration(id, spv::DecorationNonUniform))
                {
                    compiler.unset_decoration(id, spv::DecorationNonUniform);
                }

                try
                {
                    const auto& type = compiler.get_type(id);
                    if (type.basetype == spirv_cross::SPIRType::Struct)
                    {
                        for (uint32_t i = 0; i < type.member_types.size(); ++i)
                        {
                            if (compiler.has_member_decoration(id, i, spv::DecorationNonUniform))
                            {
                                compiler.unset_member_decoration(id, i, spv::DecorationNonUniform);
                            }
                        }
                    }
                }
                catch (const spirv_cross::CompilerError&)
                {

                }
            }

            spirv_cross::CompilerGLSL::Options opts;
            opts.version = 460;
            opts.es = false;
            opts.emit_push_constant_as_uniform_buffer = true;
            opts.vulkan_semantics = false;
            // opts.enable_row_major_load_workaround

            compiler.set_common_options(opts);

            compiler.build_combined_image_samplers();

            spirv_cross::ShaderResources resources = compiler.get_shader_resources();

            if(!resources.push_constant_buffers.empty())
            {
                compiler.set_name(resources.push_constant_buffers[0].base_type_id, kPushConstantBlockName);
            }

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
