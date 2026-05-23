#pragma once

#include "d3D12Device.h"
#include "shader.h"

namespace urhi
{
class D3D12Shader final : public Shader
{
public:
    struct ResourceBinding;

    D3D12Shader(D3D12Device* device, const ShaderEntryPoint& entryPoint);
    ShaderEntryPoint entryPoint() override { return m_entryPoint; }
    D3D12_SHADER_BYTECODE bytecode() const;
    const std::unordered_map<uint32_t, ResourceBinding>& resourceBindings() { return m_resourceBindings; }


private:
    D3D12Device* m_device;
    ShaderEntryPoint m_entryPoint;

    std::vector<uint8_t> m_dxil;

    // name hash to resource binding
    std::unordered_map<uint32_t, ResourceBinding> m_resourceBindings;

    static std::unordered_map<uint32_t, std::vector<uint8_t>> m_dxilCache;

    std::vector<uint8_t> getOrCompileDxil(const std::vector<uint32_t>& spirv, ShaderStage stage) const;

    std::optional<std::vector<uint8_t>> tryLoadDisk(uint32_t hash) const;
    void saveDisk(uint32_t hash, std::vector<uint8_t> dxil) const;

    void buildResourceBindings();

    static std::string spirvToHlsl(const std::vector<uint32_t>& spirv);
    static std::vector<uint8_t> hlslToDxil(const std::string& hlsl, ShaderStage stage);

    struct ResourceBinding
    {
        D3D12_DESCRIPTOR_RANGE_TYPE rangeType;
        uint32_t regSpace;
        uint32_t regIndex;
    };
};
}
