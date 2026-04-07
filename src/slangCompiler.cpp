#include "slangCompiler.h"

#include <slang-com-ptr.h>
#include <slang.h>

namespace urhi
{
    ShaderSet SlangCompiler::compile(const SlangCompileDesc &desc)
    {
        return SlangCompiler().compileImpl(desc);
    }

    ShaderSet SlangCompiler::compileImpl(const SlangCompileDesc &desc)
    {
        createSession(desc);
        compileAndLink(desc);

        slang::ProgramLayout* programLayout = m_linked->getLayout();

        std::vector<ShaderEntryPoint> result;

        for (size_t i = 0; i < m_entryPoints.size(); i++)
        {
            auto spriv = extractSpirv(i);
            slang::EntryPointReflection* epReflection = programLayout->getEntryPointByIndex(i);
            SlangStage slangStage = epReflection->getStage();

            auto reflection = extractReflection(programLayout, i);

            ShaderEntryPoint entryPoint;
            entryPoint.name = m_entryPoints[i]->getLayout()->getEntryPointByIndex(0)->getName();
            entryPoint.stage = convertStage(slangStage);
            entryPoint.reflection = reflection;
            entryPoint.spirv = std::move(spriv);

            result.push_back(std::move(entryPoint));
        }


        return ShaderSet(result);
    }


    void SlangCompiler::compileAndLink(const SlangCompileDesc &desc)
    {
        std::vector<slang::IModule*> modules;
        Slang::ComPtr<slang::IBlob> diagnostics; // TODO use to check errors

        for (auto& m : desc.modules)
        {
            auto* module = m_session->loadModuleFromSourceString(
                m.name.c_str(),
                m.path.c_str(),
                m.source.c_str(),
                diagnostics.writeRef());

            modules.push_back(module);
        }

        if (desc.entryPoints.empty())
        {
            for (auto* module : modules)
            {
                const SlangInt epCount = module->getDefinedEntryPointCount();
                for (SlangInt32 i = 0; i < epCount; i++)
                {
                    Slang::ComPtr<slang::IEntryPoint> ep;
                    module->getDefinedEntryPoint(i, ep.writeRef());

                    if (ep)
                    {
                        m_entryPoints.push_back(ep);
                    }
                }
            }
        }
        else
        {
            for (auto& name : desc.entryPoints)
            {
                for (auto* module : modules)
                {
                    Slang::ComPtr<slang::IEntryPoint> ep;
                    if (SLANG_SUCCEEDED(module->findEntryPointByName(name.c_str(), ep.writeRef())))
                    {
                        m_entryPoints.push_back(ep);
                        break;
                    }
                }
            }
        }

        std::vector<slang::IComponentType*> components;

        for (auto* m : modules)
            components.push_back(m);

        for (auto& ep : m_entryPoints)
            components.push_back(ep);

        Slang::ComPtr<slang::IComponentType> composed;
        m_session->createCompositeComponentType(
            components.data(),
            static_cast<int64_t>(components.size()),
            composed.writeRef(),
            diagnostics.writeRef());

        if (!desc.typeSpecializations.empty())
        {
            Slang::ComPtr<slang::IComponentType> specialized;

            std::vector<slang::SpecializationArg> args;
            args.reserve(desc.typeSpecializations.size());

            for (const auto& typeName : desc.typeSpecializations)
            {
                slang::TypeReflection* concreteType = nullptr;

                // Find the concrete type in loaded modules
                for (auto* module : modules)
                {
                    concreteType = module->getLayout()->findTypeByName(typeName.c_str());
                    if (concreteType)
                        break;
                }

                if (concreteType)
                {
                    slang::SpecializationArg arg{};
                    arg.kind = slang::SpecializationArg::Kind::Type;
                    arg.type = concreteType;
                    args.push_back(arg);
                }
            }

            if (!args.empty())
            {
                composed->specialize(
                    args.data(),
                    static_cast<SlangInt>(args.size()),
                    specialized.writeRef(),
                    diagnostics.writeRef());

                if (specialized)
                {
                    specialized->link(m_linked.writeRef(), diagnostics.writeRef());
                    return;
                }
            }
        }

        composed->link(m_linked.writeRef(), diagnostics.writeRef());
    }

    void SlangCompiler::createSession(const SlangCompileDesc& desc)
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

        globalSession->createSession(sessionDesc, m_session.writeRef());
    }

    std::vector<uint32_t> SlangCompiler::extractSpirv(const uint32_t index) const
    {
        Slang::ComPtr<slang::IBlob> spirvBlob;
        Slang::ComPtr<slang::IBlob> diagnostics; // TODO use to check errors

        m_linked->getEntryPointCode(
            index,
            0,
            spirvBlob.writeRef(),
            diagnostics.writeRef()
        );

        std::vector<uint32_t> spirv;

        const auto* spirvData = static_cast<const uint32_t*>(spirvBlob->getBufferPointer());
        const size_t spirvWordCount = spirvBlob->getBufferSize() / sizeof(uint32_t);
        spirv.assign(spirvData, spirvData + spirvWordCount);

        return spirv;
    }

    ShaderReflection SlangCompiler::extractReflection(slang::ProgramLayout* layout, const size_t entryPointIndex)
    {
        ShaderReflection result;

        slang::EntryPointReflection* ep = layout->getEntryPointByIndex(entryPointIndex);
        const SlangStage stage = ep->getStage();

        extractPushConstants(layout, result);

        const uint32_t globalParamCount = layout->getParameterCount();
        for (uint32_t i = 0; i < globalParamCount; i++)
        {
            slang::VariableLayoutReflection* param = layout->getParameterByIndex(i);

            extractBinding(param, result);
        }

        uint32_t epParamCount = ep->getParameterCount();
        for (uint32_t p = 0; p < epParamCount; p++)
        {
            slang::VariableLayoutReflection* param = ep->getParameterByIndex(p);
            const slang::ParameterCategory category = param->getCategory();

            if (category == slang::ParameterCategory::VaryingInput ||
                category == slang::ParameterCategory::Mixed)
            {
                if (stage == SLANG_STAGE_VERTEX)
                {
                    extractVertexInput(param, result);
                }
            }
            else if(category != slang::ParameterCategory::VaryingOutput)
            {
                extractBinding(param, result);
            }
        }

        if (stage == SLANG_STAGE_COMPUTE)
        {
            SlangUInt size[3];
            ep->getComputeThreadGroupSize(3, size);
            result.computeInfo = {
                static_cast<uint32_t>(size[0]),
                static_cast<uint32_t>(size[1]),
                static_cast<uint32_t>(size[2])
            };
        }

        return result;
    }

    void SlangCompiler::extractVertexInput(slang::VariableLayoutReflection* param, ShaderReflection& result)
    {
        slang::TypeLayoutReflection* typeLayout = param->getTypeLayout();
        const auto bindingSlot = static_cast<uint32_t>(result.vertexInput.bindings.size());

        const uint32_t paramBaseLocation = param->getOffset(SLANG_PARAMETER_CATEGORY_VARYING_INPUT);

        if (typeLayout->getKind() == slang::TypeReflection::Kind::Struct)
        {
            uint32_t currentOffset = 0;
            const uint32_t fieldCount = typeLayout->getFieldCount();

            for (uint32_t f = 0; f < fieldCount; f++)
            {
                slang::VariableLayoutReflection* field = typeLayout->getFieldByIndex(f);
                slang::TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();

                ShaderReflection::VertexAttribute attr;
                attr.binding = bindingSlot;
                attr.location = paramBaseLocation + static_cast<uint32_t>(field->getOffset(SLANG_PARAMETER_CATEGORY_VARYING_INPUT));
                attr.offset = currentOffset;
                attr.type = convertDataType(fieldTypeLayout);
                attr.name = field->getName();
                result.vertexInput.attributes.push_back(attr);

                currentOffset += getDataTypeByteSize(attr.type);
            }

            ShaderReflection::VertexBinding binding;
            binding.binding = bindingSlot;
            binding.stride = currentOffset;
            result.vertexInput.bindings.push_back(binding);
        }
        else
        {
            ShaderReflection::VertexAttribute attr;
            attr.binding = bindingSlot;
            attr.location = static_cast<uint32_t>(param->getOffset(slang::ParameterCategory::VaryingInput));
            attr.offset = 0;
            attr.type = convertDataType(typeLayout);
            attr.name = param->getName();
            result.vertexInput.attributes.push_back(attr);

            ShaderReflection::VertexBinding binding;
            binding.binding = bindingSlot;
            binding.stride =  getDataTypeByteSize(attr.type);
            result.vertexInput.bindings.push_back(binding);
        }
    }

    void SlangCompiler::extractBinding(slang::VariableLayoutReflection* param, ShaderReflection& result)
    {
        if (param->getCategory() == slang::ParameterCategory::PushConstantBuffer)
            return;

        slang::TypeLayoutReflection* typeLayout = param->getTypeLayout();
        slang::TypeReflection::Kind kind = typeLayout->getKind();

        ShaderReflection::Resource resource;
        bool isBinding = true;

        switch (kind)
        {
            case slang::TypeReflection::Kind::ConstantBuffer:
            {
                resource.type = ShaderReflection::ResourceType::ConstantBuffer;

                slang::TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
                if (elementType)
                {
                    resource.totalSize = static_cast<uint32_t>(elementType->getSize());
                    extractMembers(elementType, resource.members);
                }

                break;
            }
            case slang::TypeReflection::Kind::ParameterBlock:
            {
                extractParameterBlock(param, typeLayout, result);
                isBinding = false;
                break;
            }
            case slang::TypeReflection::Kind::Resource:
            {
                const SlangResourceShape baseShape = typeLayout->getResourceShape();
                const SlangResourceAccess access = typeLayout->getResourceAccess();

                if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                    resource.access = ResourceAccess::ReadWrite;
                else if (access == SLANG_RESOURCE_ACCESS_WRITE)
                    resource.access = ResourceAccess::WriteOnly;
                else
                    resource.access = ResourceAccess::ReadOnly;

                if (baseShape == SLANG_STRUCTURED_BUFFER)
                {
                    resource.type = ShaderReflection::ResourceType::StorageBuffer;

                    slang::TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
                    if (elementType)
                    {
                        resource.totalSize = static_cast<uint32_t>(elementType->getSize());
                        extractMembers(elementType, resource.members);
                    }
                }
                else if (baseShape == SLANG_BYTE_ADDRESS_BUFFER)
                {
                    resource.type = ShaderReflection::ResourceType::StorageBuffer;
                }
                else if (baseShape >= SLANG_TEXTURE_1D && baseShape <= SLANG_TEXTURE_CUBE)
                {
                    if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                    {
                        resource.type = ShaderReflection::ResourceType::StorageImage;
                    }
                    else
                    {
                        resource.type = ShaderReflection::ResourceType::Texture;
                    }

                    resource.dimension = convertTextureType(baseShape);
                }else
                {
                    resource.type = ShaderReflection::ResourceType::StorageBuffer;
                }

                break;
            }

            case slang::TypeReflection::Kind::SamplerState:
                resource.type = ShaderReflection::ResourceType::Sampler;
                break;
            case slang::TypeReflection::Kind::Array:
            {
                slang::TypeLayoutReflection* elementTypeLayout = typeLayout->getElementTypeLayout();
                const auto arrayCount = static_cast<uint32_t>(typeLayout->getElementCount());

                const slang::TypeReflection::Kind elementKind = elementTypeLayout->getKind();

                if (elementKind == slang::TypeReflection::Kind::Resource ||
                    elementKind == slang::TypeReflection::Kind::SamplerState)
                {
                    resource.count = arrayCount;

                    if (elementKind == slang::TypeReflection::Kind::SamplerState)
                    {
                        resource.type = ShaderReflection::ResourceType::Sampler;
                    }
                    else
                    {
                        // Get access and shape from element type
                        const auto baseShape = static_cast<SlangResourceShape>(elementTypeLayout->getResourceShape() & SLANG_RESOURCE_BASE_SHAPE_MASK);
                        const SlangResourceAccess access = elementTypeLayout->getResourceAccess();

                        if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                            resource.access = ResourceAccess::ReadWrite;
                        else if (access == SLANG_RESOURCE_ACCESS_WRITE)
                            resource.access = ResourceAccess::WriteOnly;
                        else
                            resource.access = ResourceAccess::ReadOnly;

                        if (baseShape >= SLANG_TEXTURE_1D && baseShape <= SLANG_TEXTURE_CUBE)
                        {
                            if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                                resource.type = ShaderReflection::ResourceType::StorageImage;
                            else
                                resource.type = ShaderReflection::ResourceType::Texture;

                            resource.dimension = convertTextureType(baseShape);
                        }
                        else
                        {
                            resource.type = ShaderReflection::ResourceType::StorageBuffer;
                        }
                    }
                }
                else
                {
                    isBinding = false;
                }
                break;
            }
            default:
                isBinding = false;
                break;
        }

        if (isBinding)
        {
            resource.set = param->getBindingSpace();
            resource.binding = param->getBindingIndex();
            resource.name = param->getName();

            slang::TypeReflection* type = typeLayout->getType();
            if (type->getKind() == slang::TypeReflection::Kind::Array && resource.count == 1)
            {
                resource.count = static_cast<uint32_t>(type->getElementCount());
            }

            result.resources.push_back(resource);
        }
    }

    void SlangCompiler::extractMembers(slang::TypeLayoutReflection* typeLayout, std::vector<ShaderReflection::Member>& members)
    {
        if (typeLayout->getKind() != slang::TypeReflection::Kind::Struct)
            return;

        const uint32_t fieldCount = typeLayout->getFieldCount();
        for (uint32_t i = 0; i < fieldCount; i++)
        {
            slang::VariableLayoutReflection* field = typeLayout->getFieldByIndex(i);
            slang::TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();
            slang::TypeReflection* fieldType = fieldTypeLayout->getType();

            ShaderReflection::Member member;
            member.name = field->getName();
            member.offset = static_cast<uint32_t>(field->getOffset());
            member.size = static_cast<uint32_t>(fieldTypeLayout->getSize());
            member.arrayCount = 1;

            if (fieldType->getKind() == slang::TypeReflection::Kind::Array)
            {
                member.arrayCount = static_cast<uint32_t>(fieldType->getElementCount());
                slang::TypeLayoutReflection* elementLayout = fieldTypeLayout->getElementTypeLayout();
                member.type = convertDataType(elementLayout);

                // If array of structs, recurse
                if (elementLayout->getKind() == slang::TypeReflection::Kind::Struct)
                {
                    member.type = ShaderReflection::DataType::Struct;
                    extractMembers(elementLayout, member.members);
                }
            }
            else if (fieldType->getKind() == slang::TypeReflection::Kind::Struct)
            {
                member.type = ShaderReflection::DataType::Struct;
                extractMembers(fieldTypeLayout, member.members);
            }
            else if (fieldType->getKind() == slang::TypeReflection::Kind::Matrix)
            {
                const uint32_t rows = fieldType->getRowCount();
                const uint32_t cols = fieldType->getColumnCount();

                if (rows == 4 && cols == 4)
                    member.type = ShaderReflection::DataType::Mat4;
                else if (rows == 3 && cols == 3)
                    member.type = ShaderReflection::DataType::Mat3;
                else
                    member.type = ShaderReflection::DataType::Unknown;
            }
            else
            {
                member.type = convertDataType(fieldTypeLayout);
            }

            members.push_back(member);
        }
    }

    void SlangCompiler::extractPushConstants(slang::ProgramLayout* layout, ShaderReflection& result)
    {
        const uint32_t paramCount = layout->getParameterCount();
        for (uint32_t i = 0; i < paramCount; i++)
        {
            slang::VariableLayoutReflection* param = layout->getParameterByIndex(i);
            slang::TypeLayoutReflection* typeLayout = param->getTypeLayout();

            if (param->getCategory() == slang::ParameterCategory::PushConstantBuffer)
            {
                ShaderReflection::PushConstant pc;
                pc.name = param->getName();
                pc.offset = 0;

                // Get the inner element type for the actual size
                slang::TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
                if (elementType)
                {
                    pc.size = static_cast<uint32_t>(elementType->getSize());
                    extractMembers(elementType, pc.members); // if you have members on PushConstant
                }
                else
                {
                    pc.size = static_cast<uint32_t>(typeLayout->getSize());
                }

                result.pushConstant = pc;
                return;
            }
        }
    }

    void SlangCompiler::extractParameterBlock(slang::VariableLayoutReflection* param, slang::TypeLayoutReflection* typeLayout, ShaderReflection& result)
    {
        slang::TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
        if (!elementType)
            return;

        uint32_t innerSet = static_cast<uint32_t>(param->getOffset(SLANG_PARAMETER_CATEGORY_SUB_ELEMENT_REGISTER_SPACE));

        if (innerSet == 0xFFFFFFFF) {
            innerSet = param->getBindingSpace();
        }

        const std::string blockName = param->getName();

        const size_t uniformSize = elementType->getSize(SLANG_PARAMETER_CATEGORY_UNIFORM);
        if (uniformSize > 0)
        {
            ShaderReflection::Resource uniformResource;
            uniformResource.name = blockName;
            uniformResource.type = ShaderReflection::ResourceType::ConstantBuffer;
            uniformResource.set = innerSet;
            uniformResource.binding = 0;
            uniformResource.totalSize = static_cast<uint32_t>(uniformSize);

            extractUniformMembers(elementType, uniformResource.members);

            if (!uniformResource.members.empty())
            {
                result.resources.push_back(uniformResource);
            }
        }

        extractParameterBlockResources(elementType, innerSet, 0, blockName, result);
    }

    void SlangCompiler::extractUniformMembers(slang::TypeLayoutReflection* typeLayout, std::vector<ShaderReflection::Member>& members)
    {
        if (typeLayout->getKind() != slang::TypeReflection::Kind::Struct)
            return;

        const uint32_t fieldCount = typeLayout->getFieldCount();
        for (uint32_t i = 0; i < fieldCount; i++)
        {
            slang::VariableLayoutReflection* field = typeLayout->getFieldByIndex(i);
            slang::TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();
            const slang::TypeReflection::Kind fieldKind = fieldTypeLayout->getKind();

            if (fieldKind == slang::TypeReflection::Kind::Resource ||
                fieldKind == slang::TypeReflection::Kind::SamplerState ||
                fieldKind == slang::TypeReflection::Kind::ConstantBuffer ||
                fieldKind == slang::TypeReflection::Kind::ParameterBlock)
            {
                continue;
            }

            slang::TypeReflection* fieldType = fieldTypeLayout->getType();

            ShaderReflection::Member member;
            member.name = field->getName();
            member.offset = static_cast<uint32_t>(field->getOffset(SLANG_PARAMETER_CATEGORY_UNIFORM));
            member.size = static_cast<uint32_t>(fieldTypeLayout->getSize(SLANG_PARAMETER_CATEGORY_UNIFORM));
            member.arrayCount = 1;

            if (fieldType->getKind() == slang::TypeReflection::Kind::Array)
            {
                member.arrayCount = static_cast<uint32_t>(fieldType->getElementCount());
                slang::TypeLayoutReflection* elementLayout = fieldTypeLayout->getElementTypeLayout();

                if (elementLayout->getKind() == slang::TypeReflection::Kind::Struct)
                {
                    member.type = ShaderReflection::DataType::Struct;
                    extractUniformMembers(elementLayout, member.members);
                }
                else
                {
                    member.type = convertDataType(elementLayout);
                }
            }
            else if (fieldType->getKind() == slang::TypeReflection::Kind::Struct)
            {
                member.type = ShaderReflection::DataType::Struct;
                extractUniformMembers(fieldTypeLayout, member.members);
            }
            else if (fieldType->getKind() == slang::TypeReflection::Kind::Matrix)
            {
                uint32_t rows = fieldType->getRowCount();
                uint32_t cols = fieldType->getColumnCount();

                if (rows == 4 && cols == 4)
                    member.type = ShaderReflection::DataType::Mat4;
                else if (rows == 3 && cols == 3)
                    member.type = ShaderReflection::DataType::Mat3;
                else
                    member.type = ShaderReflection::DataType::Unknown;
            }
            else
            {
                member.type = convertDataType(fieldTypeLayout);
            }

            if (member.size > 0)
            {
                members.push_back(member);
            }
        }
    }

     void SlangCompiler::extractParameterBlockResources(slang::TypeLayoutReflection* typeLayout, uint32_t set, uint32_t baseBinding, const std::string& prefix, ShaderReflection& result)
    {
        if (typeLayout->getKind() != slang::TypeReflection::Kind::Struct)
            return;

        uint32_t fieldCount = typeLayout->getFieldCount();
        for (uint32_t i = 0; i < fieldCount; i++)
        {
            slang::VariableLayoutReflection* field = typeLayout->getFieldByIndex(i);

            if (field->getCategory() == slang::ParameterCategory::Uniform)
                continue;

            slang::TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();
            slang::TypeReflection::Kind fieldKind = fieldTypeLayout->getKind();

            const std::string qualifiedName = prefix + "." + field->getName();

            // 4. Calculate Absolute Binding for Vulkan (Base + Relative)
            // e.g. albedoMap is the 2nd thing in the block, so it gets offset 1. 0 + 1 = 1.
            auto bindingOffset = static_cast<uint32_t>(field->getOffset(SLANG_PARAMETER_CATEGORY_DESCRIPTOR_TABLE_SLOT));
            uint32_t absoluteBinding = baseBinding + bindingOffset;

            if (fieldKind == slang::TypeReflection::Kind::Resource)
            {
                ShaderReflection::Resource resource;
                resource.name = qualifiedName;
                resource.set = set;                 // <--- Uses innerSet (1)
                resource.binding = absoluteBinding; // <--- Uses absolute (1)
                resource.count = 1;

                auto baseShape = static_cast<SlangResourceShape>(fieldTypeLayout->getResourceShape() & SLANG_RESOURCE_BASE_SHAPE_MASK);
                SlangResourceAccess access = fieldTypeLayout->getResourceAccess();

                if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                    resource.access = ResourceAccess::ReadWrite;
                else if (access == SLANG_RESOURCE_ACCESS_WRITE)
                    resource.access = ResourceAccess::WriteOnly;
                else
                    resource.access = ResourceAccess::ReadOnly;

                if (baseShape == SLANG_STRUCTURED_BUFFER)
                {
                    resource.type = ShaderReflection::ResourceType::StorageBuffer;
                    slang::TypeLayoutReflection* elementType = fieldTypeLayout->getElementTypeLayout();
                    if (elementType)
                    {
                        resource.totalSize = static_cast<uint32_t>(elementType->getSize());
                        extractMembers(elementType, resource.members);
                    }
                }
                else if (baseShape == SLANG_BYTE_ADDRESS_BUFFER)
                {
                    resource.type = ShaderReflection::ResourceType::StorageBuffer;
                }
                else if (baseShape >= SLANG_TEXTURE_1D && baseShape <= SLANG_TEXTURE_CUBE)
                {
                    if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                        resource.type = ShaderReflection::ResourceType::StorageImage;
                    else
                        resource.type = ShaderReflection::ResourceType::Texture;

                    resource.dimension = convertTextureType(baseShape);
                }
                else
                {
                    resource.type = ShaderReflection::ResourceType::StorageBuffer;
                }

                result.resources.push_back(resource);
            }
            else if (fieldKind == slang::TypeReflection::Kind::SamplerState)
            {
                ShaderReflection::Resource resource;
                resource.name = qualifiedName;
                resource.type = ShaderReflection::ResourceType::Sampler;
                resource.set = set;                 // <--- Uses innerSet (1)
                resource.binding = absoluteBinding; // <--- Uses absolute (2, 3, etc)
                resource.count = 1;
                result.resources.push_back(resource);
            }
            else if (fieldKind == slang::TypeReflection::Kind::Array)
            {
                slang::TypeLayoutReflection* elementLayout = fieldTypeLayout->getElementTypeLayout();
                slang::TypeReflection::Kind elementKind = elementLayout->getKind();
                auto arrayCount = static_cast<uint32_t>(fieldTypeLayout->getElementCount());

                if (elementKind == slang::TypeReflection::Kind::Resource ||
                    elementKind == slang::TypeReflection::Kind::SamplerState)
                {
                    ShaderReflection::Resource resource;
                    resource.name = qualifiedName;
                    resource.set = set;
                    resource.binding = absoluteBinding;
                    resource.count = arrayCount;

                    if (elementKind == slang::TypeReflection::Kind::SamplerState)
                    {
                        resource.type = ShaderReflection::ResourceType::Sampler;
                    }
                    else
                    {
                        auto baseShape = static_cast<SlangResourceShape>(elementLayout->getResourceShape() & SLANG_RESOURCE_BASE_SHAPE_MASK);
                        SlangResourceAccess access = elementLayout->getResourceAccess();

                        if (baseShape >= SLANG_TEXTURE_1D && baseShape <= SLANG_TEXTURE_CUBE)
                        {
                            if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                                resource.type = ShaderReflection::ResourceType::StorageImage;
                            else
                                resource.type = ShaderReflection::ResourceType::Texture;

                            resource.dimension = convertTextureType(baseShape);
                        }
                        else
                        {
                            resource.type = ShaderReflection::ResourceType::StorageBuffer;
                        }
                    }

                    result.resources.push_back(resource);
                }
            }
            else if (fieldKind == slang::TypeReflection::Kind::Struct)
            {
                // Recurse into nested structs
                extractParameterBlockResources(fieldTypeLayout, set, absoluteBinding, qualifiedName, result);
            }
        }
    }

    ShaderReflection::DataType SlangCompiler::convertDataType(slang::TypeLayoutReflection* typeLayout)
    {
        slang::TypeReflection* type = typeLayout->getType();
        slang::TypeReflection::ScalarType scalarType;
        int elementCount = 1;

        if (type->getKind() == slang::TypeReflection::Kind::Vector)
        {
            elementCount = static_cast<int>(type->getElementCount());
            scalarType = type->getElementType()->getScalarType();
        }
        else
        {
            scalarType = type->getScalarType();
        }

        if (scalarType == slang::TypeReflection::ScalarType::Float32)
        {
            switch (elementCount)
            {
                case 1: return ShaderReflection::DataType::Float;
                case 2: return ShaderReflection::DataType::Float2;
                case 3: return ShaderReflection::DataType::Float3;
                case 4: return ShaderReflection::DataType::Float4;
                default: break;
            }
        }
        else if (scalarType == slang::TypeReflection::ScalarType::Int32)
        {
            switch (elementCount)
            {
                case 1: return ShaderReflection::DataType::Int;
                case 2: return ShaderReflection::DataType::Int2;
                case 3: return ShaderReflection::DataType::Int3;
                case 4: return ShaderReflection::DataType::Int4;
                default: break;
            }
        }
        else if (scalarType == slang::TypeReflection::ScalarType::UInt32)
        {
            switch (elementCount)
            {
                case 1: return ShaderReflection::DataType::UInt;
                case 2: return ShaderReflection::DataType::UInt2;
                case 3: return ShaderReflection::DataType::UInt3;
                case 4: return ShaderReflection::DataType::UInt4;
                default: break;
            }
        }

        return ShaderReflection::DataType::Unknown;
    }

    ShaderStage SlangCompiler::convertStage(const SlangStage slangStage)
    {
        switch (slangStage)
        {
            case SLANG_STAGE_VERTEX:   return ShaderStage::Vertex;
            case SLANG_STAGE_FRAGMENT: return ShaderStage::Fragment;
            case SLANG_STAGE_COMPUTE:  return ShaderStage::Compute;
            case SLANG_STAGE_GEOMETRY: return ShaderStage::Geometry;
            // case SLANG_STAGE_HULL:     return ShaderStage::TessControl;
            // case SLANG_STAGE_DOMAIN:   return ShaderStage::TessEvaluation;
            default:                   return ShaderStage::Vertex;
        }
    }

    TextureType SlangCompiler::convertTextureType(const SlangResourceShape shape)
    {
        const unsigned baseShape = shape & SLANG_RESOURCE_BASE_SHAPE_MASK;

        switch (baseShape)
        {
            case SLANG_TEXTURE_1D:   return TextureType::Texture1D;
            case SLANG_TEXTURE_2D:   return TextureType::Texture2D;
            case SLANG_TEXTURE_3D:   return TextureType::Texture3D;
            case SLANG_TEXTURE_CUBE: return TextureType::TextureCube;
            default:                 return TextureType::Texture2D;
        }
    }

    uint32_t SlangCompiler::getDataTypeByteSize(const ShaderReflection::DataType type)
    {
        switch (type)
        {
            case ShaderReflection::DataType::Float:
            case ShaderReflection::DataType::Int:
            case ShaderReflection::DataType::UInt:
                return 4;

            case ShaderReflection::DataType::Float2:
            case ShaderReflection::DataType::Int2:
            case ShaderReflection::DataType::UInt2:
                return 8;

            case ShaderReflection::DataType::Float3:
            case ShaderReflection::DataType::Int3:
            case ShaderReflection::DataType::UInt3:
                return 12;

            case ShaderReflection::DataType::Float4:
            case ShaderReflection::DataType::Int4:
            case ShaderReflection::DataType::UInt4:
                return 16;

            case ShaderReflection::DataType::Mat3: return 36; // 3x3 * 4 bytes
            case ShaderReflection::DataType::Mat4: return 64; // 4x4 * 4 bytes

            default: return 0;
        }
    }

}
