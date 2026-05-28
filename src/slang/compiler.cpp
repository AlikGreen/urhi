#include "compiler.h"

#include <iostream>
#include <slang-com-ptr.h>

#include "clogr.h"
#include "convert.h"
#include "memoryBlob.h"


namespace urhi::slang
{
    using namespace ::slang;

    Module Compiler::compileModule(const CompileDesc &desc, Diagnostics *outDiag)
    {
        if (outDiag) *outDiag = Diagnostics{};

        std::vector<const char*> searchPaths;
        searchPaths.reserve(desc.includePaths.size());
        for (auto& s : desc.includePaths) searchPaths.push_back(s.c_str());

        std::vector<std::string> macroNames;
        std::vector<std::string> macroValues;
        macroNames.reserve(desc.defines.size());
        macroValues.reserve(desc.defines.size());

        std::vector<PreprocessorMacroDesc> macros;
        macros.reserve(desc.defines.size());

        for (const std::string& def : desc.defines)
        {
            const size_t eq = def.find('=');
            const std::string name  = def.substr(0, eq);
            const std::string value = (eq == std::string::npos) ? "1" : def.substr(eq + 1);

            macroNames.push_back(name);
            macroValues.push_back(value);

            PreprocessorMacroDesc m = {};
            m.name  = macroNames.back().c_str();
            m.value = macroValues.back().c_str();
            macros.push_back(m);
        }

        SessionDesc sessionDesc = {};

        TargetDesc targetDesc = {};
        targetDesc.format  = SLANG_SPIRV;
        targetDesc.profile = getGlobalSession()->findProfile("spirv_1_6");

        sessionDesc.targets     = &targetDesc;
        sessionDesc.targetCount = 1;

        sessionDesc.searchPaths     = searchPaths.empty() ? nullptr : searchPaths.data();
        sessionDesc.searchPathCount = static_cast<SlangInt>(searchPaths.size());

        sessionDesc.preprocessorMacros     = macros.empty() ? nullptr : macros.data();
        sessionDesc.preprocessorMacroCount = static_cast<SlangInt>(macros.size());

        Slang::ComPtr<ISession> session;
        getGlobalSession()->createSession(sessionDesc, session.writeRef());

        Slang::ComPtr<IBlob> loadDiags;
        Slang::ComPtr<IModule> module;
        module = session->loadModuleFromSourceString(
            desc.moduleName.c_str(),
            desc.modulePath.c_str(),
            desc.moduleSource.c_str(),
            loadDiags.writeRef());

        appendDiagnostics(outDiag, loadDiags.get());

        if (!module)
        {
            if (outDiag)
            {
                outDiag->hasErrors = true;
                std::cout << outDiag->text << std::endl;
            }
            return {};
        }

        Slang::ComPtr<IBlob> serialized;
        SlangResult r = module->serialize(serialized.writeRef());
        if (SLANG_FAILED(r) || !serialized)
        {
            appendDiagnostics(outDiag, "IModule::serialize() failed.\n", true);
            return {};
        }

        Module result;


        result.name = desc.moduleName;
        result.path = desc.modulePath;
        result.ir.assign(
            static_cast<const uint8_t *>(serialized->getBufferPointer()),
            static_cast<const uint8_t *>(serialized->getBufferPointer()) + serialized->getBufferSize());

        return result;
    }

    std::vector<ShaderReflection::Resource> Compiler::getResources(const std::vector<Module>& modules, Diagnostics *outDiag)
    {
        const auto session = getSession();

        std::vector<Slang::ComPtr<IModule>> slangModules;
        std::vector<IComponentType*> components;

        for (auto& m : modules)
        {
            auto slangModule = loadModuleFromIR(session, m, outDiag);
            if (slangModule)
            {
                slangModules.push_back(slangModule);
                components.push_back(slangModule.get());
            }
        }

        if (components.empty()) return {};

        Slang::ComPtr<IComponentType> linkedLib;
        session->createCompositeComponentType(
            components.data(),
            static_cast<SlangInt>(components.size()),
            linkedLib.writeRef());

        std::vector<ShaderReflection::Resource> resources;

        auto layout = linkedLib->getLayout();
        const uint32_t globalParamCount = layout->getParameterCount();
        for (uint32_t i = 0; i < globalParamCount; i++)
        {
            VariableLayoutReflection* param = layout->getParameterByIndex(i);

            extractBinding(param, resources);
        }

        return resources;
    }

    ShaderSet Compiler::linkToShaderSet(const LinkDesc &desc, Diagnostics *outDiag)
    {
        auto [entryPoints, linked, session] = compileAndLink(desc, outDiag);
        if(outDiag && outDiag->hasErrors) return {};

        ProgramLayout* programLayout = linked->getLayout();

        std::vector<ShaderEntryPoint> result;

        for (size_t i = 0; i < entryPoints.size(); i++)
        {
            auto spriv = extractSpirv(linked, i, outDiag);
            EntryPointReflection* epReflection = programLayout->getEntryPointByIndex(i);
            SlangStage slangStage = epReflection->getStage();

            auto reflection = extractReflection(programLayout, i);

            ShaderEntryPoint entryPoint;
            entryPoint.name = entryPoints[i]->getLayout()->getEntryPointByIndex(0)->getName();
            entryPoint.stage = Convert::shaderStage(slangStage);
            entryPoint.reflection = reflection;
            entryPoint.spirv = std::move(spriv);

            result.push_back(std::move(entryPoint));
        }

        return ShaderSet(result);
    }

    IGlobalSession* Compiler::getGlobalSession()
    {
        static Slang::ComPtr<IGlobalSession> s_global;
        if (!s_global)
        {
            createGlobalSession(s_global.writeRef());
        }
        return s_global.get();
    }

    Slang::ComPtr<ISession> Compiler::getSession()
    {
        static Slang::ComPtr<ISession> s_session;
        if (s_session)
        {
            return s_session;
        }

        IGlobalSession* globalSession = getGlobalSession();

        TargetDesc targetDesc{};
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = globalSession->findProfile("spirv_1_6");
        targetDesc.forceGLSLScalarBufferLayout = false;

        SessionDesc sessionDesc{};
        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.searchPaths = nullptr;
        sessionDesc.searchPathCount = 0;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

        globalSession->createSession(sessionDesc, s_session.writeRef());
        return s_session;
    }

    std::tuple<std::vector<Slang::ComPtr<IEntryPoint>>, Slang::ComPtr<IComponentType>, Slang::ComPtr<ISession>>
    Compiler::compileAndLink(const LinkDesc &desc, Diagnostics *outDiags)
    {
        const auto session = getSession();
        std::vector<Slang::ComPtr<IModule>> modules;

        for (auto& m : desc.modules)
        {
            auto module = loadModuleFromIR(session, m, outDiags);
            if (!module)
                return {};
            modules.push_back(module);
        }

        // ── Helper: find a concrete type by name across all modules ──────────
        auto findType = [&](const std::string& typeName) -> TypeReflection*
        {
            for (const auto& mod : modules)
            {
                TypeReflection* t = mod->getLayout()->findTypeByName(typeName.c_str());
                if (t) return t;
            }
            return nullptr;
        };

        // ── Helper: specialize a component if it has params ──────────────────
        auto trySpecialize = [&](Slang::ComPtr<IComponentType> component) -> Slang::ComPtr<IComponentType>
        {
            const SlangInt paramCount = component->getSpecializationParamCount();
            if (paramCount == 0 || desc.typeSpecializations.empty())
                return component;

            std::vector<SpecializationArg> args;
            for (SlangInt i = 0; i < paramCount && i < (SlangInt)desc.typeSpecializations.size(); i++)
            {
                TypeReflection* concreteType = findType(desc.typeSpecializations[i]);
                if (!concreteType)
                {
                    appendDiagnostics(outDiags, "Could not find type: " + desc.typeSpecializations[i] + "\n", true);
                    return nullptr;
                }

                SpecializationArg arg{};
                arg.kind = SpecializationArg::Kind::Type;
                arg.type = concreteType;
                args.push_back(arg);
            }

            Slang::ComPtr<IComponentType> specialized;
            Slang::ComPtr<IBlob> diag;
            SlangResult r = component->specialize(
                args.data(),
                static_cast<SlangInt>(args.size()),
                specialized.writeRef(),
                diag.writeRef());

            appendDiagnostics(outDiags, diag);

            if (SLANG_FAILED(r) || !specialized)
            {
                if (outDiags) outDiags->hasErrors = true;
                return nullptr;
            }

            return specialized;
        };

        // ── Step 1: specialize individual modules that need it ───────────────
        std::vector<Slang::ComPtr<IComponentType>> components;

        for (const auto& mod : modules)
        {
            // Explicit conversion from IModule to IComponentType
            Slang::ComPtr<IComponentType> modComp;
            mod->queryInterface(IComponentType::getTypeGuid(), (void**)modComp.writeRef());

            auto specialized = trySpecialize(modComp);
            if (!specialized)
                return {};
            components.push_back(specialized);
        }


        // ── Step 2: collect entry points ─────────────────────────────────────
        std::vector<Slang::ComPtr<IEntryPoint>> entryPoints;

        for (const auto& module : modules)
        {
            if (desc.entryPoints.empty())
            {
                const SlangInt epCount = module->getDefinedEntryPointCount();
                for (SlangInt i = 0; i < epCount; i++)
                {
                    Slang::ComPtr<IEntryPoint> ep;
                    module->getDefinedEntryPoint(i, ep.writeRef());
                    if (ep) entryPoints.push_back(ep);
                }
            }
            else
            {
                for (auto& name : desc.entryPoints)
                {
                    Slang::ComPtr<IEntryPoint> ep;
                    if (SLANG_SUCCEEDED(module->findEntryPointByName(name.c_str(), ep.writeRef())))
                        entryPoints.push_back(ep);
                }
            }
        }

        for (const auto& ep : entryPoints)
        {
            Slang::ComPtr<IComponentType> epComp;
            ep->queryInterface(IComponentType::getTypeGuid(), reinterpret_cast<void **>(epComp.writeRef()));
            components.push_back(epComp);
        }

        // ── Step 3: composite ─────────────────────────────────────────────────
        Slang::ComPtr<IComponentType> composed;
        {
            std::vector<IComponentType*> rawComponents;
            rawComponents.reserve(components.size());
            for (const auto& c : components)
                rawComponents.push_back(c.get());

            Slang::ComPtr<IBlob> diag;
            const SlangResult r = session->createCompositeComponentType(
                rawComponents.data(),
                static_cast<SlangInt>(rawComponents.size()),
                composed.writeRef(),
                diag.writeRef());

            appendDiagnostics(outDiags, diag);

            if (SLANG_FAILED(r) || !composed)
            {
                if (outDiags) outDiags->hasErrors = true;
                return {};
            }

        }

        // ── Step 4: specialize composite if it still has params ───────────────
        composed = trySpecialize(composed);
        if (!composed)
            return {};

        // ── Step 5: link ──────────────────────────────────────────────────────
        Slang::ComPtr<IComponentType> linked;
        {
            Slang::ComPtr<IBlob> diag;
            SlangResult r = composed->link(linked.writeRef(), diag.writeRef());
            appendDiagnostics(outDiags, diag);

            if (SLANG_FAILED(r) || !linked)
            {
                if (outDiags) outDiags->hasErrors = true;
                return {};
            }
        }

        return {entryPoints, linked, session};
    }


    void Compiler::appendDiagnostics(Diagnostics *outDiag, IBlob *diagBlob)
    {
        if (!outDiag) return;

        if (diagBlob && diagBlob->getBufferPointer())
        {
            outDiag->text += static_cast<const char *>(diagBlob->getBufferPointer()); // slang diag is null terminated
            clogr::warn(outDiag->text);
            clogr::flush();
        }
    }

    void Compiler::appendDiagnostics(Diagnostics *outDiag, const std::string &text)
    {
        if (!outDiag) return;
        outDiag->text += text;
    }

    void Compiler::appendDiagnostics(Diagnostics *outDiag, const std::string &text, bool hasErrors)
    {
        if (!outDiag) return;
        outDiag->text += text;
        outDiag->hasErrors = hasErrors;
    }

    Slang::ComPtr<IModule> Compiler::loadModuleFromIR(ISession *session, const Module& module, Diagnostics* diags)
    {
        Slang::ComPtr<IBlob> slangDiags;

        Slang::ComPtr<IBlob> irBlob = Slang::ComPtr<IBlob>(new MemoryBlob(module.ir));

        Slang::ComPtr<IModule> slangModule;
        slangModule = session->loadModuleFromIRBlob(
            module.name.c_str(),
            module.path.c_str(),
            irBlob.get(),
            slangDiags.writeRef());

        if (diags)
            appendDiagnostics(diags, slangDiags.get());

        return slangModule;
    }

    std::vector<uint32_t> Compiler::extractSpirv(const Slang::ComPtr<IComponentType> &linked, const uint32_t index, Diagnostics* outDiags)
    {
        Slang::ComPtr<IBlob> spirvBlob;
        Slang::ComPtr<IBlob> diagnostics; // TODO use to check errors

        linked->getEntryPointCode(
            index,
            0,
            spirvBlob.writeRef(),
            diagnostics.writeRef()
        );

        appendDiagnostics(outDiags, diagnostics);

        std::vector<uint32_t> spirv;

        const auto* spirvData = static_cast<const uint32_t*>(spirvBlob->getBufferPointer());
        const size_t spirvWordCount = spirvBlob->getBufferSize() / sizeof(uint32_t);
        spirv.assign(spirvData, spirvData + spirvWordCount);

        return spirv;
    }

    ShaderReflection Compiler::extractReflection(ProgramLayout* layout, const size_t entryPointIndex)
    {
        ShaderReflection result;

        EntryPointReflection* ep = layout->getEntryPointByIndex(entryPointIndex);
        const SlangStage stage = ep->getStage();

        extractPushConstants(layout, result);

        const uint32_t globalParamCount = layout->getParameterCount();
        for (uint32_t i = 0; i < globalParamCount; i++)
        {
            VariableLayoutReflection* param = layout->getParameterByIndex(i);

            extractBinding(param, result.resources);
        }

        uint32_t epParamCount = ep->getParameterCount();
        for (uint32_t p = 0; p < epParamCount; p++)
        {
            VariableLayoutReflection* param = ep->getParameterByIndex(p);
            const ParameterCategory category = param->getCategory();

            if (category == VaryingInput ||
                category == Mixed)
            {
                if (stage == SLANG_STAGE_VERTEX)
                {
                    extractVertexInput(param, result);
                }
            }
            else if(category != VaryingOutput)
            {
                extractBinding(param, result.resources);
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

    void Compiler::extractVertexInput(VariableLayoutReflection* param, ShaderReflection& result)
    {
        TypeLayoutReflection* typeLayout = param->getTypeLayout();
        const auto bindingSlot = static_cast<uint32_t>(result.vertexInput.bindings.size());

        const uint32_t paramBaseLocation = param->getOffset(SLANG_PARAMETER_CATEGORY_VARYING_INPUT);

        if (typeLayout->getKind() == TypeReflection::Kind::Struct)
        {
            uint32_t currentOffset = 0;
            const uint32_t fieldCount = typeLayout->getFieldCount();

            for (uint32_t f = 0; f < fieldCount; f++)
            {
                VariableLayoutReflection* field = typeLayout->getFieldByIndex(f);
                TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();

                ShaderReflection::VertexAttribute attr;
                attr.binding = bindingSlot;
                attr.location = paramBaseLocation + static_cast<uint32_t>(field->getOffset(SLANG_PARAMETER_CATEGORY_VARYING_INPUT));
                attr.offset = currentOffset;
                attr.type = Convert::dataType(fieldTypeLayout);
                attr.name = field->getName();
                result.vertexInput.attributes.push_back(attr);

                currentOffset += Convert::dataTypeByteWidth(attr.type);
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
            attr.location = static_cast<uint32_t>(param->getOffset(VaryingInput));
            attr.offset = 0;
            attr.type = Convert::dataType(typeLayout);
            attr.name = param->getName();
            result.vertexInput.attributes.push_back(attr);

            ShaderReflection::VertexBinding binding;
            binding.binding = bindingSlot;
            binding.stride =  Convert::dataTypeByteWidth(attr.type);
            result.vertexInput.bindings.push_back(binding);
        }
    }

    void Compiler::extractBinding(VariableLayoutReflection* param, std::vector<ShaderReflection::Resource>& resources)
    {
        if (param->getCategory() == PushConstantBuffer)
            return;

        TypeLayoutReflection* typeLayout = param->getTypeLayout();
        TypeReflection::Kind kind = typeLayout->getKind();

        ShaderReflection::Resource resource;
        bool isBinding = true;

        switch (kind)
        {
            case TypeReflection::Kind::ConstantBuffer:
            {
                resource.type = ShaderReflection::ResourceType::ConstantBuffer;

                TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
                if (elementType)
                {
                    resource.totalSize = static_cast<uint32_t>(elementType->getSize());
                    extractMembers(elementType, resource.members);
                }

                break;
            }
            case TypeReflection::Kind::ParameterBlock:
            {
                extractParameterBlock(param, typeLayout, resources);
                isBinding = false;
                break;
            }
            case TypeReflection::Kind::Resource:
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

                    TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
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

                    resource.dimension = Convert::textureType(baseShape);
                }else
                {
                    resource.type = ShaderReflection::ResourceType::StorageBuffer;
                }

                break;
            }

            case TypeReflection::Kind::SamplerState:
                resource.type = ShaderReflection::ResourceType::Sampler;
                break;
            case TypeReflection::Kind::Array:
            {
                TypeLayoutReflection* elementTypeLayout = typeLayout->getElementTypeLayout();
                const auto arrayCount = static_cast<uint32_t>(typeLayout->getElementCount());

                const TypeReflection::Kind elementKind = elementTypeLayout->getKind();

                if (elementKind == TypeReflection::Kind::Resource ||
                    elementKind == TypeReflection::Kind::SamplerState)
                {
                    resource.count = arrayCount;

                    if (elementKind == TypeReflection::Kind::SamplerState)
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

                            resource.dimension = Convert::textureType(baseShape);
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

            TypeReflection* type = typeLayout->getType();
            if (type->getKind() == TypeReflection::Kind::Array && resource.count == 1)
            {
                resource.count = static_cast<uint32_t>(type->getElementCount());
            }

            resources.push_back(resource);
        }
    }

    void Compiler::extractMembers(TypeLayoutReflection* typeLayout, std::vector<ShaderReflection::Member>& members)
    {
        if (typeLayout->getKind() != TypeReflection::Kind::Struct)
            return;

        const uint32_t fieldCount = typeLayout->getFieldCount();
        for (uint32_t i = 0; i < fieldCount; i++)
        {
            VariableLayoutReflection* field = typeLayout->getFieldByIndex(i);
            TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();
            TypeReflection* fieldType = fieldTypeLayout->getType();

            ShaderReflection::Member member;
            member.name = field->getName();
            member.offset = static_cast<uint32_t>(field->getOffset());
            member.size = static_cast<uint32_t>(fieldTypeLayout->getSize());
            member.arrayCount = 1;

            if (fieldType->getKind() == TypeReflection::Kind::Array)
            {
                member.arrayCount = static_cast<uint32_t>(fieldType->getElementCount());
                TypeLayoutReflection* elementLayout = fieldTypeLayout->getElementTypeLayout();
                member.type = Convert::dataType(elementLayout);

                // If array of structs, recurse
                if (elementLayout->getKind() == TypeReflection::Kind::Struct)
                {
                    member.type = ShaderReflection::DataType::Struct;
                    extractMembers(elementLayout, member.members);
                }
            }
            else if (fieldType->getKind() == TypeReflection::Kind::Struct)
            {
                member.type = ShaderReflection::DataType::Struct;
                extractMembers(fieldTypeLayout, member.members);
            }
            else if (fieldType->getKind() == TypeReflection::Kind::Matrix)
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
                member.type = Convert::dataType(fieldTypeLayout);
            }

            members.push_back(member);
        }
    }

    void Compiler::extractPushConstants(ProgramLayout* layout, ShaderReflection& result)
    {
        const uint32_t paramCount = layout->getParameterCount();
        for (uint32_t i = 0; i < paramCount; i++)
        {
            VariableLayoutReflection* param = layout->getParameterByIndex(i);
            TypeLayoutReflection* typeLayout = param->getTypeLayout();

            if (param->getCategory() == PushConstantBuffer)
            {
                ShaderReflection::PushConstant pc;
                pc.name = param->getName();
                pc.offset = 0;

                // Get the inner element type for the actual size
                TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
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

    void Compiler::extractParameterBlock(VariableLayoutReflection* param, TypeLayoutReflection* typeLayout, std::vector<ShaderReflection::Resource>& resources)
    {
        TypeLayoutReflection* elementType = typeLayout->getElementTypeLayout();
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
            ShaderReflection::Resource uniformResource;
            uniformResource.name = blockName;
            uniformResource.type = ShaderReflection::ResourceType::ConstantBuffer;
            uniformResource.set = innerSet;
            uniformResource.binding = 0;
            uniformResource.totalSize = static_cast<uint32_t>(uniformSize);

            extractUniformMembers(elementType, uniformResource.members);

            if (!uniformResource.members.empty())
            {
                resources.push_back(uniformResource);
            }
        }

        extractParameterBlockResources(elementType, innerSet, 0, blockName, resources);
    }

    void Compiler::extractUniformMembers(TypeLayoutReflection* typeLayout, std::vector<ShaderReflection::Member>& members)
    {
        if (typeLayout->getKind() != TypeReflection::Kind::Struct)
            return;

        const uint32_t fieldCount = typeLayout->getFieldCount();
        for (uint32_t i = 0; i < fieldCount; i++)
        {
            VariableLayoutReflection* field = typeLayout->getFieldByIndex(i);
            TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();
            const TypeReflection::Kind fieldKind = fieldTypeLayout->getKind();

            if (fieldKind == TypeReflection::Kind::Resource ||
                fieldKind == TypeReflection::Kind::SamplerState ||
                fieldKind == TypeReflection::Kind::ConstantBuffer ||
                fieldKind == TypeReflection::Kind::ParameterBlock)
            {
                continue;
            }

            TypeReflection* fieldType = fieldTypeLayout->getType();

            ShaderReflection::Member member;
            member.name = field->getName();
            member.offset = static_cast<uint32_t>(field->getOffset(SLANG_PARAMETER_CATEGORY_UNIFORM));
            member.size = static_cast<uint32_t>(fieldTypeLayout->getSize(SLANG_PARAMETER_CATEGORY_UNIFORM));
            member.arrayCount = 1;

            if (fieldType->getKind() == TypeReflection::Kind::Array)
            {
                member.arrayCount = static_cast<uint32_t>(fieldType->getElementCount());
                TypeLayoutReflection* elementLayout = fieldTypeLayout->getElementTypeLayout();

                if (elementLayout->getKind() == TypeReflection::Kind::Struct)
                {
                    member.type = ShaderReflection::DataType::Struct;
                    extractUniformMembers(elementLayout, member.members);
                }
                else
                {
                    member.type = Convert::dataType(elementLayout);
                }
            }
            else if (fieldType->getKind() == TypeReflection::Kind::Struct)
            {
                member.type = ShaderReflection::DataType::Struct;
                extractUniformMembers(fieldTypeLayout, member.members);
            }
            else if (fieldType->getKind() == TypeReflection::Kind::Matrix)
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
                member.type = Convert::dataType(fieldTypeLayout);
            }

            if (member.size > 0)
            {
                members.push_back(member);
            }
        }
    }

     void Compiler::extractParameterBlockResources(TypeLayoutReflection* typeLayout, uint32_t set, uint32_t baseBinding, const std::string& prefix, std::vector<ShaderReflection::Resource>& resources)
    {
        if (typeLayout->getKind() != TypeReflection::Kind::Struct)
            return;

        uint32_t fieldCount = typeLayout->getFieldCount();
        for (uint32_t i = 0; i < fieldCount; i++)
        {
            VariableLayoutReflection* field = typeLayout->getFieldByIndex(i);

            if (field->getCategory() == Uniform)
                continue;

            TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();
            TypeReflection::Kind fieldKind = fieldTypeLayout->getKind();

            const std::string qualifiedName = prefix + "." + field->getName();

            // 4. Calculate Absolute Binding for Vulkan (Base + Relative)
            // e.g. albedoMap is the 2nd thing in the block, so it gets offset 1. 0 + 1 = 1.
            auto bindingOffset = static_cast<uint32_t>(field->getOffset(SLANG_PARAMETER_CATEGORY_DESCRIPTOR_TABLE_SLOT));
            uint32_t absoluteBinding = baseBinding + bindingOffset;

            if (fieldKind == TypeReflection::Kind::Resource)
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
                    TypeLayoutReflection* elementType = fieldTypeLayout->getElementTypeLayout();
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

                    resource.dimension = Convert::textureType(baseShape);
                }
                else
                {
                    resource.type = ShaderReflection::ResourceType::StorageBuffer;
                }

                resources.push_back(resource);
            }
            else if (fieldKind == TypeReflection::Kind::SamplerState)
            {
                ShaderReflection::Resource resource;
                resource.name = qualifiedName;
                resource.type = ShaderReflection::ResourceType::Sampler;
                resource.set = set;                 // <--- Uses innerSet (1)
                resource.binding = absoluteBinding; // <--- Uses absolute (2, 3, etc)
                resource.count = 1;
                resources.push_back(resource);
            }
            else if (fieldKind == TypeReflection::Kind::Array)
            {
                TypeLayoutReflection* elementLayout = fieldTypeLayout->getElementTypeLayout();
                TypeReflection::Kind elementKind = elementLayout->getKind();
                auto arrayCount = static_cast<uint32_t>(fieldTypeLayout->getElementCount());

                if (elementKind == TypeReflection::Kind::Resource ||
                    elementKind == TypeReflection::Kind::SamplerState)
                {
                    ShaderReflection::Resource resource;
                    resource.name = qualifiedName;
                    resource.set = set;
                    resource.binding = absoluteBinding;
                    resource.count = arrayCount;

                    if (elementKind == TypeReflection::Kind::SamplerState)
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

                            resource.dimension = Convert::textureType(baseShape);
                        }
                        else
                        {
                            resource.type = ShaderReflection::ResourceType::StorageBuffer;
                        }
                    }

                    resources.push_back(resource);
                }
            }
            else if (fieldKind == TypeReflection::Kind::Struct)
            {
                extractParameterBlockResources(fieldTypeLayout, set, absoluteBinding, qualifiedName, resources);
            }
        }
    }
}
