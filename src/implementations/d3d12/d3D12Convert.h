#pragma once
#include "input/keyCodes.h"

#include "d3D12Helper.h"
#include "descriptions/shaderReflection.h"
#include "enums/blendFactor.h"
#include "enums/blendOp.h"
#include "enums/colorWriteMask.h"
#include "enums/cullMode.h"
#include "enums/fillMode.h"
#include "enums/pixelFormat.h"
#include "enums/primitiveType.h"

namespace urhi
{
class D3D12Convert
{
public:
    static KeyCode     keyCode(int glfwKey);
    static KeyMod      keyMod(int glfwMods);
    static MouseButton mouseButton(int glfwButton);

    static DXGI_FORMAT format(ShaderReflection::DataType type);
    static DXGI_FORMAT format(PixelFormat type);

    static D3D12_RESOURCE_DIMENSION resourceDimension(TextureType type);

    static D3D12_FILL_MODE fillMode(FillMode fillMode);
    static D3D12_CULL_MODE cullMode(CullMode cullMode);

    static D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType(PrimitiveType type);

    static D3D12_BLEND srcBlendFactor(BlendFactor factor);
    static D3D12_BLEND dstBlendFactor(BlendFactor factor);
    static D3D12_BLEND_OP blendOp(BlendOp op);
    static UINT8 writeMask(ColorWriteMask mask);

    static D3D12_DESCRIPTOR_RANGE_TYPE rangeType(D3D_SHADER_INPUT_TYPE type);
};
}
