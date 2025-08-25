#include "device.h"

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "glslang/Public/ShaderLang.h"
#include "glslang/Public/ResourceLimits.h"
#include "SPIRV/Logger.h"
#include "SPIRV/spvIR.h"
#include "SPIRV/GlslangToSpv.h"

#include "enums/shaderType.h"
#include "util/string.h"

namespace NRHI
{
    std::vector<uint32_t> compileToSpirv(const std::string& source_name,
                                    const ShaderType type,
                                    const std::string& source,
                                    const bool optimize = false)
    {
        // Map shader type to glslang stage
        EShLanguage stage;
        switch (type)
        {
            case ShaderType::Vertex:   stage = EShLangVertex; break;
            case ShaderType::Fragment: stage = EShLangFragment; break;
            case ShaderType::Compute:  stage = EShLangCompute; break;
            default:
                std::printf("Unknown shader type\n");
                return {};
        }

        // Create shader object
        glslang::TShader shader(stage);
        const char* sourcePtr = source.c_str();
        const char* namePtr = source_name.c_str();
        shader.setStrings(&sourcePtr, 1);
        shader.setSourceEntryPoint("main");
        shader.setEntryPoint("main");

        // Set up resources (using default limits)
        const TBuiltInResource* resources = GetDefaultResources();

        // Parse the shader
        constexpr auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);

        if (const bool parseResult = shader.parse(resources, 100, false, messages); !parseResult)
        {
            std::printf("Shader parsing failed:\n%s\n%s\n", shader.getInfoLog(), shader.getInfoDebugLog());
            return {};
        }

        glslang::TProgram program;
        program.addShader(&shader);

        if (const bool linkResult = program.link(messages); !linkResult)
        {
            std::printf("Shader linking failed:\n%s\n%s\n",
                       program.getInfoLog(), program.getInfoDebugLog());
            return {};
        }
        
        std::vector<uint32_t> spirv;
        spv::SpvBuildLogger logger;
        glslang::SpvOptions spvOptions;

        if (optimize) {
            spvOptions.optimizeSize = true;
        }

        glslang::GlslangToSpv(*program.getIntermediate(stage), spirv, &logger, &spvOptions);

        if (!logger.getAllMessages().empty())
        {
            std::printf("SPIR-V generation messages:\n%s\n", logger.getAllMessages().c_str());
        }

        return spirv;
    }

    Shader* Device::createShaderFromSource(const std::string &source, const std::string &filepath)
    {
        std::unordered_map<ShaderType, std::string> shaderSources;
        std::string common;

        std::istringstream stream(source);
        std::string line;
        std::string currentSection;
        std::string currentShaderType;
        bool inShaderSection = false;

        uint32_t shaderStartLine = 0;
        uint32_t currentLine = 0;

        while (std::getline(stream, line))
        {
            currentLine++;
            std::string trimmedLine = String::trim(line);

            if (trimmedLine.substr(0, 5) == "#type")
            {
                if (inShaderSection && !currentShaderType.empty())
                {
                    std::string shaderSource =  common +
                                                "#line "+std::to_string(shaderStartLine)+"\n"+
                                                currentSection;

                    shaderSource = String::trim(shaderSource);

                    if(currentShaderType == "vertex")
                        shaderSources[ShaderType::Vertex] = shaderSource;
                    else if(currentShaderType == "fragment")
                        shaderSources[ShaderType::Fragment] = shaderSource;
                    else if(currentShaderType == "compute")
                        shaderSources[ShaderType::Compute] = shaderSource;
                }

                shaderStartLine = currentLine+1;

                const size_t spacePos = trimmedLine.find_first_of(" \t", 5);
                if (spacePos != std::string::npos)
                {
                    currentShaderType = String::toLower(String::trim(trimmedLine.substr(spacePos + 1)));
                    currentSection.clear();
                    inShaderSection = true;
                }
            }
            else if (inShaderSection)
            {
                currentSection += line + "\n";
            }
            else
            {
                common += line + "\n";
            }
        }

        if (inShaderSection && !currentShaderType.empty())
        {
            std::string shaderSource =  common +
                                        "#line "+std::to_string(shaderStartLine)+"\n"+
                                        currentSection;

            shaderSource = String::trim(shaderSource);

            if(currentShaderType == "vertex")
                shaderSources[ShaderType::Vertex] = shaderSource;
            else if(currentShaderType == "fragment")
                shaderSources[ShaderType::Fragment] = shaderSource;
            else if(currentShaderType == "compute")
                shaderSources[ShaderType::Compute] = shaderSource;
        }

        std::unordered_map<ShaderType, std::vector<uint32_t>> spirvShaders;

        for(const auto& [shaderType, shaderSource] : shaderSources)
        {
            spirvShaders[shaderType] = compileToSpirv(filepath, shaderType, shaderSource);
        }

        return createShaderFromSpirvImpl(spirvShaders);
    }
}
