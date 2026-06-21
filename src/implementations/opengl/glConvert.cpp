#include "glConvert.h"

#include "glDevice.h"
#include "validation.h"
#include "GLFW/glfw3.h"

namespace urhi
{
    KeyCode GlConvert::keyCode(const int glfwKey)
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

    KeyMod GlConvert::keyMod(const int glfwMods)
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

    MouseButton GlConvert::mouseButton(const int glfwButton)
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

    GLenum GlConvert::pixelFormat(PixelFormat format)
    {
        switch (format)
        {
            // Single channel normalized / float
            case PixelFormat::R8UNorm:
            case PixelFormat::R8SNorm:
            case PixelFormat::R16Float:
            case PixelFormat::R32Float:
                return GL_RED;

            // Single channel integer
            case PixelFormat::R8UInt:
            case PixelFormat::R8SInt:
            case PixelFormat::R16UInt:
            case PixelFormat::R16SInt:
            case PixelFormat::R32UInt:
            case PixelFormat::R32SInt:
                return GL_RED_INTEGER;

            // Two channel normalized / float
            case PixelFormat::RG8UNorm:
            case PixelFormat::RG8SNorm:
            case PixelFormat::RG16Float:
            case PixelFormat::RG32Float:
                return GL_RG;

            // Two channel integer
            case PixelFormat::RG8UInt:
            case PixelFormat::RG8SInt:
            case PixelFormat::RG16UInt:
            case PixelFormat::RG16SInt:
            case PixelFormat::RG32UInt:
            case PixelFormat::RG32SInt:
                return GL_RG_INTEGER;

            // Four channel normalized / float
            case PixelFormat::RGBA8UNorm:
            case PixelFormat::RGBA8UNormSrgb:
            case PixelFormat::RGBA8SNorm:
            case PixelFormat::RGBA16Float:
            case PixelFormat::RGBA32Float:
            case PixelFormat::RGB10A2UNorm:
                return GL_RGBA;

            // Four channel integer
            case PixelFormat::RGBA8UInt:
            case PixelFormat::RGBA8SInt:
            case PixelFormat::RGBA16UInt:
            case PixelFormat::RGBA16SInt:
            case PixelFormat::RGBA32UInt:
            case PixelFormat::RGBA32SInt:
            case PixelFormat::RGB10A2UInt:
                return GL_RGBA_INTEGER;

            // BGRA
            case PixelFormat::BGRA8UNorm:
            case PixelFormat::BGRA8UNormSrgb:
                return GL_BGRA;

            // Packed RGB
            case PixelFormat::RGB9E5UFloat:
            case PixelFormat::RG11B10UFloat:
                return GL_RGB;

            // Depth / stencil
            case PixelFormat::Depth16UNorm:
            case PixelFormat::Depth32Float:
                return GL_DEPTH_COMPONENT;

            case PixelFormat::Stencil8:
                return GL_STENCIL_INDEX;

            case PixelFormat::Depth24PlusStencil8:
                return GL_DEPTH_STENCIL;

            default:
                URHI_VALIDATE(false, "Unknown PixelFormat");
                return GL_NONE;
        }
    }

    GLenum GlConvert::internalFormat(PixelFormat format)
    {
        switch (format)
        {
            // 8-bit single channel
            case PixelFormat::R8UNorm:  return GL_R8;
            case PixelFormat::R8SNorm:  return GL_R8_SNORM;
            case PixelFormat::R8UInt:   return GL_R8UI;
            case PixelFormat::R8SInt:   return GL_R8I;

            // 16-bit single channel
            case PixelFormat::R16UInt:  return GL_R16UI;
            case PixelFormat::R16SInt:  return GL_R16I;
            case PixelFormat::R16Float: return GL_R16F;

            // 16-bit two channel
            case PixelFormat::RG8UNorm: return GL_RG8;
            case PixelFormat::RG8SNorm: return GL_RG8_SNORM;
            case PixelFormat::RG8UInt:  return GL_RG8UI;
            case PixelFormat::RG8SInt:  return GL_RG8I;

            // 32-bit single channel
            case PixelFormat::R32UInt:  return GL_R32UI;
            case PixelFormat::R32SInt:  return GL_R32I;
            case PixelFormat::R32Float: return GL_R32F;

            // 32-bit two channel
            case PixelFormat::RG16UInt:  return GL_RG16UI;
            case PixelFormat::RG16SInt:  return GL_RG16I;
            case PixelFormat::RG16Float: return GL_RG16F;

            // 32-bit four channel
            case PixelFormat::RGBA8UNorm:     return GL_RGBA8;
            case PixelFormat::RGBA8UNormSrgb: return GL_SRGB8_ALPHA8;
            case PixelFormat::RGBA8SNorm:     return GL_RGBA8_SNORM;
            case PixelFormat::RGBA8UInt:      return GL_RGBA8UI;
            case PixelFormat::RGBA8SInt:      return GL_RGBA8I;

            // BGRA — GL has no native BGRA internal format,
            // store as RGBA8 and use GL_BGRA as the upload format in pixelFormat()
            case PixelFormat::BGRA8UNorm:     return GL_RGBA8;
            case PixelFormat::BGRA8UNormSrgb: return GL_SRGB8_ALPHA8;

            // Packed 32-bit
            case PixelFormat::RGB9E5UFloat:  return GL_RGB9_E5;
            case PixelFormat::RGB10A2UNorm:  return GL_RGB10_A2;
            case PixelFormat::RGB10A2UInt:   return GL_RGB10_A2UI;
            case PixelFormat::RG11B10UFloat: return GL_R11F_G11F_B10F;

            // 64-bit two channel
            case PixelFormat::RG32UInt:  return GL_RG32UI;
            case PixelFormat::RG32SInt:  return GL_RG32I;
            case PixelFormat::RG32Float: return GL_RG32F;

            // 64-bit four channel
            case PixelFormat::RGBA16UInt:  return GL_RGBA16UI;
            case PixelFormat::RGBA16SInt:  return GL_RGBA16I;
            case PixelFormat::RGBA16Float: return GL_RGBA16F;

            // 128-bit four channel
            case PixelFormat::RGBA32UInt:  return GL_RGBA32UI;
            case PixelFormat::RGBA32SInt:  return GL_RGBA32I;
            case PixelFormat::RGBA32Float: return GL_RGBA32F;

            // Depth / stencil
            case PixelFormat::Depth16UNorm:       return GL_DEPTH_COMPONENT16;
            case PixelFormat::Depth32Float:       return GL_DEPTH_COMPONENT32F;
            case PixelFormat::Stencil8:           return GL_STENCIL_INDEX8;
            case PixelFormat::Depth24PlusStencil8: return GL_DEPTH24_STENCIL8;

            default:
                URHI_VALIDATE(false, "Unknown PixelFormat");
                return GL_NONE;
        }
    }

    GLenum GlConvert::pixelType(PixelFormat format)
    {
        switch (format)
        {
            case PixelFormat::R8UNorm:
            case PixelFormat::R8UInt:
            case PixelFormat::RG8UNorm:
            case PixelFormat::RG8UInt:
            case PixelFormat::RGBA8UNorm:
            case PixelFormat::RGBA8UNormSrgb:
            case PixelFormat::RGBA8UInt:
            case PixelFormat::BGRA8UNorm:
            case PixelFormat::BGRA8UNormSrgb:
            case PixelFormat::Stencil8:
                return GL_UNSIGNED_BYTE;

            case PixelFormat::R8SNorm:
            case PixelFormat::R8SInt:
            case PixelFormat::RG8SNorm:
            case PixelFormat::RG8SInt:
            case PixelFormat::RGBA8SNorm:
            case PixelFormat::RGBA8SInt:
                return GL_BYTE;

            case PixelFormat::R16UInt:
            case PixelFormat::RG16UInt:
            case PixelFormat::RGBA16UInt:
            case PixelFormat::Depth16UNorm:
                return GL_UNSIGNED_SHORT;

            case PixelFormat::R16SInt:
            case PixelFormat::RG16SInt:
            case PixelFormat::RGBA16SInt:
                return GL_SHORT;

            case PixelFormat::R16Float:
            case PixelFormat::RG16Float:
            case PixelFormat::RGBA16Float:
                return GL_HALF_FLOAT;

            case PixelFormat::R32UInt:
            case PixelFormat::RG32UInt:
            case PixelFormat::RGBA32UInt:
                return GL_UNSIGNED_INT;

            case PixelFormat::R32SInt:
            case PixelFormat::RG32SInt:
            case PixelFormat::RGBA32SInt:
                return GL_INT;

            case PixelFormat::R32Float:
            case PixelFormat::RG32Float:
            case PixelFormat::RGBA32Float:
            case PixelFormat::Depth32Float:
                return GL_FLOAT;

            // Packed types
            case PixelFormat::RGB9E5UFloat:
                return GL_UNSIGNED_INT_5_9_9_9_REV;

            case PixelFormat::RGB10A2UNorm:
            case PixelFormat::RGB10A2UInt:
                return GL_UNSIGNED_INT_2_10_10_10_REV;

            case PixelFormat::RG11B10UFloat:
                return GL_UNSIGNED_INT_10F_11F_11F_REV;

            case PixelFormat::Depth24PlusStencil8:
                return GL_UNSIGNED_INT_24_8;

            default:
                URHI_VALIDATE(false, "Unknown PixelFormat");
                return GL_NONE;
        }
    }

    GLenum GlConvert::bufferUsage(const BufferUsage usage)
    {
        // Explicit hints take priority
        if (hasFlag(usage, BufferUsage::Static))  return GL_STATIC_DRAW;
        if (hasFlag(usage, BufferUsage::Dynamic)) return GL_DYNAMIC_DRAW;

        // Guess based on usage type:
        // Uniform / storage / indirect are typically updated frequently
        if (hasFlag(usage, BufferUsage::Uniform))  return GL_DYNAMIC_DRAW;
        if (hasFlag(usage, BufferUsage::Storage))  return GL_DYNAMIC_DRAW;
        if (hasFlag(usage, BufferUsage::Indirect)) return GL_DYNAMIC_DRAW;

        // Vertex / index buffers are typically uploaded once
        if (hasFlag(usage, BufferUsage::Vertex)) return GL_STATIC_DRAW;
        if (hasFlag(usage, BufferUsage::Index))  return GL_STATIC_DRAW;

        return GL_STATIC_DRAW;
    }

    GLenum GlConvert::bufferTarget(const BufferUsage usage)
    {
        // Priority order matters for multi-flag buffers —
        // pick the most specific / restrictive target
        if (hasFlag(usage, BufferUsage::Index))    return GL_ELEMENT_ARRAY_BUFFER;
        if (hasFlag(usage, BufferUsage::Vertex))   return GL_ARRAY_BUFFER;
        if (hasFlag(usage, BufferUsage::Uniform))  return GL_UNIFORM_BUFFER;
        if (hasFlag(usage, BufferUsage::Storage))  return GL_SHADER_STORAGE_BUFFER;
        if (hasFlag(usage, BufferUsage::Indirect)) return GL_DRAW_INDIRECT_BUFFER;

        URHI_VALIDATE(false, "BufferUsage has no target-relevant flags");
        return GL_NONE;
    }

    GLenum GlConvert::primitiveType(PrimitiveType type)
    {
        switch (type)
        {
            case PrimitiveType::TriangleList:  return GL_TRIANGLES;
            case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
            case PrimitiveType::LineList:      return GL_LINES;
            case PrimitiveType::LineStrip:     return GL_LINE_STRIP;
            case PrimitiveType::PointList:     return GL_POINTS;
            default:
                URHI_VALIDATE(false, "Unknown PrimitiveType");
            return GL_NONE;
        }
    }

    GLint GlConvert::componentCount(const ShaderReflection::DataType type)
    {
        switch (type)
        {
            case ShaderReflection::DataType::Float:  case ShaderReflection::DataType::Int:  case ShaderReflection::DataType::UInt:  return 1;
            case ShaderReflection::DataType::Float2: case ShaderReflection::DataType::Int2: case ShaderReflection::DataType::UInt2: return 2;
            case ShaderReflection::DataType::Float3: case ShaderReflection::DataType::Int3: case ShaderReflection::DataType::UInt3: return 3;
            case ShaderReflection::DataType::Float4: case ShaderReflection::DataType::Int4: case ShaderReflection::DataType::UInt4: return 4;

            // Matrices are set up as N consecutive float4 attributes
            case ShaderReflection::DataType::Mat3: return 3;
            case ShaderReflection::DataType::Mat4: return 4;

            default:
                URHI_VALIDATE(false, "Invalid DataType for vertex attribute");
            return 0;
        }
    }

    GLenum GlConvert::vertexBaseType(const ShaderReflection::DataType type)
    {
        switch (type)
        {
            case ShaderReflection::DataType::Float:
            case ShaderReflection::DataType::Float2:
            case ShaderReflection::DataType::Float3:
            case ShaderReflection::DataType::Float4:
            case ShaderReflection::DataType::Mat3:
            case ShaderReflection::DataType::Mat4:
                return GL_FLOAT;

            case ShaderReflection::DataType::Int:
            case ShaderReflection::DataType::Int2:
            case ShaderReflection::DataType::Int3:
            case ShaderReflection::DataType::Int4:
                return GL_INT;

            case ShaderReflection::DataType::UInt:
            case ShaderReflection::DataType::UInt2:
            case ShaderReflection::DataType::UInt3:
            case ShaderReflection::DataType::UInt4:
                return GL_UNSIGNED_INT;

            default:
                URHI_VALIDATE(false, "Invalid DataType for vertex attribute");
            return GL_NONE;
        }
    }

    bool GlConvert::isIntegerType(const ShaderReflection::DataType type)
    {
        switch (type)
        {
            case ShaderReflection::DataType::Int:  case ShaderReflection::DataType::Int2:
            case ShaderReflection::DataType::Int3:  case ShaderReflection::DataType::Int4:
            case ShaderReflection::DataType::UInt: case ShaderReflection::DataType::UInt2:
            case ShaderReflection::DataType::UInt3: case ShaderReflection::DataType::UInt4:
                return true;
            default:
                return false;
        }
    }

    GLenum GlConvert::filter(TextureFilter texFilter, MipmapFilter mipFilter)
    {
        if (mipFilter == MipmapFilter::None)
        {
            switch (texFilter)
            {
                case TextureFilter::Nearest: return GL_NEAREST;
                case TextureFilter::Linear:  return GL_LINEAR;
            }
        }

        if (texFilter == TextureFilter::Nearest)
        {
            switch (mipFilter)
            {
                case MipmapFilter::Nearest: return GL_NEAREST_MIPMAP_NEAREST;
                case MipmapFilter::Linear:  return GL_NEAREST_MIPMAP_LINEAR;
                default: break;
            }
        }

        if (texFilter == TextureFilter::Linear)
        {
            switch (mipFilter)
            {
                case MipmapFilter::Nearest: return GL_LINEAR_MIPMAP_NEAREST;
                case MipmapFilter::Linear:  return GL_LINEAR_MIPMAP_LINEAR;
                default: break;
            }
        }

        URHI_VALIDATE(false, "Unknown TextureFilter/MipmapFilter combination");
        return GL_NONE;
    }

    GLenum GlConvert::filter(const TextureFilter texFilter)
    {
        switch (texFilter)
        {
            case TextureFilter::Nearest: return GL_NEAREST;
            case TextureFilter::Linear:  return GL_LINEAR;
            default:
                URHI_VALIDATE(false, "Unknown TextureFilter");
                return GL_NONE;
        }
    }

    GLenum GlConvert::wrapMode(const AddressMode mode)
    {
        switch (mode)
        {
            case AddressMode::Repeat:            return GL_REPEAT;
            case AddressMode::MirroredRepeat:    return GL_MIRRORED_REPEAT;
            case AddressMode::ClampToEdge:       return GL_CLAMP_TO_EDGE;
            case AddressMode::ClampToBorder:     return GL_CLAMP_TO_BORDER;
            case AddressMode::MirrorClampToEdge: return GL_MIRROR_CLAMP_TO_EDGE;
            default:
                URHI_VALIDATE(false, "Unknown AddressMode");
                return GL_NONE;
        }
    }

    GLenum GlConvert::compareFunc(const CompareOp op)
    {
        switch (op)
        {
            case CompareOp::Never:          return GL_NEVER;
            case CompareOp::Less:           return GL_LESS;
            case CompareOp::Equal:          return GL_EQUAL;
            case CompareOp::LessOrEqual:    return GL_LEQUAL;
            case CompareOp::Greater:        return GL_GREATER;
            case CompareOp::NotEqual:       return GL_NOTEQUAL;
            case CompareOp::GreaterOrEqual: return GL_GEQUAL;
            case CompareOp::Always:         return GL_ALWAYS;
            default:
                URHI_VALIDATE(false, "Unknown CompareOp");
                return GL_NONE;
        }
    }

    GLenum GlConvert::shaderStage(const ShaderStage stage)
    {
        switch (stage)
        {
            case ShaderStage::Vertex:                 return GL_VERTEX_SHADER;
            case ShaderStage::Fragment:               return GL_FRAGMENT_SHADER;
            case ShaderStage::Geometry:               return GL_GEOMETRY_SHADER;
            case ShaderStage::Compute:                return GL_COMPUTE_SHADER;
            // case ShaderStage::TessellationControl:    return GL_TESS_CONTROL_SHADER;
            // case ShaderStage::TessellationEvaluation: return GL_TESS_EVALUATION_SHADER;
            default:
                URHI_VALIDATE(false, "Unknown ShaderStage");
            return GL_NONE;
        }
    }

    GLenum GlConvert::textureType(TextureType type)
    {
        switch (type)
        {
            case TextureType::Texture1D:        return GL_TEXTURE_1D;
            case TextureType::Texture2D:        return GL_TEXTURE_2D;
            case TextureType::Texture3D:        return GL_TEXTURE_3D;
            case TextureType::Texture2DArray:   return GL_TEXTURE_2D_ARRAY;
            case TextureType::TextureCube:      return GL_TEXTURE_CUBE_MAP;
            case TextureType::TextureCubeArray: return GL_TEXTURE_CUBE_MAP_ARRAY;
            default:
                URHI_VALIDATE(false, "Unknown TextureType");
            return GL_NONE;
        }
    }

    GLenum GlConvert::indexFormat(const IndexFormat format)
    {
        switch (format)
        {
            case IndexFormat::UInt16:
                return GL_UNSIGNED_SHORT;
            case IndexFormat::UInt32:
                return GL_UNSIGNED_INT;
            default:
                URHI_VALIDATE(false, "Unknown index format");
                return 0;
        }
    }

    GLenum GlConvert::sizeOf(const IndexFormat format)
    {
        switch (format)
        {
            case IndexFormat::UInt16:
                return 2;
            case IndexFormat::UInt32:
                return 4;
            default:
                URHI_VALIDATE(false, "Unknown index format");
            return 0;
        }
    }

    GLenum GlConvert::blendOp(const BlendOp op)
    {
        switch(op)
        {
            case BlendOp::Add:         return GL_FUNC_ADD;
            case BlendOp::Subtract:    return GL_FUNC_SUBTRACT;
            case BlendOp::RevSubtract: return GL_FUNC_REVERSE_SUBTRACT;
            case BlendOp::Min:         return GL_MIN;
            case BlendOp::Max:         return GL_MAX;
            default:                   return GL_FUNC_ADD;
        }
    }

    GLenum GlConvert::blendFactor(const BlendFactor factor)
    {
        switch(factor)
        {
            case BlendFactor::One:         return GL_ONE;
            case BlendFactor::Zero:        return GL_ZERO;
            case BlendFactor::SrcAlpha:    return GL_SRC_ALPHA;
            case BlendFactor::InvSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
            default:                       return GL_ONE;
        }
    }

    GLenum GlConvert::compareOp(const CompareOp op)
    {
        switch(op)
        {
            case CompareOp::Never:        return GL_NEVER;
            case CompareOp::Less:         return GL_LESS;
            case CompareOp::Equal:        return GL_EQUAL;
            case CompareOp::LessOrEqual:  return GL_LEQUAL;
            case CompareOp::Greater:      return GL_GREATER;
            case CompareOp::NotEqual:     return GL_NOTEQUAL;
            case CompareOp::GreaterOrEqual:return GL_GEQUAL;
            case CompareOp::Always:       return GL_ALWAYS;
            default:                      return GL_ALWAYS;
        }
    }

    GLenum GlConvert::fillMode(const FillMode mode)
    {
        switch(mode)
        {
            case FillMode::Fill: return GL_FILL;
            case FillMode::Line: return GL_LINE;
            default:             return GL_FILL;
        }
    }

    GLenum GlConvert::cullMode(const CullMode mode)
    {
        switch(mode)
        {
            case CullMode::Back:  return GL_BACK;
            case CullMode::Front: return GL_FRONT;
            case CullMode::None:  return GL_NONE;
            default:              return GL_BACK;
        }
    }

    GLenum GlConvert::bufferBarrierBit(const BufferUsage usage)
    {
        switch(usage)
        {
            case BufferUsage::Vertex:  return GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
            case BufferUsage::Index:  return GL_ELEMENT_ARRAY_BARRIER_BIT;
            case BufferUsage::Indirect: return GL_COMMAND_BARRIER_BIT;
            case BufferUsage::Uniform: return GL_UNIFORM_BARRIER_BIT;
            case BufferUsage::Storage: return GL_SHADER_STORAGE_BARRIER_BIT;
            case BufferUsage::None:  return GL_NONE;
            default:              return GL_BACK;
        }
    }

    GLenum GlConvert::resourceAccess(const ResourceAccess access)
    {
        switch (access)
        {
            case ResourceAccess::ReadOnly:
                return GL_READ_ONLY;
            case ResourceAccess::WriteOnly:
                return GL_WRITE_ONLY;
            case ResourceAccess::ReadWrite:
            default:
                return GL_READ_WRITE;
        }
    }
}
