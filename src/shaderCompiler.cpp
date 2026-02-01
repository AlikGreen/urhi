#include "shaderCompiler.h"

#include <slang-com-ptr.h>
#include <slang.h>

#include "debug.h"

namespace Neon::RHI
{
    static ShaderReflection::DataType slangTypeToDataType(slang::TypeReflection* type)
    {
        auto kind = type->getKind();

        if (kind == slang::TypeReflection::Kind::Struct)
            return ShaderReflection::DataType::Struct;

        if (kind == slang::TypeReflection::Kind::Vector)
        {
            auto elemCount = type->getElementCount();
            auto elemType = type->getElementType();
            auto scalarKind = elemType->getScalarType();

            if (scalarKind == slang::TypeReflection::ScalarType::Float32)
            {
                switch (elemCount)
                {
                    case 2: return ShaderReflection::DataType::Float2;
                    case 3: return ShaderReflection::DataType::Float3;
                    case 4: return ShaderReflection::DataType::Float4;
                }
            }
            else if (scalarKind == slang::TypeReflection::ScalarType::Int32)
            {
                switch (elemCount)
                {
                    case 2: return ShaderReflection::DataType::Int2;
                    case 3: return ShaderReflection::DataType::Int3;
                    case 4: return ShaderReflection::DataType::Int4;
                }
            }
            else if (scalarKind == slang::TypeReflection::ScalarType::UInt32)
            {
                switch (elemCount)
                {
                    case 2: return ShaderReflection::DataType::UInt2;
                    case 3: return ShaderReflection::DataType::UInt3;
                    case 4: return ShaderReflection::DataType::UInt4;
                }
            }
        }

        if (kind == slang::TypeReflection::Kind::Matrix)
        {
            auto rowCount = type->getRowCount();
            auto colCount = type->getColumnCount();

            if (rowCount == 3 && colCount == 3)
                return ShaderReflection::DataType::Mat3;
            if (rowCount == 4 && colCount == 4)
                return ShaderReflection::DataType::Mat4;
        }

        if (kind == slang::TypeReflection::Kind::Scalar)
        {
            auto scalarKind = type->getScalarType();

            if (scalarKind == slang::TypeReflection::ScalarType::Int32)
                return ShaderReflection::DataType::Int;
            if (scalarKind == slang::TypeReflection::ScalarType::UInt32)
                return ShaderReflection::DataType::UInt;
        }

        return ShaderReflection::DataType::Float;
    }

    CompiledShader ShaderCompiler::compile(const ShaderCompileDescription& compileDesc)
    {
        using namespace Slang;
        ComPtr<slang::IGlobalSession> globalSession;
        slang::createGlobalSession(globalSession.writeRef());

        // Configure session with SPIR-V target
        slang::SessionDesc sessionDesc = {};
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

        // Set up SPIR-V target
        slang::TargetDesc targetDesc = {};
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = globalSession->findProfile("spirv_1_6");
        targetDesc.forceGLSLScalarBufferLayout = false;

        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;

        // Add search paths for includes
        const char** searchPaths = new const char*[compileDesc.includePaths.size()];
        for (size_t i = 0; i < compileDesc.includePaths.size(); ++i)
        {
            searchPaths[i] = compileDesc.includePaths[i].c_str();
        }
        sessionDesc.searchPaths = searchPaths;
        sessionDesc.searchPathCount = static_cast<int>(compileDesc.includePaths.size());

        // Create session
        ComPtr<slang::ISession> session;
        globalSession->createSession(sessionDesc, session.writeRef());

        // Load shader module
        ComPtr<slang::IModule> module;
        ComPtr<slang::IBlob> diagnostics;
        module = session->loadModuleFromSourceString(
            "shaderModule",
            compileDesc.path.c_str(),
            compileDesc.source.c_str(),
            diagnostics.writeRef()
        );

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

        // Get reflection data from Slang
        slang::ProgramLayout* layout = linkedProgram->getLayout();

        CompiledShader compiled;

        // Reflect parameters using Slang's reflection API
        SlangUInt paramCount = layout->getParameterCount();
        for (SlangUInt i = 0; i < paramCount; i++)
        {
            slang::VariableLayoutReflection* param = layout->getParameterByIndex(i);
            auto typeLayout = param->getTypeLayout();
            auto type = typeLayout->getType();

            const char* name = param->getName();

            uint32_t binding = 0;
            bool isPushConstant = false;

            auto categoryCount = param->getCategoryCount();
            for (SlangInt c = 0; c < categoryCount; c++)
            {
                auto category = param->getCategoryByIndex(c);

                binding = param->getOffset(category);
                if (category == SLANG_PARAMETER_CATEGORY_PUSH_CONSTANT_BUFFER) // stupid probably doesnt do anything but i cant figure out how to add push constants in a nice way anyway so...
                {
                    isPushConstant = true;
                }
                break;
            }

            auto kind = type->getKind();

            if (kind == slang::TypeReflection::Kind::ConstantBuffer ||
                kind == slang::TypeReflection::Kind::ParameterBlock ||
                isPushConstant)
            {
                ShaderReflection::Resource resource{};
                resource.type = isPushConstant ? ShaderReflection::ResourceType::PushConstant : ShaderReflection::ResourceType::ConstantBuffer;
                resource.name = name;
                resource.binding = binding;

                auto elementType = type->getElementType();
                resource.totalSize = static_cast<uint32_t>(typeLayout->getSize());

                // Reflect struct members
                SlangUInt fieldCount = elementType->getFieldCount();
                for (SlangUInt f = 0; f < fieldCount; f++)
                {
                    slang::VariableReflection* field = elementType->getFieldByIndex(f);
                    auto fieldTypeLayout = typeLayout->getElementTypeLayout()->getFieldByIndex(f);

                    ShaderReflection::Member member;
                    member.name = field->getName();
                    member.offset = static_cast<uint32_t>(fieldTypeLayout->getOffset());
                    member.size = static_cast<uint32_t>(fieldTypeLayout->getTypeLayout()->getSize());
                    member.type = slangTypeToDataType(field->getType());

                    resource.members.push_back(member);
                }

                compiled.reflection.resources.push_back(resource);
            }
            // Handle storage buffers
            else if (kind == slang::TypeReflection::Kind::Resource)
            {
                auto resourceShape = type->getResourceShape();
                auto access = type->getResourceAccess();

                if ((resourceShape & SLANG_RESOURCE_BASE_SHAPE_MASK) == SLANG_STRUCTURED_BUFFER)
                {
                    ShaderReflection::Resource resource{};
                    resource.type = ShaderReflection::ResourceType::StorageBuffer;
                    resource.name = name;
                    resource.binding = binding;

                    compiled.reflection.resources.push_back(resource);
                }

                else if ((resourceShape & SLANG_RESOURCE_BASE_SHAPE_MASK) == SLANG_TEXTURE_2D ||
                         (resourceShape & SLANG_RESOURCE_BASE_SHAPE_MASK) == SLANG_TEXTURE_CUBE)
                {
                    ShaderReflection::Resource resource{};
                    resource.name = name;
                    resource.binding = binding;

                    // Check if it's a storage image (RW texture) or sampled image
                    if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                        resource.type = ShaderReflection::ResourceType::StorageImage;
                    else
                        resource.type = ShaderReflection::ResourceType::Texture;

                    compiled.reflection.resources.push_back(resource);
                }
            }
            else if (kind == slang::TypeReflection::Kind::SamplerState)
            {
                ShaderReflection::Resource resource{};
                resource.type = ShaderReflection::ResourceType::Sampler;
                resource.name = name;
                resource.binding = binding;

                compiled.reflection.resources.push_back(resource);
            }
        }

        // Generate SPIR-V code
        ComPtr<slang::IBlob> spirvCode;
        linkedProgram->getTargetCode(0, spirvCode.writeRef(), diagnostics.writeRef());

        diagnosticsString = "";
        if (diagnostics)
            diagnosticsString = static_cast<const char *>(diagnostics->getBufferPointer());

        Debug::ensure(spirvCode, "Slang - Code generation failed: {}", diagnosticsString);

        const auto* spirvData = static_cast<const uint32_t*>(spirvCode->getBufferPointer());
        const size_t spirvSizeInBytes = spirvCode->getBufferSize();
        const size_t spirvSizeInWords = spirvSizeInBytes / sizeof(uint32_t);

        compiled.spirv = std::vector<uint32_t>(spirvData, spirvData + spirvSizeInWords);

        delete[] searchPaths;

        return compiled;
    }
}