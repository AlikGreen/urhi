#include "d3D12Convert.h"

#include "GLFW/glfw3.h"

namespace urhi
{
    KeyCode D3D12Convert::keyCode(const int glfwKey)
    {
        switch (glfwKey)
        {
            case GLFW_KEY_ENTER:       return KeyCode::Return;
            case GLFW_KEY_ESCAPE:      return KeyCode::Escape;
            case GLFW_KEY_BACKSPACE:   return KeyCode::Backspace;
            case GLFW_KEY_TAB:         return KeyCode::Tab;
            case GLFW_KEY_SPACE:       return KeyCode::Space;
            case GLFW_KEY_APOSTROPHE:  return KeyCode::Apostrophe;
            case GLFW_KEY_COMMA:       return KeyCode::Comma;
            case GLFW_KEY_MINUS:       return KeyCode::Minus;
            case GLFW_KEY_PERIOD:      return KeyCode::Period;
            case GLFW_KEY_SLASH:       return KeyCode::Slash;
            case GLFW_KEY_0:           return KeyCode::Num0;
            case GLFW_KEY_1:           return KeyCode::Num1;
            case GLFW_KEY_2:           return KeyCode::Num2;
            case GLFW_KEY_3:           return KeyCode::Num3;
            case GLFW_KEY_4:           return KeyCode::Num4;
            case GLFW_KEY_5:           return KeyCode::Num5;
            case GLFW_KEY_6:           return KeyCode::Num6;
            case GLFW_KEY_7:           return KeyCode::Num7;
            case GLFW_KEY_8:           return KeyCode::Num8;
            case GLFW_KEY_9:           return KeyCode::Num9;
            case GLFW_KEY_SEMICOLON:   return KeyCode::Semicolon;
            case GLFW_KEY_EQUAL:       return KeyCode::Equals;
            case GLFW_KEY_LEFT_BRACKET:return KeyCode::LeftBracket;
            case GLFW_KEY_BACKSLASH:   return KeyCode::Backslash;
            case GLFW_KEY_RIGHT_BRACKET:return KeyCode::RightBracket;
            case GLFW_KEY_GRAVE_ACCENT:return KeyCode::Grave;
            case GLFW_KEY_A:           return KeyCode::A;
            case GLFW_KEY_B:           return KeyCode::B;
            case GLFW_KEY_C:           return KeyCode::C;
            case GLFW_KEY_D:           return KeyCode::D;
            case GLFW_KEY_E:           return KeyCode::E;
            case GLFW_KEY_F:           return KeyCode::F;
            case GLFW_KEY_G:           return KeyCode::G;
            case GLFW_KEY_H:           return KeyCode::H;
            case GLFW_KEY_I:           return KeyCode::I;
            case GLFW_KEY_J:           return KeyCode::J;
            case GLFW_KEY_K:           return KeyCode::K;
            case GLFW_KEY_L:           return KeyCode::L;
            case GLFW_KEY_M:           return KeyCode::M;
            case GLFW_KEY_N:           return KeyCode::N;
            case GLFW_KEY_O:           return KeyCode::O;
            case GLFW_KEY_P:           return KeyCode::P;
            case GLFW_KEY_Q:           return KeyCode::Q;
            case GLFW_KEY_R:           return KeyCode::R;
            case GLFW_KEY_S:           return KeyCode::S;
            case GLFW_KEY_T:           return KeyCode::T;
            case GLFW_KEY_U:           return KeyCode::U;
            case GLFW_KEY_V:           return KeyCode::V;
            case GLFW_KEY_W:           return KeyCode::W;
            case GLFW_KEY_X:           return KeyCode::X;
            case GLFW_KEY_Y:           return KeyCode::Y;
            case GLFW_KEY_Z:           return KeyCode::Z;
            case GLFW_KEY_DELETE:      return KeyCode::Delete;

            // Function keys
            case GLFW_KEY_CAPS_LOCK:   return KeyCode::CapsLock;
            case GLFW_KEY_F1:          return KeyCode::F1;
            case GLFW_KEY_F2:          return KeyCode::F2;
            case GLFW_KEY_F3:          return KeyCode::F3;
            case GLFW_KEY_F4:          return KeyCode::F4;
            case GLFW_KEY_F5:          return KeyCode::F5;
            case GLFW_KEY_F6:          return KeyCode::F6;
            case GLFW_KEY_F7:          return KeyCode::F7;
            case GLFW_KEY_F8:          return KeyCode::F8;
            case GLFW_KEY_F9:          return KeyCode::F9;
            case GLFW_KEY_F10:         return KeyCode::F10;
            case GLFW_KEY_F11:         return KeyCode::F11;
            case GLFW_KEY_F12:         return KeyCode::F12;
            case GLFW_KEY_F13:         return KeyCode::F13;
            case GLFW_KEY_F14:         return KeyCode::F14;
            case GLFW_KEY_F15:         return KeyCode::F15;
            case GLFW_KEY_F16:         return KeyCode::F16;
            case GLFW_KEY_F17:         return KeyCode::F17;
            case GLFW_KEY_F18:         return KeyCode::F18;
            case GLFW_KEY_F19:         return KeyCode::F19;
            case GLFW_KEY_F20:         return KeyCode::F20;
            case GLFW_KEY_F21:         return KeyCode::F21;
            case GLFW_KEY_F22:         return KeyCode::F22;
            case GLFW_KEY_F23:         return KeyCode::F23;
            case GLFW_KEY_F24:         return KeyCode::F24;
            case GLFW_KEY_F25:         return KeyCode::F25;

            case GLFW_KEY_PRINT_SCREEN:return KeyCode::PrintScreen;
            case GLFW_KEY_SCROLL_LOCK: return KeyCode::ScrollLock;
            case GLFW_KEY_PAUSE:       return KeyCode::Pause;
            case GLFW_KEY_INSERT:      return KeyCode::Insert;
            case GLFW_KEY_HOME:        return KeyCode::Home;
            case GLFW_KEY_PAGE_UP:     return KeyCode::PageUp;
            case GLFW_KEY_END:         return KeyCode::End;
            case GLFW_KEY_PAGE_DOWN:   return KeyCode::PageDown;
            case GLFW_KEY_RIGHT:       return KeyCode::Right;
            case GLFW_KEY_LEFT:        return KeyCode::Left;
            case GLFW_KEY_DOWN:        return KeyCode::Down;
            case GLFW_KEY_UP:          return KeyCode::Up;
            case GLFW_KEY_NUM_LOCK:    return KeyCode::NumLockClear;
            case GLFW_KEY_KP_DIVIDE:   return KeyCode::KpDivide;
            case GLFW_KEY_KP_MULTIPLY: return KeyCode::KpMultiply;
            case GLFW_KEY_KP_SUBTRACT: return KeyCode::KpMinus;
            case GLFW_KEY_KP_ADD:      return KeyCode::KpPlus;
            case GLFW_KEY_KP_ENTER:    return KeyCode::KpEnter;
            case GLFW_KEY_KP_1:        return KeyCode::Kp1;
            case GLFW_KEY_KP_2:        return KeyCode::Kp2;
            case GLFW_KEY_KP_3:        return KeyCode::Kp3;
            case GLFW_KEY_KP_4:        return KeyCode::Kp4;
            case GLFW_KEY_KP_5:        return KeyCode::Kp5;
            case GLFW_KEY_KP_6:        return KeyCode::Kp6;
            case GLFW_KEY_KP_7:        return KeyCode::Kp7;
            case GLFW_KEY_KP_8:        return KeyCode::Kp8;
            case GLFW_KEY_KP_9:        return KeyCode::Kp9;
            case GLFW_KEY_KP_0:        return KeyCode::Kp0;
            case GLFW_KEY_KP_DECIMAL:  return KeyCode::KpPeriod;
            case GLFW_KEY_KP_EQUAL:    return KeyCode::KpEquals;
            case GLFW_KEY_MENU:        return KeyCode::Menu;

            // Modifier keys
            case GLFW_KEY_LEFT_CONTROL: return KeyCode::LCtrl;
            case GLFW_KEY_LEFT_SHIFT:   return KeyCode::LShift;
            case GLFW_KEY_LEFT_ALT:     return KeyCode::LAlt;
            case GLFW_KEY_LEFT_SUPER:   return KeyCode::LGui;
            case GLFW_KEY_RIGHT_CONTROL:return KeyCode::RCtrl;
            case GLFW_KEY_RIGHT_SHIFT:  return KeyCode::RShift;
            case GLFW_KEY_RIGHT_ALT:    return KeyCode::RAlt;
            case GLFW_KEY_RIGHT_SUPER:  return KeyCode::RGui;

            // Note: GLFW doesn't have direct equivalents for many of the specialized SDL keys
            // like media keys, special symbols, or advanced keypad functions
            // These would need to be handled through character input callbacks if needed

            default:
                return KeyCode::Unknown;
        }
    }

    KeyMod D3D12Convert::keyMod(const int glfwMods)
    {
        auto mod = KeyMod::None;
        if (glfwMods & GLFW_MOD_SHIFT)     mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::LShift));
        if (glfwMods & GLFW_MOD_CONTROL)   mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::LCtrl));
        if (glfwMods & GLFW_MOD_ALT)       mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::LAlt));
        if (glfwMods & GLFW_MOD_SUPER)     mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::LGui));
        if (glfwMods & GLFW_MOD_CAPS_LOCK) mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::Caps));
        if (glfwMods & GLFW_MOD_NUM_LOCK)  mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::Num));

        // Note: GLFW doesn't distinguish between left/right modifiers in the mods parameter
        // and doesn't have direct equivalents for Mode or Scroll lock modifiers

        return mod;
    }

    MouseButton D3D12Convert::mouseButton(const int glfwButton)
    {
        switch (glfwButton)
        {
            case GLFW_MOUSE_BUTTON_LEFT:   return MouseButton::Left;
            case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
            case GLFW_MOUSE_BUTTON_RIGHT:  return MouseButton::Right;
            case GLFW_MOUSE_BUTTON_4:      return MouseButton::Side1;
            case GLFW_MOUSE_BUTTON_5:      return MouseButton::Side2;
            // GLFW supports buttons 6-8 as well if your MouseButton enum has more
            default:                       return MouseButton::None;
        }
    }

    DXGI_FORMAT D3D12Convert::format(const ShaderReflection::DataType type)
    {
        switch (type)
        {
            case ShaderReflection::DataType::Float:
                return DXGI_FORMAT_R32_FLOAT;
            case ShaderReflection::DataType::Float2:
                return DXGI_FORMAT_R32G32_FLOAT;
            case ShaderReflection::DataType::Float3:
                return DXGI_FORMAT_R32G32B32_FLOAT;
            case ShaderReflection::DataType::Float4:
                return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case ShaderReflection::DataType::Int:
                return DXGI_FORMAT_R32_SINT;
            case ShaderReflection::DataType::Int2:
                return DXGI_FORMAT_R32G32_SINT;
            case ShaderReflection::DataType::Int3:
                return DXGI_FORMAT_R32G32B32_SINT;
            case ShaderReflection::DataType::Int4:
                return DXGI_FORMAT_R32G32B32A32_SINT;
            case ShaderReflection::DataType::UInt:
                return DXGI_FORMAT_R32_UINT;
            case ShaderReflection::DataType::UInt2:
                return DXGI_FORMAT_R32G32_UINT;
            case ShaderReflection::DataType::UInt3:
                return DXGI_FORMAT_R32G32B32_UINT;
            case ShaderReflection::DataType::UInt4:
                return DXGI_FORMAT_R32G32B32A32_UINT;
            case ShaderReflection::DataType::Mat3:
            case ShaderReflection::DataType::Mat4:
            case ShaderReflection::DataType::Struct:
            case ShaderReflection::DataType::Unknown:
            default:
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    DXGI_FORMAT D3D12Convert::format(PixelFormat type)
    {
        switch (type)
        {
            case PixelFormat::Unknown: return DXGI_FORMAT_UNKNOWN;

            // 8-bit
            case PixelFormat::R8UNorm: return DXGI_FORMAT_R8_UNORM;
            case PixelFormat::R8SNorm: return DXGI_FORMAT_R8_SNORM;
            case PixelFormat::R8UInt:  return DXGI_FORMAT_R8_UINT;
            case PixelFormat::R8SInt:  return DXGI_FORMAT_R8_SINT;

            // 16-bit
            case PixelFormat::R16UInt:  return DXGI_FORMAT_R16_UINT;
            case PixelFormat::R16SInt:  return DXGI_FORMAT_R16_SINT;
            case PixelFormat::R16Float:  return DXGI_FORMAT_R16_FLOAT;

            case PixelFormat::RG8UNorm: return DXGI_FORMAT_R8G8_UNORM;
            case PixelFormat::RG8SNorm: return DXGI_FORMAT_R8G8_SNORM;
            case PixelFormat::RG8UInt:  return DXGI_FORMAT_R8G8_UINT;
            case PixelFormat::RG8SInt:  return DXGI_FORMAT_R8G8_SINT;

            // 32-bit (single/dual channel)
            case PixelFormat::R32UInt:  return DXGI_FORMAT_R32_UINT;
            case PixelFormat::R32SInt:  return DXGI_FORMAT_R32_SINT;
            case PixelFormat::R32Float: return DXGI_FORMAT_R32_FLOAT;

            case PixelFormat::RG16UInt:  return DXGI_FORMAT_R16G16_UINT;
            case PixelFormat::RG16SInt:  return DXGI_FORMAT_R16G16_SINT;
            case PixelFormat::RG16Float: return DXGI_FORMAT_R16G16_FLOAT;

            // 8-bit RGBA
            case PixelFormat::RGBA8UNorm:      return DXGI_FORMAT_R8G8B8A8_UNORM;
            case PixelFormat::RGBA8UNormSrgb:  return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            case PixelFormat::RGBA8SNorm:      return DXGI_FORMAT_R8G8B8A8_SNORM;
            case PixelFormat::RGBA8UInt:       return DXGI_FORMAT_R8G8B8A8_UINT;
            case PixelFormat::RGBA8SInt:       return DXGI_FORMAT_R8G8B8A8_SINT;

            case PixelFormat::BGRA8UNorm:      return DXGI_FORMAT_B8G8R8A8_UNORM;
            case PixelFormat::BGRA8UNormSrgb:  return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

            // Packed 32-bit
            case PixelFormat::RGB10A2UInt:   return DXGI_FORMAT_R10G10B10A2_UINT;
            case PixelFormat::RGB10A2UNorm:  return DXGI_FORMAT_R10G10B10A2_UNORM;
            case PixelFormat::RG11B10UFloat: return DXGI_FORMAT_R11G11B10_FLOAT;
            case PixelFormat::RGB9E5UFloat:  return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;

            // 64-bit
            case PixelFormat::RG32UInt:  return DXGI_FORMAT_R32G32_UINT;
            case PixelFormat::RG32SInt:  return DXGI_FORMAT_R32G32_SINT;
            case PixelFormat::RG32Float: return DXGI_FORMAT_R32G32_FLOAT;

            case PixelFormat::RGBA16UInt:  return DXGI_FORMAT_R16G16B16A16_UINT;
            case PixelFormat::RGBA16SInt:  return DXGI_FORMAT_R16G16B16A16_SINT;
            case PixelFormat::RGBA16Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;

            // 128-bit
            case PixelFormat::RGBA32UInt:  return DXGI_FORMAT_R32G32B32A32_UINT;
            case PixelFormat::RGBA32SInt:  return DXGI_FORMAT_R32G32B32A32_SINT;
            case PixelFormat::RGBA32Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;

            // Depth / stencil
            case PixelFormat::Depth16UNorm:          return DXGI_FORMAT_D16_UNORM;
            case PixelFormat::Depth32Float:          return DXGI_FORMAT_D32_FLOAT;
            case PixelFormat::Stencil8:               return DXGI_FORMAT_D24_UNORM_S8_UINT; // closest match is typically combined
            case PixelFormat::Depth24PlusStencil8:   return DXGI_FORMAT_D24_UNORM_S8_UINT;

            default:
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    D3D12_RESOURCE_DIMENSION D3D12Convert::resourceDimension(const TextureType type)
    {
        switch (type)
        {
            case TextureType::Texture1D:
                return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
            case TextureType::Texture2D:
            case TextureType::Texture2DArray:
            case TextureType::TextureCube:
            case TextureType::TextureCubeArray:
                return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            case TextureType::Texture3D:
                return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
            default:
                return D3D12_RESOURCE_DIMENSION_UNKNOWN;
        }
    }

    D3D12_FILL_MODE D3D12Convert::fillMode(const FillMode fillMode)
    {
        switch (fillMode)
        {
            case FillMode::Fill:
                return D3D12_FILL_MODE_SOLID;
            case FillMode::Line:
                return D3D12_FILL_MODE_WIREFRAME;
        }

        return D3D12_FILL_MODE_SOLID;
    }

    D3D12_CULL_MODE D3D12Convert::cullMode(const CullMode cullMode)
    {
        switch (cullMode)
        {
            case CullMode::Back:
                return D3D12_CULL_MODE_BACK;
            case CullMode::Front:
                return D3D12_CULL_MODE_FRONT;
            case CullMode::None:
                return D3D12_CULL_MODE_NONE;
        }

        return D3D12_CULL_MODE_NONE;
    }

    D3D12_PRIMITIVE_TOPOLOGY_TYPE D3D12Convert::primitiveType(const PrimitiveType type)
    {
        switch (type)
        {
            case PrimitiveType::TriangleList:
            case PrimitiveType::TriangleStrip:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            case PrimitiveType::LineList:
            case PrimitiveType::LineStrip:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            case PrimitiveType::PointList:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            default:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
        }
    }

    D3D12_BLEND D3D12Convert::srcBlendFactor(const BlendFactor factor)
    {
        switch (factor)
        {
            case BlendFactor::One:
                return D3D12_BLEND_ONE;

            case BlendFactor::Zero:
                return D3D12_BLEND_ZERO;

            case BlendFactor::SrcAlpha:
                return D3D12_BLEND_SRC_ALPHA;

            case BlendFactor::InvSrcAlpha:
                return D3D12_BLEND_INV_SRC_ALPHA;

            default:
                return D3D12_BLEND_ONE;
        }
    }

    D3D12_BLEND D3D12Convert::dstBlendFactor(const BlendFactor factor)
    {
        switch (factor)
        {
            case BlendFactor::One:
                return D3D12_BLEND_ONE;

            case BlendFactor::Zero:
                return D3D12_BLEND_ZERO;

            case BlendFactor::SrcAlpha:
                return D3D12_BLEND_SRC_ALPHA;

            case BlendFactor::InvSrcAlpha:
                return D3D12_BLEND_INV_SRC_ALPHA;

            default:
                return D3D12_BLEND_ZERO;
        }
    }

    D3D12_BLEND_OP D3D12Convert::blendOp(const BlendOp op)
    {
        switch (op)
        {
            case BlendOp::Add:
                return D3D12_BLEND_OP_ADD;

            case BlendOp::Subtract:
                return D3D12_BLEND_OP_SUBTRACT;

            case BlendOp::RevSubtract:
                return D3D12_BLEND_OP_REV_SUBTRACT;

            case BlendOp::Min:
                return D3D12_BLEND_OP_MIN;

            case BlendOp::Max:
                return D3D12_BLEND_OP_MAX;

            default:
                return D3D12_BLEND_OP_ADD;
        }
    }

    UINT8 D3D12Convert::writeMask(ColorWriteMask mask)
    {
        UINT8 result = 0;

        if (mask == ColorWriteMask::None)
            return 0;

        if ((static_cast<uint8_t>(mask) & static_cast<uint8_t>(ColorWriteMask::R)) != 0)
            result |= D3D12_COLOR_WRITE_ENABLE_RED;

        if ((static_cast<uint8_t>(mask) & static_cast<uint8_t>(ColorWriteMask::G)) != 0)
            result |= D3D12_COLOR_WRITE_ENABLE_GREEN;

        if ((static_cast<uint8_t>(mask) & static_cast<uint8_t>(ColorWriteMask::B)) != 0)
            result |= D3D12_COLOR_WRITE_ENABLE_BLUE;

        if ((static_cast<uint8_t>(mask) & static_cast<uint8_t>(ColorWriteMask::A)) != 0)
            result |= D3D12_COLOR_WRITE_ENABLE_ALPHA;

        return result;
    }

    D3D12_DESCRIPTOR_RANGE_TYPE D3D12Convert::rangeType(const D3D_SHADER_INPUT_TYPE type)
    {
        switch (type)
        {
            case D3D_SIT_CBUFFER:
                return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

            case D3D_SIT_TEXTURE:
            case D3D_SIT_TBUFFER:
            case D3D_SIT_STRUCTURED:
            case D3D_SIT_BYTEADDRESS:
                return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

            case D3D_SIT_UAV_RWTYPED:
            case D3D_SIT_UAV_RWSTRUCTURED:
            case D3D_SIT_UAV_RWBYTEADDRESS:
            case D3D_SIT_UAV_APPEND_STRUCTURED:
            case D3D_SIT_UAV_CONSUME_STRUCTURED:
            case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

            case D3D_SIT_SAMPLER:
                return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;

            default:
                return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        }
    }
}
