#include "shaderCompiler.h"

#include <algorithm>
#include <clogr.h>

#include "descriptions/shaderEntryPoint.h"

namespace urhi
{
    uint32_t dataTypeSize(const ShaderReflection::DataType type)
    {
        switch (type)
        {
            case ShaderReflection::DataType::Float:  return 4;
            case ShaderReflection::DataType::Float2: return 8;
            case ShaderReflection::DataType::Float3: return 12;
            case ShaderReflection::DataType::Float4: return 16;
            case ShaderReflection::DataType::Int:    return 4;
            case ShaderReflection::DataType::Int2:   return 8;
            case ShaderReflection::DataType::Int3:   return 12;
            case ShaderReflection::DataType::Int4:   return 16;
            case ShaderReflection::DataType::UInt:   return 4;
            case ShaderReflection::DataType::UInt2:  return 8;
            case ShaderReflection::DataType::UInt3:  return 12;
            case ShaderReflection::DataType::UInt4:  return 16;
            case ShaderReflection::DataType::Mat3:   return 36;
            case ShaderReflection::DataType::Mat4:   return 64;
            default: return 0;
        }
    }

   std::vector<ShaderEntryPoint> ShaderCompiler::compile(const ShaderCompileDesc& desc)
    {
        Slang::ComPtr<slang::IGlobalSession> globalSession;
        slang::createGlobalSession(globalSession.writeRef());

        slang::TargetDesc targetDesc{};
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = globalSession->findProfile("spirv_1_6");
        targetDesc.forceGLSLScalarBufferLayout = false;

        std::vector<const char*> searchPaths;
        searchPaths.reserve(desc.includePaths.size());
        for (const auto& path : desc.includePaths)
        {
            searchPaths.push_back(path.c_str());
        }

        slang::SessionDesc sessionDesc{};
        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.searchPaths = searchPaths.data();
        sessionDesc.searchPathCount = static_cast<int>(searchPaths.size());
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

        Slang::ComPtr<slang::ISession> session;
        globalSession->createSession(sessionDesc, session.writeRef());

        Slang::ComPtr<slang::IBlob> diagnostics;

        slang::IModule* module = session->loadModuleFromSourceString(
            "shaderModule",
            desc.path.c_str(),
            desc.source.c_str(),
            diagnostics.writeRef());

        if (diagnostics)
        {
            auto msg = static_cast<const char*>(diagnostics->getBufferPointer());
            clogr::ensure(module != nullptr, "Slang compilation failed: {}", msg);
        }

        std::vector<Slang::ComPtr<slang::IEntryPoint>> entryPoints;
        for (SlangInt32 i = 0; i < module->getDefinedEntryPointCount(); ++i)
        {
            Slang::ComPtr<slang::IEntryPoint> ep;
            module->getDefinedEntryPoint(i, ep.writeRef());
            entryPoints.push_back(ep);
        }

        std::vector<ShaderEntryPoint> shaderEntryPoints;

        for (uint32_t i = 0; i < entryPoints.size(); ++i)
        {
            slang::IComponentType* components[] = {module, entryPoints[i]};

            Slang::ComPtr<slang::IComponentType> program;
            session->createCompositeComponentType(
                components, 2,
                program.writeRef(),
                diagnostics.writeRef());

            Slang::ComPtr<slang::IComponentType> linked;
            program->link(linked.writeRef(), diagnostics.writeRef());

            if (diagnostics)
            {
                auto msg = static_cast<const char*>(diagnostics->getBufferPointer());
                clogr::ensure(linked != nullptr, "Slang linking failed: {}", msg);
            }

            auto* layout = linked->getLayout();
            auto* epReflection = layout->getEntryPointByIndex(0);

            ShaderEntryPoint stageData;
            stageData.name = epReflection->getName();
            stageData.stage = convertStage(epReflection->getStage());
            stageData.reflection = extractReflection(layout, epReflection, desc);
            stageData.spirv = extractSpirv(linked);

            shaderEntryPoints.push_back(std::move(stageData));
        }

        return shaderEntryPoints;
    }

    Slang::ComPtr<slang::IComponentType> ShaderCompiler::compileAndLink(
        slang::ISession* session,
        const ShaderCompileDesc& desc)
    {
        Slang::ComPtr<slang::IBlob> diagnostics;

        slang::IModule* module = session->loadModuleFromSourceString(
            "shaderModule",
            desc.path.c_str(),
            desc.source.c_str(),
            diagnostics.writeRef());

        if (diagnostics)
        {
            auto msg = static_cast<const char*>(diagnostics->getBufferPointer());
            clogr::ensure(module != nullptr, "Slang compilation failed: {}", msg);
        }

        std::vector<slang::IComponentType*> components;
        components.push_back(module);

        std::vector<Slang::ComPtr<slang::IEntryPoint>> entryPointStorage;
        for (SlangInt32 i = 0; i < module->getDefinedEntryPointCount(); ++i)
        {
            Slang::ComPtr<slang::IEntryPoint> ep;
            module->getDefinedEntryPoint(i, ep.writeRef());
            components.push_back(ep);
            entryPointStorage.push_back(std::move(ep));
        }

        Slang::ComPtr<slang::IComponentType> program;
        session->createCompositeComponentType(
            components.data(),
            static_cast<int>(components.size()),
            program.writeRef(),
            diagnostics.writeRef());

        Slang::ComPtr<slang::IComponentType> linked;
        program->link(linked.writeRef(), diagnostics.writeRef());

        if (diagnostics)
        {
            auto msg = static_cast<const char*>(diagnostics->getBufferPointer());
            clogr::ensure(linked != nullptr, "Slang linking failed: {}", msg);
        }

        return linked;
    }

    std::vector<uint32_t> ShaderCompiler::extractSpirv(slang::IComponentType* linkedProgram)
    {
        Slang::ComPtr<slang::IBlob> spirvBlob;
        Slang::ComPtr<slang::IBlob> diagnostics;

        linkedProgram->getTargetCode(0, spirvBlob.writeRef(), diagnostics.writeRef());

        if (diagnostics)
        {
            auto msg = static_cast<const char*>(diagnostics->getBufferPointer());
            clogr::ensure(spirvBlob != nullptr, "Slang code generation failed: {}", msg);
        }

        auto* data = static_cast<const uint32_t*>(spirvBlob->getBufferPointer());
        size_t wordCount = spirvBlob->getBufferSize() / sizeof(uint32_t);

        return {data, data + wordCount};
    }

    ShaderReflection ShaderCompiler::extractReflection(
        slang::ProgramLayout* layout,
        slang::EntryPointReflection* entryPoint,
        const ShaderCompileDesc& desc)
    {
        ShaderReflection reflection;

        if (entryPoint->getStage() == SLANG_STAGE_VERTEX)
        {
            extractVertexInput(entryPoint, desc, reflection);
        }
        else if (entryPoint->getStage() == SLANG_STAGE_COMPUTE)
        {
            SlangUInt size[3];
            entryPoint->getComputeThreadGroupSize(3, size);
            reflection.computeInfo = ShaderReflection::ComputeInfo{
                static_cast<uint32_t>(size[0]),
                static_cast<uint32_t>(size[1]),
                static_cast<uint32_t>(size[2])
            };
        }

        extractResources(layout, reflection);

        return reflection;
    }

    void ShaderCompiler::extractVertexInput(slang::EntryPointReflection* entryPoint, const ShaderCompileDesc& desc, ShaderReflection& reflection)
    {
        uint32_t bindingIndex = 0;
        uint32_t locationIndex = 0;

        for (uint32_t i = 0; i < entryPoint->getParameterCount(); ++i)
        {
            auto* param = entryPoint->getParameterByIndex(i);

            if (param->getCategory() != slang::ParameterCategory::VaryingInput)
                continue;

            auto* type = param->getType();

            ShaderReflection::VertexBinding binding;
            binding.binding = bindingIndex++;
            binding.stride = 0;
            binding.structName = type->getName() ? type->getName() : "anonymous";

            if (type->getKind() == slang::TypeReflection::Kind::Struct)
            {
                for (uint32_t f = 0; f < type->getFieldCount(); ++f)
                {
                    auto* field = type->getFieldByIndex(f);
                    auto* fieldType = field->getType();

                    ShaderReflection::VertexAttribute attr;
                    attr.name = field->getName() ? field->getName() : "";
                    attr.location = locationIndex++;
                    attr.type = convertDataType(fieldType);
                    attr.offset = binding.stride;

                    binding.stride += dataTypeSize(attr.type);
                    binding.attributes.push_back(std::move(attr));
                }
            }
            else
            {
                ShaderReflection::VertexAttribute attr;
                attr.name = param->getName() ? param->getName() : "";
                attr.location = locationIndex++;
                attr.type = convertDataType(type);
                attr.offset = 0;

                binding.stride = dataTypeSize(attr.type);
                binding.attributes.push_back(std::move(attr));
            }

            reflection.vertexBindings.push_back(std::move(binding));
        }
    }

    void ShaderCompiler::extractResources(
        slang::ProgramLayout* layout,
        ShaderReflection& reflection)
    {
        for (uint32_t i = 0; i < layout->getParameterCount(); ++i)
        {
            auto* param = layout->getParameterByIndex(i);
            auto* typeLayout = param->getTypeLayout();
            auto* type = typeLayout->getType();

            uint32_t set = 0;
            uint32_t binding = 0;

            for (uint32_t c = 0; c < param->getCategoryCount(); ++c)
            {
                auto category = param->getCategoryByIndex(c);
                binding = param->getOffset(category);
                set = param->getBindingSpace(category);
                break;
            }

            const auto kind = type->getKind();

            if (kind == slang::TypeReflection::Kind::ConstantBuffer ||
                kind == slang::TypeReflection::Kind::ParameterBlock)
            {
                extractConstantBuffer(param, type, typeLayout, set, binding, reflection);
            }
            else if (kind == slang::TypeReflection::Kind::Resource)
            {
                auto shape = type->getResourceShape();
                auto baseShape = shape & SLANG_RESOURCE_BASE_SHAPE_MASK;

                if (baseShape == SLANG_STRUCTURED_BUFFER ||
                    baseShape == SLANG_BYTE_ADDRESS_BUFFER)
                {
                    extractStructuredBuffer(param, type, set, binding, reflection);
                }
                else
                {
                    extractTexture(param, type, set, binding, reflection);
                }
            }
            else if (kind == slang::TypeReflection::Kind::SamplerState)
            {
                extractSampler(param, set, binding, reflection);
            }
        }
    }

    void ShaderCompiler::extractConstantBuffer(
        slang::VariableLayoutReflection* param,
        slang::TypeReflection* type,
        slang::TypeLayoutReflection* typeLayout,
        uint32_t set,
        uint32_t binding,
        ShaderReflection& reflection)
    {
        ShaderReflection::Resource resource;
        resource.name = param->getName() ? param->getName() : "";
        resource.type = ShaderReflection::ResourceType::ConstantBuffer;
        resource.set = set;
        resource.binding = binding;
        resource.totalSize = static_cast<uint32_t>(typeLayout->getSize());

        auto* elementType = type->getElementType();
        auto* elementTypeLayout = typeLayout->getElementTypeLayout();

        if (elementType && elementTypeLayout)
        {
            resource.members = extractMembers(elementType, elementTypeLayout);
        }

        reflection.resources.push_back(std::move(resource));
    }

    void ShaderCompiler::extractStructuredBuffer(
        slang::VariableLayoutReflection* param,
        slang::TypeReflection* type,
        const uint32_t set,
        const uint32_t binding,
        ShaderReflection& reflection)
    {
        ShaderReflection::Resource resource;
        resource.name = param->getName() ? param->getName() : "";
        resource.type = ShaderReflection::ResourceType::StorageBuffer;
        resource.set = set;
        resource.binding = binding;

        const auto access = type->getResourceAccess();
        if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
            resource.access = ResourceAccess::ReadWrite;
        else
            resource.access = ResourceAccess::ReadOnly;

        reflection.resources.push_back(std::move(resource));
    }

    void ShaderCompiler::extractTexture(
        slang::VariableLayoutReflection* param,
        slang::TypeReflection* type,
        uint32_t set,
        uint32_t binding,
        ShaderReflection& reflection)
    {
        auto access = type->getResourceAccess();
        auto shape = type->getResourceShape();

        ShaderReflection::Resource resource;
        resource.name = param->getName() ? param->getName() : "";
        resource.set = set;
        resource.binding = binding;
        resource.dimension = convertTextureDimension(shape);

        if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
        {
            resource.type = ShaderReflection::ResourceType::StorageImage;
            resource.access = ResourceAccess::ReadWrite;
        }
        else
        {
            resource.type = ShaderReflection::ResourceType::Texture;
            resource.access = ResourceAccess::ReadOnly;
        }

        reflection.resources.push_back(std::move(resource));
    }

    void ShaderCompiler::extractSampler(
        slang::VariableLayoutReflection* param,
        const uint32_t set,
        const uint32_t binding,
        ShaderReflection& reflection)
    {
        ShaderReflection::Resource resource;
        resource.name = param->getName() ? param->getName() : "";
        resource.type = ShaderReflection::ResourceType::Sampler;
        resource.set = set;
        resource.binding = binding;

        reflection.resources.push_back(std::move(resource));
    }

    std::vector<ShaderReflection::Member> ShaderCompiler::extractMembers(
        slang::TypeReflection* structType,
        slang::TypeLayoutReflection* structTypeLayout)
    {
        std::vector<ShaderReflection::Member> members;

        uint32_t fieldCount = structType->getFieldCount();
        for (uint32_t f = 0; f < fieldCount; ++f)
        {
            auto* field = structType->getFieldByIndex(f);
            auto* fieldLayout = structTypeLayout->getFieldByIndex(f);
            auto* fieldType = field->getType();
            auto* fieldTypeLayout = fieldLayout->getTypeLayout();

            ShaderReflection::Member member;
            member.name = field->getName() ? field->getName() : "";
            member.offset = static_cast<uint32_t>(fieldLayout->getOffset());
            member.size = static_cast<uint32_t>(fieldTypeLayout->getSize());
            member.type = convertDataType(fieldType);

            if (fieldType->getKind() == slang::TypeReflection::Kind::Array)
            {
                member.arrayCount = fieldType->getElementCount();
                auto* elementType = fieldType->getElementType();
                member.type = convertDataType(elementType);

                if (elementType->getKind() == slang::TypeReflection::Kind::Struct)
                {
                    auto* elementTypeLayout = fieldTypeLayout->getElementTypeLayout();
                    member.members = extractMembers(elementType, elementTypeLayout);
                }
            }
            else if (fieldType->getKind() == slang::TypeReflection::Kind::Struct)
            {
                member.members = extractMembers(fieldType, fieldTypeLayout);
            }

            members.push_back(std::move(member));
        }

        return members;
    }

    ShaderReflection::DataType ShaderCompiler::convertDataType(slang::TypeReflection* type)
    {
        auto kind = type->getKind();

        if (kind == slang::TypeReflection::Kind::Struct)
            return ShaderReflection::DataType::Struct;

        if (kind == slang::TypeReflection::Kind::Vector)
        {
            auto count = type->getElementCount();
            auto scalar = type->getElementType()->getScalarType();

            if (scalar == slang::TypeReflection::ScalarType::Float32)
            {
                switch (count)
                {
                    case 2: return ShaderReflection::DataType::Float2;
                    case 3: return ShaderReflection::DataType::Float3;
                    case 4: return ShaderReflection::DataType::Float4;
                    default: return ShaderReflection::DataType::Float;
                }
            }
            if (scalar == slang::TypeReflection::ScalarType::Int32)
            {
                switch (count)
                {
                    case 2: return ShaderReflection::DataType::Int2;
                    case 3: return ShaderReflection::DataType::Int3;
                    case 4: return ShaderReflection::DataType::Int4;
                    default: return ShaderReflection::DataType::Int;
                }
            }
            if (scalar == slang::TypeReflection::ScalarType::UInt32)
            {
                switch (count)
                {
                    case 2: return ShaderReflection::DataType::UInt2;
                    case 3: return ShaderReflection::DataType::UInt3;
                    case 4: return ShaderReflection::DataType::UInt4;
                    default: return ShaderReflection::DataType::UInt;
                }
            }
        }

        if (kind == slang::TypeReflection::Kind::Matrix)
        {
            auto rows = type->getRowCount();
            auto cols = type->getColumnCount();
            if (rows == 3 && cols == 3) return ShaderReflection::DataType::Mat3;
            if (rows == 4 && cols == 4) return ShaderReflection::DataType::Mat4;
        }

        if (kind == slang::TypeReflection::Kind::Scalar)
        {
            auto scalar = type->getScalarType();
            if (scalar == slang::TypeReflection::ScalarType::Float32)
                return ShaderReflection::DataType::Float;
            if (scalar == slang::TypeReflection::ScalarType::Int32)
                return ShaderReflection::DataType::Int;
            if (scalar == slang::TypeReflection::ScalarType::UInt32)
                return ShaderReflection::DataType::UInt;
        }

        return ShaderReflection::DataType::Unknown;
    }

    TextureType ShaderCompiler::convertTextureDimension(const SlangResourceShape shape)
    {
        const auto baseShape = shape & SLANG_RESOURCE_BASE_SHAPE_MASK;
        const bool isArray = (shape & SLANG_TEXTURE_ARRAY_FLAG) != 0;
        const bool isMS = (shape & SLANG_TEXTURE_MULTISAMPLE_FLAG) != 0;

        switch (baseShape)
        {
            case SLANG_TEXTURE_1D:
                return // isArray
                    // ? TextureType::Texture1DArray :
                    TextureType::Texture1D;
            case SLANG_TEXTURE_2D:
                if (isMS) return TextureType::Texture2D; // should be TextureType::Texture2DMS
                return isArray
                    ? TextureType::Texture2DArray
                    : TextureType::Texture2D;
            case SLANG_TEXTURE_3D:
                return TextureType::Texture3D;
            case SLANG_TEXTURE_CUBE:
                return isArray
                    ? TextureType::TextureCubeArray
                    : TextureType::TextureCube;
            // case SLANG_TEXTURE_BUFFER:
            //     return TextureType::Buffer;
            default:
                return TextureType::Unknown;
        }
    }

    ShaderStage ShaderCompiler::convertStage(const SlangStage stage)
    {
        switch (stage)
        {
            case SLANG_STAGE_VERTEX:   return ShaderStage::Vertex;
            case SLANG_STAGE_FRAGMENT: return ShaderStage::Fragment;
            case SLANG_STAGE_COMPUTE:  return ShaderStage::Compute;
            case SLANG_STAGE_GEOMETRY: return ShaderStage::Geometry;
            default: return ShaderStage::None;
        }
    }
}
