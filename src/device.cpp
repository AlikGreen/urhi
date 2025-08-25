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

TBuiltInResource DefaultTBuiltInResource()
{
    TBuiltInResource R = {};
    R.maxLights = 32;
    R.maxClipPlanes = 6;
    R.maxTextureUnits = 32;
    R.maxTextureCoords = 32;
    R.maxVertexAttribs = 64;
    R.maxVertexUniformComponents = 4096;
    R.maxVaryingFloats = 64;
    R.maxVertexTextureImageUnits = 32;
    R.maxCombinedTextureImageUnits = 80;
    R.maxTextureImageUnits = 32;
    R.maxFragmentUniformComponents = 4096;
    R.maxDrawBuffers = 32;
    R.maxVertexUniformVectors = 128;
    R.maxVaryingVectors = 8;
    R.maxFragmentUniformVectors = 16;
    R.maxVertexOutputVectors = 16;
    R.maxFragmentInputVectors = 15;
    R.minProgramTexelOffset = -8;
    R.maxProgramTexelOffset = 7;
    R.maxClipDistances = 8;
    R.maxComputeWorkGroupCountX = 65535;
    R.maxComputeWorkGroupCountY = 65535;
    R.maxComputeWorkGroupCountZ = 65535;
    R.maxComputeWorkGroupSizeX = 1024;
    R.maxComputeWorkGroupSizeY = 1024;
    R.maxComputeWorkGroupSizeZ = 64;
    R.maxComputeUniformComponents = 1024;
    R.maxComputeTextureImageUnits = 16;
    R.maxComputeImageUniforms = 8;
    R.maxComputeAtomicCounters = 8;
    R.maxComputeAtomicCounterBuffers = 1;
    R.maxVaryingComponents = 60;
    R.maxVertexOutputComponents = 64;
    R.maxGeometryInputComponents = 64;
    R.maxGeometryOutputComponents = 128;
    R.maxFragmentInputComponents = 128;
    R.maxImageUnits = 8;
    R.maxCombinedImageUnitsAndFragmentOutputs = 8;
    R.maxCombinedShaderOutputResources = 8;
    R.maxImageSamples = 0;
    R.maxVertexImageUniforms = 0;
    R.maxTessControlImageUniforms = 0;
    R.maxTessEvaluationImageUniforms = 0;
    R.maxGeometryImageUniforms = 0;
    R.maxFragmentImageUniforms = 8;
    R.maxCombinedImageUniforms = 8;
    R.maxGeometryTextureImageUnits = 16;
    R.maxGeometryOutputVertices = 256;
    R.maxGeometryTotalOutputComponents = 1024;
    R.maxGeometryUniformComponents = 1024;
    R.maxGeometryVaryingComponents = 64;
    R.maxTessControlInputComponents = 128;
    R.maxTessControlOutputComponents = 128;
    R.maxTessControlTextureImageUnits = 16;
    R.maxTessControlUniformComponents = 1024;
    R.maxTessControlTotalOutputComponents = 4096;
    R.maxTessEvaluationInputComponents = 128;
    R.maxTessEvaluationOutputComponents = 128;
    R.maxTessEvaluationTextureImageUnits = 16;
    R.maxTessEvaluationUniformComponents = 1024;
    R.maxTessPatchComponents = 120;
    R.maxPatchVertices = 32;
    R.maxTessGenLevel = 64;
    R.maxViewports = 16;
    R.maxVertexAtomicCounters = 0;
    R.maxTessControlAtomicCounters = 0;
    R.maxTessEvaluationAtomicCounters = 0;
    R.maxGeometryAtomicCounters = 0;
    R.maxFragmentAtomicCounters = 8;
    R.maxCombinedAtomicCounters = 8;
    R.maxAtomicCounterBindings = 1;
    R.maxVertexAtomicCounterBuffers = 0;
    R.maxTessControlAtomicCounterBuffers = 0;
    R.maxTessEvaluationAtomicCounterBuffers = 0;
    R.maxGeometryAtomicCounterBuffers = 0;
    R.maxFragmentAtomicCounterBuffers = 1;
    R.maxCombinedAtomicCounterBuffers = 1;
    R.maxAtomicCounterBufferSize = 16384;
    R.maxTransformFeedbackBuffers = 4;
    R.maxTransformFeedbackInterleavedComponents = 64;
    R.maxCullDistances = 8;
    R.maxCombinedClipAndCullDistances = 8;
    R.maxSamples = 4;
    // Limits (booleans)
    R.limits.nonInductiveForLoops = true;
    R.limits.whileLoops = true;
    R.limits.doWhileLoops = true;
    R.limits.generalUniformIndexing = true;
    R.limits.generalAttributeMatrixVectorIndexing = true;
    R.limits.generalVaryingIndexing = true;
    R.limits.generalSamplerIndexing = true;
    R.limits.generalVariableIndexing = true;
    R.limits.generalConstantMatrixVectorIndexing = true;
    return R;
}

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
        const TBuiltInResource resources = DefaultTBuiltInResource();

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
