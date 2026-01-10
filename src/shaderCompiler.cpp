#include "shaderCompiler.h"

#include <slang-com-ptr.h>
#include <slang.h>

#include "debug.h"

namespace Neon::RHI
{
    std::vector<uint32_t> ShaderCompiler::compile(const ShaderCompileDescription& compileDesc)
    {
        using namespace Slang;
        ComPtr<slang::IGlobalSession> globalSession;
        slang::createGlobalSession(globalSession.writeRef());

        // 2. Configure session with SPIR-V target
        slang::SessionDesc sessionDesc = {};

        // Set up SPIR-V target
        slang::TargetDesc targetDesc = {};
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = globalSession->findProfile("spirv_1_5");

        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;

        // Optional: Add search paths for includes
        const char** searchPaths = new const char*[compileDesc.includePaths.size()];
        for (size_t i = 0; i < compileDesc.includePaths.size(); ++i)
        {
            searchPaths[i] = compileDesc.includePaths[i].c_str();
        }
        sessionDesc.searchPaths = searchPaths;
        sessionDesc.searchPathCount = static_cast<int>(compileDesc.includePaths.size());

        // Macros if needed
        // constexpr slang::PreprocessorMacroDesc macros[] = {
        //     { "MY_DEFINE", "1" }
        // };
        // sessionDesc.preprocessorMacros = macros;
        // sessionDesc.preprocessorMacroCount = 1;

        // 3. Create session
        ComPtr<slang::ISession> session;
        globalSession->createSession(sessionDesc, session.writeRef());

        // 4. Load shader module
        ComPtr<slang::IModule> module;
        ComPtr<slang::IBlob> diagnostics;
        module = session->loadModuleFromSourceString("shaderModule", compileDesc.path.c_str(),
                                              compileDesc.source.c_str(), diagnostics.writeRef());

        std::string diagnosticsString{};
        if (diagnostics)
            diagnosticsString = static_cast<const char *>(diagnostics->getBufferPointer());

        Debug::ensure(module, "Slang - Compilation failed: {}", diagnosticsString);

        std::vector<slang::IComponentType*> components;
        components.push_back(module);

        const uint32_t entryPointCount = module->getDefinedEntryPointCount();
        for (SlangInt32 i = 0; i < entryPointCount; ++i)
        {
            ComPtr<slang::IEntryPoint> entryPoint;
            module->getDefinedEntryPoint(i, entryPoint.writeRef());
            components.push_back(entryPoint);
        }

        ComPtr<slang::IComponentType> program;
        session->createCompositeComponentType(
            components.data(),
            static_cast<int>(components.size()),
            program.writeRef(),
            diagnostics.writeRef());
        
        ComPtr<slang::IComponentType> linkedProgram;
        program->link(linkedProgram.writeRef(), diagnostics.writeRef());

        diagnosticsString = "";
        if (diagnostics)
            diagnosticsString = static_cast<const char *>(diagnostics->getBufferPointer());

        Debug::ensure(linkedProgram, "Slang - Linking failed: {}", diagnosticsString);

        ComPtr<slang::IBlob> spirvCode;
        linkedProgram->getTargetCode(0, spirvCode.writeRef(), diagnostics.writeRef());

        diagnosticsString = "";
        if (diagnostics)
            diagnosticsString = static_cast<const char *>(diagnostics->getBufferPointer());

        Debug::ensure(spirvCode, "Slang - Code generation failed: {}", diagnosticsString);

        const auto* spirvData = static_cast<const uint32_t*>(spirvCode->getBufferPointer());
        const size_t spirvSizeInBytes = spirvCode->getBufferSize();
        const size_t spirvSizeInWords = spirvSizeInBytes / sizeof(uint32_t);

        std::vector spirvVector(spirvData, spirvData + spirvSizeInWords);

        return spirvVector;
    }
}
