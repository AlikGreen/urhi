#include "d3D12Shader.h"

#include <d3d12shader.h>

#include "d3D12Context.h"
#include "d3D12Convert.h"
#include "spirv_hlsl.hpp"


namespace urhi
{
    std::unordered_map<uint32_t, std::vector<uint8_t>> D3D12Shader::m_dxilCache;

    D3D12Shader::D3D12Shader(D3D12Device *device, const ShaderEntryPoint &entryPoint)
        : m_device(device), m_entryPoint(entryPoint)
    {
        m_dxil = getOrCompileDxil(entryPoint.spirv, entryPoint.stage);
        buildResourceBindings();
    }

    D3D12_SHADER_BYTECODE D3D12Shader::bytecode() const
    {
        D3D12_SHADER_BYTECODE bytecode;
        bytecode.BytecodeLength = m_dxil.size();
        bytecode.pShaderBytecode = m_dxil.data();
        return bytecode;
    }

    std::vector<uint8_t> D3D12Shader::getOrCompileDxil(const std::vector<uint32_t> &spirv, ShaderStage stage) const
    {
        uint32_t hash = grl::Hash::fnv1a32( std::as_bytes(std::span(spirv)));
        grl::Hash::hashCombine(hash, static_cast<uint32_t>(stage));
        grl::Hash::hashCombine(hash, 1); // Compiler hash

        if (const auto it = m_dxilCache.find(hash); it != m_dxilCache.end())
            return it->second;

        if (const auto cached = tryLoadDisk(hash))
            return m_dxilCache[hash] = *cached;

        const auto hlsl = spirvToHlsl(spirv);
        const auto dxil = hlslToDxil(hlsl, stage);

        // TODO put max size on cache and evict LRU entry
        saveDisk(hash, dxil);
        return m_dxilCache[hash] = dxil;
    }

    std::string D3D12Shader::spirvToHlsl(const std::vector<uint32_t> &spirv)
    {
        spirv_cross::CompilerHLSL compiler(spirv);

        spirv_cross::CompilerHLSL::Options opts{};
        opts.shader_model = 66;
        compiler.set_hlsl_options(opts);

        try
        {
            return compiler.compile();
        }catch (const spirv_cross::CompilerError& e)
        {
            URHI_VALIDATE(false, "Failed to compile shader: {}", e.what());
            return "";
        }
    }

    std::vector<uint8_t> D3D12Shader::hlslToDxil(const std::string &hlsl, const ShaderStage stage)
    {
        ComPtr<IDxcUtils>     utils;
        ComPtr<IDxcCompiler3> compiler;
        DxcCreateInstance(CLSID_DxcUtils,     IID_PPV_ARGS(&utils));
        DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

        ComPtr<IDxcBlobEncoding> source;
        utils->CreateBlob(hlsl.data(), hlsl.size(), CP_UTF8, &source);

        std::wstring shaderType{};

        switch (stage)
        {
            case ShaderStage::Compute:
                shaderType = L"cs_6_6";
            case ShaderStage::Fragment:
                shaderType = L"ps_6_6";
            case ShaderStage::Vertex:
                shaderType = L"vs_6_6";
            case ShaderStage::Geometry:
                shaderType = L"gs_6_6";
            default:
                shaderType = L"unknown";
        }

        LPCWSTR args[] = {
            L"-E", L"main",
            L"-T", shaderType.c_str(),
        };

        DxcBuffer sourceBuffer{};
        sourceBuffer.Ptr      = source->GetBufferPointer();
        sourceBuffer.Size     = source->GetBufferSize();
        sourceBuffer.Encoding = CP_UTF8;

        ComPtr<IDxcResult> result;
        compiler->Compile(&sourceBuffer, args, _countof(args),
                          nullptr, IID_PPV_ARGS(&result));

        ComPtr<IDxcBlob> dxil;
        result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&dxil), nullptr);

        auto data = static_cast<const uint8_t*>(dxil->GetBufferPointer());
        return { data, data + dxil->GetBufferSize() };
    }

    std::optional<std::vector<uint8_t>> D3D12Shader::tryLoadDisk(const uint32_t hash) const
    {
        const std::string fileName = fmt::format("{:X}", hash) + ".dxil";
        const auto path = m_device->context()->cachePath() / fileName;
        auto result = grl::File::read(path.string());

        if(result.has_value())
            return std::vector<uint8_t>{result.value().begin(), result.value().end()};

        return std::nullopt;
    }

    void D3D12Shader::saveDisk(const uint32_t hash, std::vector<uint8_t> dxil) const
    {
        const std::string fileName = fmt::format("{:X}", hash) + ".dxil";
        const auto path = m_device->context()->cachePath() / fileName;

        grl::File::write(path.string(), dxil);
    }

    void D3D12Shader::buildResourceBindings()
    {
        m_resourceBindings.clear();

        ComPtr<IDxcUtils> utils;
        auto hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to create DXC utils");

        ComPtr<ID3D12ShaderReflection> reflection;

        DxcBuffer dxilBuffer{};
        dxilBuffer.Ptr = m_dxil.data();
        dxilBuffer.Size = m_dxil.size();
        dxilBuffer.Encoding = 0;

        hr = utils->CreateReflection(&dxilBuffer, IID_PPV_ARGS(&reflection));
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to create DXIL reflection");

        D3D12_SHADER_DESC shaderDesc{};
        hr = reflection->GetDesc(&shaderDesc);
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to get shader reflection desc");

        for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
        {
            D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
            if (FAILED(reflection->GetResourceBindingDesc(i, &bindDesc)) || bindDesc.Name == nullptr)
                continue;

            ResourceBinding binding{};
            binding.rangeType = D3D12Convert::rangeType(bindDesc.Type);
            binding.regIndex = bindDesc.BindPoint;
            binding.regSpace = bindDesc.Space;

            const uint32_t nameHash = grl::Hash::fnv1a32(bindDesc.Name);

            m_resourceBindings.emplace(nameHash, binding);
        }
    }
}
