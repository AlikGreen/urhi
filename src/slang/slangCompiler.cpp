#include "slangCompiler.h"

#include <format>
#include <regex>
#include <slang-com-ptr.h>
#include <slang.h>

#include "memoryBlob.h"
#include "slangConvert.h"
#include <grl/file.h>

#include "descriptions/shaderDesc.h"

namespace urhi
{
    std::expected<SlangModule, std::string> SlangCompiler::compile(const SlangSource &source)
    {
        std::vector<const char*> searchPaths;
        searchPaths.reserve(source.includeDirs.size());
        for (auto& s : source.includeDirs) searchPaths.push_back(s.c_str());

        std::vector<slang::PreprocessorMacroDesc> macros;
        macros.reserve(source.defines.size());

        for (const auto& [name, value] : source.defines)
        {
            slang::PreprocessorMacroDesc m = {};
            m.name  = name.c_str();
            m.value = value.c_str();
            macros.push_back(m);
        }

        slang::SessionDesc sessionDesc = {};
        sessionDesc.targets          = nullptr;
        sessionDesc.targetCount      = 0;
        sessionDesc.searchPaths      = searchPaths.empty() ? nullptr : searchPaths.data();
        sessionDesc.searchPathCount  = static_cast<SlangInt>(searchPaths.size());
        sessionDesc.preprocessorMacros     = macros.empty() ? nullptr : macros.data();
        sessionDesc.preprocessorMacroCount = static_cast<SlangInt>(macros.size());

        Slang::ComPtr<slang::ISession> session;
        getGlobalSession()->createSession(sessionDesc, session.writeRef());

        Slang::ComPtr<slang::IBlob> loadDiags;
        Slang::ComPtr<slang::IModule> module;
        module = session->loadModuleFromSourceString(
            source.path.c_str(),
            source.path.c_str(),
            source.source.c_str(),
            loadDiags.writeRef());

        if (!module)
            return std::unexpected<std::string>(static_cast<const char*>(loadDiags->getBufferPointer()));

        Slang::ComPtr<slang::IBlob> serialized;
        const SlangResult r = module->serialize(serialized.writeRef());
        if (SLANG_FAILED(r) || !serialized)
            return std::unexpected<std::string>("IModule::serialize() failed.\n");

        SlangModule result{};

        result.name = source.path;
        result.path = source.path;
        result.ir.assign(
            static_cast<const uint8_t *>(serialized->getBufferPointer()),
            static_cast<const uint8_t *>(serialized->getBufferPointer()) + serialized->getBufferSize());

        return result;
    }


    std::expected<SlangLinkedModule, std::string> SlangCompiler::link(const SlangLinkDesc& desc)
    {
        const auto session = getSession();
        std::vector<Slang::ComPtr<slang::IModule>> modules;

        for (auto& m : desc.modules)
        {
            auto module = loadModuleFromIR(session, m);
            if (!module.has_value())
                return std::unexpected(module.error());
            modules.push_back(module.value());
        }

        std::vector<Slang::ComPtr<slang::IComponentType>> components;

        for (const auto& mod : modules)
        {
            Slang::ComPtr<slang::IComponentType> modComp;
            mod->queryInterface(slang::IComponentType::getTypeGuid(), reinterpret_cast<void **>(modComp.writeRef()));

            auto specialized = trySpecialize(modules, desc.typeSpecializations, modComp);
            if (!specialized.has_value())
                return std::unexpected(specialized.error());
            components.push_back(specialized.value());
        }

        std::vector<Slang::ComPtr<slang::IEntryPoint>> entryPoints;

        for (const auto& module : modules)
        {
            if (desc.entryPoints.empty())
            {
                const SlangInt epCount = module->getDefinedEntryPointCount();
                for (SlangInt i = 0; i < epCount; i++)
                {
                    Slang::ComPtr<slang::IEntryPoint> ep;
                    module->getDefinedEntryPoint(i, ep.writeRef());
                    if (ep) entryPoints.push_back(ep);
                }
            }
            else
            {
                for (auto& name : desc.entryPoints)
                {
                    Slang::ComPtr<slang::IEntryPoint> ep;
                    if (SLANG_SUCCEEDED(module->findEntryPointByName(name.c_str(), ep.writeRef())))
                        entryPoints.push_back(ep);
                }
            }
        }

        for (const auto& ep : entryPoints)
        {
            Slang::ComPtr<slang::IComponentType> epComp;
            ep->queryInterface(slang::IComponentType::getTypeGuid(), reinterpret_cast<void **>(epComp.writeRef()));
            components.push_back(epComp);
        }

        Slang::ComPtr<slang::IComponentType> composed;
        {
            std::vector<slang::IComponentType*> rawComponents;
            rawComponents.reserve(components.size());
            for (const auto& c : components)
                rawComponents.push_back(c.get());

            Slang::ComPtr<slang::IBlob> diag;
            const SlangResult r = session->createCompositeComponentType(
                rawComponents.data(),
                static_cast<SlangInt>(rawComponents.size()),
                composed.writeRef(),
                diag.writeRef());

            if (SLANG_FAILED(r) || !composed)
            {
                return std::unexpected(static_cast<const char*>(diag->getBufferPointer()));
            }

        }

        auto composedSpecialized = trySpecialize(modules, desc.typeSpecializations, composed);
        if (!composedSpecialized.has_value())
            return std::unexpected(composedSpecialized.error());

        composed = composedSpecialized.value();

        Slang::ComPtr<slang::IComponentType> linked;
        {
            Slang::ComPtr<slang::IBlob> diag;
            SlangResult r = composed->link(linked.writeRef(), diag.writeRef());

            if (SLANG_FAILED(r) || !linked)
            {
                return std::unexpected(static_cast<const char*>(diag->getBufferPointer()));
            }
        }

        SlangLinkedModule linkedModule{};
        linkedModule.session = session;
        linkedModule.entryPoints = entryPoints;
        linkedModule.linkedShader = linked;

        return linkedModule;
    }

    std::expected<ShaderEntryPoint, std::string> SlangCompiler::extract(const SlangLinkedModule &linked, ShaderStage stage, const std::string &name)
    {
        slang::ProgramLayout* programLayout = linked.linkedShader->getLayout();

        int epIndex = -1;
        for(int i = 0; i < linked.entryPoints.size(); i++)
        {
            slang::ProgramLayout* layout = linked.entryPoints[i]->getLayout();

            slang::EntryPointReflection* epRefl = layout->getEntryPointByIndex(0);

            const char* epName  = epRefl->getName();
            const ShaderStage epStage = convertSlangStage(epRefl->getStage());

            if((epName == name || name.empty()) && epStage == stage)
            {
                epIndex = i;
                break;
            }
        }

        if(epIndex < 0)
            return std::unexpected("Entry point with specified stage or name does not exist");


        Slang::ComPtr<slang::IBlob> spirvCode, spirvDiag;
        const auto res = linked.linkedShader->getEntryPointCode(
            epIndex,
            0, // spir-v target
            spirvCode.writeRef(),
            spirvDiag.writeRef());

        if(SLANG_FAILED(res))
            return std::unexpected(static_cast<const char*>(spirvDiag->getBufferPointer()));

        std::vector<uint32_t> spirv;

        const auto* spirvData = static_cast<const uint32_t*>(spirvCode->getBufferPointer());
        const size_t spirvWordCount = spirvCode->getBufferSize() / sizeof(uint32_t);
        spirv.assign(spirvData, spirvData + spirvWordCount);

        const auto reflection = extractReflection(programLayout, epIndex);

        slang::ProgramLayout* layout = linked.entryPoints[epIndex]->getLayout();

        ShaderEntryPoint ep{};
        ep.name = layout->getEntryPointByIndex(0)->getName();
        ep.stage = stage;
        ep.spirvCode = std::move(spirv);
        ep.reflection = reflection;

        return ep;
    }

    slang::IGlobalSession * SlangCompiler::getGlobalSession()
    {
        static Slang::ComPtr<slang::IGlobalSession> s_global;

        if (!s_global)
            createGlobalSession(s_global.writeRef());

        return s_global.get();
    }

    Slang::ComPtr<slang::ISession> SlangCompiler::getSession()
    {
        Slang::ComPtr<slang::ISession> session;

        slang::IGlobalSession* globalSession = getGlobalSession();

        slang::TargetDesc targets[2] = {};
        targets[0].format  = SLANG_SPIRV;
        targets[0].profile = globalSession->findProfile("glsl_460");

        slang::SessionDesc sessionDesc{};
        sessionDesc.targets      = targets;
        sessionDesc.targetCount  = 2;
        sessionDesc.searchPaths = nullptr;
        sessionDesc.searchPathCount = 0;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

        globalSession->createSession(sessionDesc, session.writeRef());
        return session;
    }

    std::expected<Slang::ComPtr<slang::IModule>, std::string> SlangCompiler::loadModuleFromIR(slang::ISession *session, const SlangModule& module)
    {
        Slang::ComPtr<slang::IBlob> slangDiags;
        const Slang::ComPtr<slang::IBlob> irBlob = Slang::ComPtr<slang::IBlob>(new SlangMemoryBlob(module.ir));

        Slang::ComPtr<slang::IModule> slangModule;
        slangModule = session->loadModuleFromIRBlob(
            module.name.c_str(),
            module.path.c_str(),
            irBlob.get(),
            slangDiags.writeRef());

        if (slangModule == nullptr)
        {
            std::string errStr = slangDiags ? static_cast<const char*>(slangDiags->getBufferPointer()) : "";

            if (errStr.find("exists") != std::string::npos || errStr.find("Dictionary") != std::string::npos)
            {
                Slang::ComPtr<slang::IBlob> ignoreDiags;
                slangModule = session->loadModule(module.name.c_str(), ignoreDiags.writeRef());
                if (slangModule != nullptr)
                {
                    return slangModule;
                }
            }
            return std::unexpected<std::string>(errStr);
        }

        return slangModule;
    }

    slang::TypeReflection * SlangCompiler::findType(const std::vector<Slang::ComPtr<slang::IModule>> &modules, const std::string &typeName)
    {
        for (const auto& mod : modules)
        {
            slang::TypeReflection* t = mod->getLayout()->findTypeByName(typeName.c_str());
            if (t) return t;
        }
        return nullptr;
    }

    std::expected<Slang::ComPtr<slang::IComponentType>, std::string> SlangCompiler::trySpecialize(const std::vector<Slang::ComPtr<slang::IModule>>& modules, const std::vector<std::string>& typeSpecializations, Slang::ComPtr<slang::IComponentType> component)
    {
        const SlangInt paramCount = component->getSpecializationParamCount();
        if (paramCount == 0 || typeSpecializations.empty())
            return component;

        std::vector<slang::SpecializationArg> args;
        for (size_t i = 0; i < paramCount && i < typeSpecializations.size(); i++)
        {
            slang::TypeReflection* concreteType = findType(modules, typeSpecializations[i]);
            if (!concreteType)
            {
                return std::unexpected("Could not find type: " + typeSpecializations[i] + "\n");
            }

            slang::SpecializationArg arg{};
            arg.kind = slang::SpecializationArg::Kind::Type;
            arg.type = concreteType;
            args.push_back(arg);
        }

        Slang::ComPtr<slang::IComponentType> specialized;
        Slang::ComPtr<slang::IBlob> diag;
        SlangResult r = component->specialize(
            args.data(),
            static_cast<SlangInt>(args.size()),
            specialized.writeRef(),
            diag.writeRef());

        if (SLANG_FAILED(r) || !specialized)
        {
            return std::unexpected(static_cast<const char*>(diag->getBufferPointer()));
        }

        return specialized;
    }

    ShaderStage SlangCompiler::convertSlangStage(const SlangStage stage)
    {
        switch (stage)
        {
            case SLANG_STAGE_VERTEX:
                return ShaderStage::Vertex;
            case SLANG_STAGE_FRAGMENT:
                return ShaderStage::Fragment;
            case SLANG_STAGE_GEOMETRY:
                return ShaderStage::Geometry;
            case SLANG_STAGE_COMPUTE:
                return ShaderStage::Compute;
            default:
                return ShaderStage::None;
        }
    }

    refl::Data SlangCompiler::extractReflection(slang::ProgramLayout* layout, const size_t entryPointIndex)
    {
        refl::Data result;

        slang::EntryPointReflection* ep = layout->getEntryPointByIndex(entryPointIndex);
        const SlangStage stage = ep->getStage();

        extractPushConstants(layout, result);

        const uint32_t globalParamCount = layout->getParameterCount();
        for (uint32_t i = 0; i < globalParamCount; i++)
        {
            slang::VariableLayoutReflection* param = layout->getParameterByIndex(i);

            extractBinding(param, result.resources);
        }

        uint32_t epParamCount = ep->getParameterCount();
        uint32_t bindingIndex = 0;
        for (uint32_t p = 0; p < epParamCount; p++)
        {
            slang::VariableLayoutReflection* param = ep->getParameterByIndex(p);
            const slang::ParameterCategory category = param->getCategory();

            if (category == slang::VaryingInput ||
                category == slang::Mixed)
            {
                if (stage == SLANG_STAGE_VERTEX)
                    extractVertexInput(param, result, bindingIndex++);
            }
            else if(category != slang::VaryingOutput)
            {
                extractBinding(param, result.resources);
            }
        }

        if (stage == SLANG_STAGE_COMPUTE)
        {
            SlangUInt size[3];
            ep->getComputeThreadGroupSize(3, size);
            result.workgroupSize = {
                static_cast<uint32_t>(size[0]),
                static_cast<uint32_t>(size[1]),
                static_cast<uint32_t>(size[2])
            };
        }

        return result;
    }

    void SlangCompiler::extractVertexInput(slang::VariableLayoutReflection* param, refl::Data& result, uint32_t binding)
    {
        slang::TypeLayoutReflection* typeLayout = param->getTypeLayout();

        const uint32_t paramBaseLocation = param->getOffset(SLANG_PARAMETER_CATEGORY_VARYING_INPUT);

        if (typeLayout->getKind() == slang::TypeReflection::Kind::Struct)
        {
            uint32_t currentOffset = 0;
            const uint32_t fieldCount = typeLayout->getFieldCount();

            for (uint32_t f = 0; f < fieldCount; f++)
            {
                slang::VariableLayoutReflection* field = typeLayout->getFieldByIndex(f);
                slang::TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();

                refl::VertexAttr attr;
                attr.binding = binding;
                attr.location = paramBaseLocation + static_cast<uint32_t>(field->getOffset(SLANG_PARAMETER_CATEGORY_VARYING_INPUT));
                attr.offset = currentOffset;
                attr.type = SlangConvert::dataType(fieldTypeLayout);
                attr.name = field->getName();
                result.vertexAttrs.push_back(attr);

                currentOffset += SlangConvert::dataTypeByteWidth(attr.type);
            }

            for(auto& attrib : result.vertexAttrs)
            {
                attrib.stride = currentOffset;
            }
        }
        else
        {
            refl::VertexAttr attr;
            attr.binding = 0;
            attr.type = SlangConvert::dataType(typeLayout);
            attr.stride = SlangConvert::dataTypeByteWidth(attr.type);
            attr.location = static_cast<uint32_t>(param->getOffset(slang::VaryingInput));
            attr.offset = 0;
            attr.name = param->getName();
            result.vertexAttrs.push_back(attr);
        }
    }

    void SlangCompiler::extractBinding(slang::VariableLayoutReflection* param, std::vector<refl::Resource>& resources)
    {
        if (param->getCategory() == slang::ParameterCategory::PushConstantBuffer)
            return;

        slang::TypeLayoutReflection* typeLayout = param->getTypeLayout();
        const slang::TypeReflection::Kind kind = typeLayout->getKind();

        refl::Resource resource;
        bool isBinding = true;

        switch (kind)
        {
            case slang::TypeReflection::Kind::ConstantBuffer:
            {
                resource.type = refl::ResType::CBuffer;

                slang::TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
                if (elementType)
                {
                    resource.size = static_cast<uint32_t>(elementType->getSize());
                    extractMembers(elementType, resource.members);
                }

                break;
            }
            case slang::TypeReflection::Kind::ParameterBlock:
            {
                extractParameterBlock(param, typeLayout, resources);
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
                    resource.type = refl::ResType::Buffer;

                    slang::TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
                    if (elementType)
                    {
                        resource.size = static_cast<uint32_t>(elementType->getSize());
                        extractMembers(elementType, resource.members);
                    }
                }
                else if (baseShape == SLANG_BYTE_ADDRESS_BUFFER)
                {
                    resource.type = refl::ResType::Buffer;
                }
                else if (baseShape >= SLANG_TEXTURE_1D && baseShape <= SLANG_TEXTURE_CUBE)
                {
                    if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                    {
                        resource.type = refl::ResType::Image;
                    }
                    else
                    {
                        resource.type = refl::ResType::Texture;
                    }
                }else
                {
                    resource.type = refl::ResType::Buffer;
                }

                break;
            }

            case slang::TypeReflection::Kind::SamplerState:
                resource.type = refl::ResType::Sampler;
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
                        resource.type = refl::ResType::Sampler;
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
                                resource.type = refl::ResType::Image;
                            else
                                resource.type = refl::ResType::Texture;
                        }
                        else
                        {
                            resource.type = refl::ResType::Buffer;
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

            resources.push_back(resource);
        }
    }

    void SlangCompiler::extractMembers(slang::TypeLayoutReflection* typeLayout, std::vector<refl::Member>& members)
    {
        if (typeLayout->getKind() != slang::TypeReflection::Kind::Struct)
            return;

        const uint32_t fieldCount = typeLayout->getFieldCount();
        for (uint32_t i = 0; i < fieldCount; i++)
        {
            slang::VariableLayoutReflection* field = typeLayout->getFieldByIndex(i);
            slang::TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();

            refl::Member member;
            member.name = field->getName();
            member.type = SlangConvert::dataType(fieldTypeLayout);
            member.offset = static_cast<uint32_t>(field->getOffset());
            member.size = static_cast<uint32_t>(fieldTypeLayout->getSize());

            members.push_back(member);
        }
    }

    void SlangCompiler::extractPushConstants(slang::ProgramLayout* layout, refl::Data& result)
    {
        const uint32_t paramCount = layout->getParameterCount();
        for (uint32_t i = 0; i < paramCount; i++)
        {
            slang::VariableLayoutReflection* param = layout->getParameterByIndex(i);
            slang::TypeLayoutReflection* typeLayout = param->getTypeLayout();

            if (param->getCategory() == slang::ParameterCategory::PushConstantBuffer)
            {
                refl::PushConst pc;

                slang::TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
                if (elementType)
                {
                    pc.size = static_cast<uint32_t>(elementType->getSize());
                    extractMembers(elementType, pc.members);
                }
                else
                {
                    pc.size = static_cast<uint32_t>(typeLayout->getSize());
                }

                result.pushConst = pc;
                return;
            }
        }
    }

    void SlangCompiler::extractParameterBlock(slang::VariableLayoutReflection* param, slang::TypeLayoutReflection* typeLayout, std::vector<refl::Resource>& resources)
    {
        slang::TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
        if (!elementType)
            return;

        auto innerSet = static_cast<uint32_t>(param->getOffset(SLANG_PARAMETER_CATEGORY_SUB_ELEMENT_REGISTER_SPACE));

        if (innerSet == 0xFFFFFFFF) {
            innerSet = param->getBindingSpace();
        }

        const std::string blockName = param->getName();

        const size_t uniformSize = elementType->getSize(SLANG_PARAMETER_CATEGORY_UNIFORM);
        if (uniformSize > 0)
        {
            refl::Resource uniformResource;
            uniformResource.name = blockName;
            uniformResource.type = refl::ResType::Buffer;
            uniformResource.set = innerSet;
            uniformResource.binding = 0;
            uniformResource.size = static_cast<uint32_t>(uniformSize);

            extractUniformMembers(elementType, uniformResource.members);

            if (!uniformResource.members.empty())
            {
                resources.push_back(uniformResource);
            }
        }

        extractParameterBlockResources(elementType, innerSet, 0, blockName, resources);
    }

    void SlangCompiler::extractUniformMembers(slang::TypeLayoutReflection* typeLayout, std::vector<refl::Member>& members)
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



            refl::Member member;
            member.name = field->getName();
            member.type = SlangConvert::dataType(fieldTypeLayout);
            member.offset = static_cast<uint32_t>(field->getOffset(SLANG_PARAMETER_CATEGORY_UNIFORM));
            member.size = static_cast<uint32_t>(fieldTypeLayout->getSize(SLANG_PARAMETER_CATEGORY_UNIFORM));


            if (member.size > 0)
            {
                members.push_back(member);
            }
        }
    }

     void SlangCompiler::extractParameterBlockResources(slang::TypeLayoutReflection* typeLayout, uint32_t set, uint32_t baseBinding, const std::string& prefix, std::vector<refl::Resource>& resources)
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

            auto bindingOffset = static_cast<uint32_t>(field->getOffset(SLANG_PARAMETER_CATEGORY_DESCRIPTOR_TABLE_SLOT));
            uint32_t absoluteBinding = baseBinding + bindingOffset;

            if (fieldKind == slang::TypeReflection::Kind::Resource)
            {
                refl::Resource resource;
                resource.name = qualifiedName;
                resource.set = set;
                resource.binding = absoluteBinding;
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
                    resource.type = refl::ResType::Buffer;
                    slang::TypeLayoutReflection* elementType = fieldTypeLayout->getElementTypeLayout();
                    if (elementType)
                    {
                        resource.size = static_cast<uint32_t>(elementType->getSize());
                        extractMembers(elementType, resource.members);
                    }
                }
                else if (baseShape == SLANG_BYTE_ADDRESS_BUFFER)
                {
                    resource.type = refl::ResType::Buffer;
                }
                else if (baseShape >= SLANG_TEXTURE_1D && baseShape <= SLANG_TEXTURE_CUBE)
                {
                    if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                        resource.type = refl::ResType::Image;
                    else
                        resource.type = refl::ResType::Texture;
                }
                else
                {
                    resource.type = refl::ResType::Buffer;
                }

                resources.push_back(resource);
            }
            else if (fieldKind == slang::TypeReflection::Kind::SamplerState)
            {
                refl::Resource resource;
                resource.name = qualifiedName;
                resource.type = refl::ResType::Sampler;
                resource.set = set;                 // <--- Uses innerSet (1)
                resource.binding = absoluteBinding; // <--- Uses absolute (2, 3, etc)
                resource.count = 1;
                resources.push_back(resource);
            }
            else if (fieldKind == slang::TypeReflection::Kind::Array)
            {
                slang::TypeLayoutReflection* elementLayout = fieldTypeLayout->getElementTypeLayout();
                slang::TypeReflection::Kind elementKind = elementLayout->getKind();
                auto arrayCount = static_cast<uint32_t>(fieldTypeLayout->getElementCount());

                if (elementKind == slang::TypeReflection::Kind::Resource ||
                    elementKind == slang::TypeReflection::Kind::SamplerState)
                {
                    refl::Resource resource;
                    resource.name = qualifiedName;
                    resource.set = set;
                    resource.binding = absoluteBinding;
                    resource.count = arrayCount;

                    if (elementKind == slang::TypeReflection::Kind::SamplerState)
                    {
                        resource.type = refl::ResType::Sampler;
                    }
                    else
                    {
                        auto baseShape = static_cast<SlangResourceShape>(elementLayout->getResourceShape() & SLANG_RESOURCE_BASE_SHAPE_MASK);
                        SlangResourceAccess access = elementLayout->getResourceAccess();

                        if (baseShape >= SLANG_TEXTURE_1D && baseShape <= SLANG_TEXTURE_CUBE)
                        {
                            if (access == SLANG_RESOURCE_ACCESS_READ_WRITE)
                                resource.type = refl::ResType::Image;
                            else
                                resource.type = refl::ResType::Texture;
                        }
                        else
                        {
                            resource.type = refl::ResType::Buffer;
                        }
                    }

                    resources.push_back(resource);
                }
            }
            else if (fieldKind == slang::TypeReflection::Kind::Struct)
            {
                extractParameterBlockResources(fieldTypeLayout, set, absoluteBinding, qualifiedName, resources);
            }
        }
    }
}
