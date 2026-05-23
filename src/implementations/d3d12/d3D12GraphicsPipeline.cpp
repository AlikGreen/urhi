#include "d3D12GraphicsPipeline.h"

#include "d3D12Convert.h"
#include "d3D12Shader.h"
#include <d3d12shader.h>
#include <map>
#include <ranges>
#include <unordered_set>

namespace urhi
{
    D3D12GraphicsPipeline::D3D12GraphicsPipeline(D3D12Device *device, const GraphicsPipelineDesc &desc)
        : m_device(device)
    {
        ID3D12PipelineState* pipelineState;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

        D3D12Shader* vertexShader  = nullptr;

        for(const auto& shader : desc.shaders)
        {
            auto dxShader = static_cast<D3D12Shader*>(shader.get());

            switch (dxShader->entryPoint().stage)
            {
                case ShaderStage::Fragment:
                    psoDesc.PS = dxShader->bytecode();
                    break;
                case ShaderStage::Vertex:
                    vertexShader = dxShader;
                    psoDesc.VS = dxShader->bytecode();
                    break;
                case ShaderStage::Geometry:
                    psoDesc.GS = dxShader->bytecode();
                    break;
                case ShaderStage::Compute:
                    URHI_VALIDATE(false, "Invalid shader type - Cannot create a graphics pipeline with a compute shader");
                    break;
                default:
                    URHI_VALIDATE(false, "Invalid shader type - This shader type cannot be used to create a graphics pipeline");
                    break;
            }
        }

        URHI_VALIDATE(psoDesc.VS.BytecodeLength > 0 && psoDesc.PS.BytecodeLength > 0, "Failed to create graphics pipeline - You must provide a vertex shader and a fragment shader to create a graphics pipeline");

        ComPtr<IDxcUtils> dxcUtils;
        auto hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));

        DxcBuffer shaderBuffer{};
        shaderBuffer.Ptr = vertexShader->bytecode().pShaderBytecode;
        shaderBuffer.Size = vertexShader->bytecode().BytecodeLength;
        shaderBuffer.Encoding = DXC_CP_ACP;

        ComPtr<ID3D12ShaderReflection> reflection;
        hr = dxcUtils->CreateReflection(&shaderBuffer, IID_PPV_ARGS(&reflection));

        D3D12_SHADER_DESC vertShaderDesc{};
        hr = reflection->GetDesc(&vertShaderDesc);

        std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;

        for (size_t i = 0; i < vertShaderDesc.InputParameters; ++i)
        {
            D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
            hr = reflection->GetInputParameterDesc(i, &paramDesc);

            const auto& attribute = vertexShader->entryPoint().reflection.vertexInput.attributes[i];

            D3D12_INPUT_ELEMENT_DESC inputDesc;
            inputDesc.SemanticName = paramDesc.SemanticName ? paramDesc.SemanticName : "";
            inputDesc.SemanticIndex = paramDesc.SemanticIndex;

            inputDesc.AlignedByteOffset = attribute.offset;
            inputDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            inputDesc.InstanceDataStepRate = 0;
            inputDesc.Format = D3D12Convert::format(attribute.type);
            inputElementDescs.push_back(inputDesc);
        }

        psoDesc.InputLayout = D3D12_INPUT_LAYOUT_DESC{inputElementDescs.data(), static_cast<uint32_t>(inputElementDescs.size())};


        psoDesc.pRootSignature = createRootSignature(desc);


        D3D12_RASTERIZER_DESC rasterDesc;
        rasterDesc.FillMode = D3D12Convert::fillMode(desc.rasterizerState.fillMode);
        rasterDesc.CullMode = D3D12Convert::cullMode(desc.rasterizerState.cullMode);
        rasterDesc.FrontCounterClockwise = FALSE;
        rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        rasterDesc.DepthClipEnable = TRUE;
        rasterDesc.MultisampleEnable = FALSE;
        rasterDesc.AntialiasedLineEnable = FALSE;
        rasterDesc.ForcedSampleCount = 0;
        rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        psoDesc.RasterizerState = rasterDesc;
        psoDesc.PrimitiveTopologyType = D3D12Convert::primitiveType(desc.primitiveType);

        D3D12_BLEND_DESC blendDesc;
        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;

        for(size_t i = 0; i < desc.colorAttachments.size(); i++)
        {
            auto blend = desc.colorAttachments[i].blend;
            const D3D12_RENDER_TARGET_BLEND_DESC targetBlendDesc = {
                blend.enableBlend,
                false,
                D3D12Convert::srcBlendFactor(blend.srcColorFactor),
                D3D12Convert::dstBlendFactor(blend.dstColorFactor),
                D3D12Convert::blendOp(blend.colorOp),
                D3D12Convert::srcBlendFactor(blend.srcAlphaFactor),
                D3D12Convert::dstBlendFactor(blend.dstAlphaFactor),
                D3D12Convert::blendOp(blend.alphaOp),
                D3D12_LOGIC_OP_NOOP,
                D3D12Convert::writeMask(blend.writeMask),
            };

            blendDesc.RenderTarget[i] = targetBlendDesc;
        }

        psoDesc.BlendState = blendDesc;

        // 🌑 Depth/Stencil State
        psoDesc.DepthStencilState.DepthEnable = desc.depthState.enableDepthTest;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;

        // 🖼️ Output
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        hr = device->device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
        URHI_VALIDATE(hr, "Failed to create graphics pipeline");
    }

    ID3D12RootSignature* D3D12GraphicsPipeline::createRootSignature(const GraphicsPipelineDesc &desc) const
    {
        std::unordered_set<uint32_t> seenNames;
        std::map<uint32_t, std::vector<D3D12_DESCRIPTOR_RANGE1>> rangesBySpace;
        std::vector<D3D12_DESCRIPTOR_RANGE1> samplerRanges;

        for (const auto& shader : desc.shaders)
        {
            auto dxShader = std::static_pointer_cast<D3D12Shader>(shader);

            for (const auto& [nameHash, binding] : dxShader->resourceBindings())
            {
                if(seenNames.contains(nameHash)) continue;
                seenNames.insert(nameHash);

                D3D12_DESCRIPTOR_RANGE1 range{};
                range.RangeType = binding.rangeType;
                range.NumDescriptors = 1;
                range.BaseShaderRegister = binding.regIndex;
                range.RegisterSpace = binding.regSpace;
                range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

                if(range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
                    samplerRanges.push_back(range);
                else
                    rangesBySpace[binding.regSpace].push_back(range);
            }
        }


        std::vector<D3D12_ROOT_PARAMETER1> parameters;

        for(const auto& ranges: rangesBySpace | std::views::values)
        {
            D3D12_ROOT_PARAMETER1 param{};
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

            param.DescriptorTable.NumDescriptorRanges = ranges.size();
            param.DescriptorTable.pDescriptorRanges = ranges.data();

            parameters.push_back(param);
        }

        if (!samplerRanges.empty())
        {
            D3D12_ROOT_PARAMETER1 samplersParam{};
            samplersParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            samplersParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
            samplersParam.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(samplerRanges.size());
            samplersParam.DescriptorTable.pDescriptorRanges = samplerRanges.data();

            parameters.push_back(samplersParam);
        }

        D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
        rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;

        rootSignatureDesc.Desc_1_1.NumParameters = parameters.size();
        rootSignatureDesc.Desc_1_1.pParameters = parameters.data();

        ID3D12RootSignature* rootSignature;

        ID3DBlob* signature;
        ID3DBlob* error;
        D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);

        m_device->device()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

        return rootSignature;
    }
}
