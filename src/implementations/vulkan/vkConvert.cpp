#include "vkConvert.h"

#include "vkDevice.h"
#include "GLFW/glfw3.h"

namespace urhi
{
    KeyCode VkConvert::keyCode(const int glfwKey)
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

    KeyMod VkConvert::keyMod(const int glfwMods)
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

    MouseButton VkConvert::mouseButton(const int glfwButton)
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

    vk::PresentModeKHR VkConvert::presentMode(const PresentMode presentMode)
    {
        switch (presentMode)
        {
            case PresentMode::TripleBuffer:
                return vk::PresentModeKHR::eMailbox;
            case PresentMode::VSync:
                return vk::PresentModeKHR::eFifo;
            case PresentMode::AdaptiveVSync:
                return vk::PresentModeKHR::eFifoRelaxed;
            case PresentMode::NoVSync:
                return vk::PresentModeKHR::eImmediate;
        }

        return vk::PresentModeKHR::eImmediate;
    }

    vk::ColorSpaceKHR VkConvert::colorSpace(const ColorSpace colorSpace)
    {
        switch (colorSpace)
        {
            case ColorSpace::Srgb:          return vk::ColorSpaceKHR::eSrgbNonlinear;
            case ColorSpace::DisplayP3:     return vk::ColorSpaceKHR::eDisplayP3NonlinearEXT;
            case ColorSpace::HDRLinear:     return vk::ColorSpaceKHR::eExtendedSrgbLinearEXT;
            case ColorSpace::HDR10bit:      return vk::ColorSpaceKHR::eHdr10St2084EXT;
            case ColorSpace::HDRBroadcast:  return vk::ColorSpaceKHR::eHdr10HlgEXT;
            case ColorSpace::AdobeRgb:      return vk::ColorSpaceKHR::eAdobergbNonlinearEXT;
            case ColorSpace::Raw:           return vk::ColorSpaceKHR::ePassThroughEXT;
        }

        return vk::ColorSpaceKHR::ePassThroughEXT;
    }

    vk::Format VkConvert::pixelFormat(const PixelFormat pixelFormat, const VkDevice* device)
    {
        switch (pixelFormat)
        {
            // 8-bit R
            case PixelFormat::R8UNorm:  return vk::Format::eR8Unorm;
            case PixelFormat::R8SNorm:  return vk::Format::eR8Snorm;
            case PixelFormat::R8UInt:   return vk::Format::eR8Uint;
            case PixelFormat::R8SInt:   return vk::Format::eR8Sint;

            // 16-bit R
            case PixelFormat::R16UInt:  return vk::Format::eR16Uint;
            case PixelFormat::R16SInt:  return vk::Format::eR16Sint;
            case PixelFormat::R16Float: return vk::Format::eR16Sfloat;

            // 32-bit R
            case PixelFormat::R32Float: return vk::Format::eR32Sfloat;
            case PixelFormat::R32UInt:  return vk::Format::eR32Uint;
            case PixelFormat::R32SInt:  return vk::Format::eR32Sint;

            // 8-bit RG
            case PixelFormat::RG8UNorm: return vk::Format::eR8G8Unorm;
            case PixelFormat::RG8SNorm: return vk::Format::eR8G8Snorm;
            case PixelFormat::RG8UInt:  return vk::Format::eR8G8Uint;
            case PixelFormat::RG8SInt:  return vk::Format::eR8G8Sint;

            // 16-bit RG
            case PixelFormat::RG16UInt:         return vk::Format::eR16G16Uint;
            case PixelFormat::RG16SInt:         return vk::Format::eR16G16Sint;
            case PixelFormat::RG16Float:        return vk::Format::eR16G16Sfloat;

            // 32-bit RG
            case PixelFormat::RG32Float:        return vk::Format::eR32G32Sfloat;
            case PixelFormat::RG32UInt:         return vk::Format::eR32G32Uint;
            case PixelFormat::RG32SInt:         return vk::Format::eR32G32Sint;

            // 8-bit RGBA / BGRA
            case PixelFormat::RGBA8UNorm:       return vk::Format::eR8G8B8A8Unorm;
            case PixelFormat::RGBA8SNorm:       return vk::Format::eR8G8B8A8Snorm;
            case PixelFormat::RGBA8UInt:        return vk::Format::eR8G8B8A8Uint;
            case PixelFormat::RGBA8SInt:        return vk::Format::eR8G8B8A8Sint;
            case PixelFormat::RGBA8UNormSrgb:   return vk::Format::eR8G8B8A8Srgb;
            case PixelFormat::BGRA8UNorm:       return vk::Format::eB8G8R8A8Unorm;
            case PixelFormat::BGRA8UNormSrgb:   return vk::Format::eB8G8R8A8Srgb;

            // 16-bit RGBA
            case PixelFormat::RGBA16UInt:    return vk::Format::eR16G16B16A16Uint;
            case PixelFormat::RGBA16SInt:    return vk::Format::eR16G16B16A16Sint;
            case PixelFormat::RGBA16Float:   return vk::Format::eR16G16B16A16Sfloat;

            // 32-bit RGBA
            case PixelFormat::RGBA32Float:    return vk::Format::eR32G32B32A32Sfloat;
            case PixelFormat::RGBA32UInt:     return vk::Format::eR32G32B32A32Uint;
            case PixelFormat::RGBA32SInt:     return vk::Format::eR32G32B32A32Sint;

            // Packed / Special
            case PixelFormat::RGB10A2UNorm:   return vk::Format::eA2B10G10R10UnormPack32;
            case PixelFormat::RG11B10UFloat:  return vk::Format::eB10G11R11UfloatPack32;
            case PixelFormat::RGB9E5UFloat:   return vk::Format::eE5B9G9R9UfloatPack32;
            case PixelFormat::RGB10A2UInt:    return vk::Format::eA2R10G10B10UintPack32;


            // Depth / Stencil
            case PixelFormat::Depth24PlusStencil8: return device->depth24PlusStencil8Format();

            case PixelFormat::Depth32Float:     return vk::Format::eD32Sfloat;
            case PixelFormat::Depth16UNorm:     return vk::Format::eD16Unorm;

            case PixelFormat::Stencil8: return vk::Format::eS8Uint;

            case PixelFormat::Unknown:
            default:                               return vk::Format::eUndefined;
        }
    }

    PixelFormat VkConvert::pixelFormat(const vk::Format format, const VkDevice* device)
    {
        switch (format)
        {
            // 8-bit R
            case vk::Format::eR8Unorm:              return PixelFormat::R8UNorm;
            case vk::Format::eR8Snorm:              return PixelFormat::R8SNorm;
            case vk::Format::eR8Uint:               return PixelFormat::R8UInt;
            case vk::Format::eR8Sint:               return PixelFormat::R8SInt;

            // 16-bit R
            case vk::Format::eR16Uint:              return PixelFormat::R16UInt;
            case vk::Format::eR16Sint:              return PixelFormat::R16SInt;
            case vk::Format::eR16Sfloat:            return PixelFormat::R16Float;

            // 32-bit R
            case vk::Format::eR32Sfloat:            return PixelFormat::R32Float;
            case vk::Format::eR32Uint:              return PixelFormat::R32UInt;
            case vk::Format::eR32Sint:              return PixelFormat::R32SInt;

            // 8-bit RG
            case vk::Format::eR8G8Unorm:            return PixelFormat::RG8UNorm;
            case vk::Format::eR8G8Snorm:            return PixelFormat::RG8SNorm;
            case vk::Format::eR8G8Uint:             return PixelFormat::RG8UInt;
            case vk::Format::eR8G8Sint:             return PixelFormat::RG8SInt;

            // 16-bit RG
            case vk::Format::eR16G16Uint:           return PixelFormat::RG16UInt;
            case vk::Format::eR16G16Sint:           return PixelFormat::RG16SInt;
            case vk::Format::eR16G16Sfloat:         return PixelFormat::RG16Float;

            // 32-bit RG
            case vk::Format::eR32G32Sfloat:         return PixelFormat::RG32Float;
            case vk::Format::eR32G32Uint:           return PixelFormat::RG32UInt;
            case vk::Format::eR32G32Sint:           return PixelFormat::RG32SInt;

            // 8-bit RGBA / BGRA
            case vk::Format::eR8G8B8A8Unorm:        return PixelFormat::RGBA8UNorm;
            case vk::Format::eR8G8B8A8Snorm:        return PixelFormat::RGBA8SNorm;
            case vk::Format::eR8G8B8A8Uint:         return PixelFormat::RGBA8UInt;
            case vk::Format::eR8G8B8A8Sint:         return PixelFormat::RGBA8SInt;
            case vk::Format::eR8G8B8A8Srgb:         return PixelFormat::RGBA8UNormSrgb;
            case vk::Format::eB8G8R8A8Unorm:        return PixelFormat::BGRA8UNorm;
            case vk::Format::eB8G8R8A8Srgb:         return PixelFormat::BGRA8UNormSrgb;

            // 16-bit RGBA
            case vk::Format::eR16G16B16A16Uint:     return PixelFormat::RGBA16UInt;
            case vk::Format::eR16G16B16A16Sint:     return PixelFormat::RGBA16SInt;
            case vk::Format::eR16G16B16A16Sfloat:   return PixelFormat::RGBA16Float;

            // 32-bit RGBA
            case vk::Format::eR32G32B32A32Sfloat:   return PixelFormat::RGBA32Float;
            case vk::Format::eR32G32B32A32Uint:     return PixelFormat::RGBA32UInt;
            case vk::Format::eR32G32B32A32Sint:     return PixelFormat::RGBA32SInt;

            // Packed / Special
            case vk::Format::eA2B10G10R10UnormPack32: return PixelFormat::RGB10A2UNorm;
            case vk::Format::eB10G11R11UfloatPack32:  return PixelFormat::RG11B10UFloat;
            case vk::Format::eE5B9G9R9UfloatPack32:   return PixelFormat::RGB9E5UFloat;
            case vk::Format::eA2R10G10B10UintPack32:  return PixelFormat::RGB10A2UInt;

            // Depth / Stencil
            case vk::Format::eD32Sfloat:            return PixelFormat::Depth32Float;
            case vk::Format::eD16Unorm:             return PixelFormat::Depth16UNorm;
            case vk::Format::eS8Uint:               return PixelFormat::Stencil8;

            // Handle the dynamic Depth24 cases
            // These require a check against the device's specific chosen formats
            default:
                if (format == device->depth24PlusStencil8Format())
                    return PixelFormat::Depth24PlusStencil8;

                return PixelFormat::Unknown;
        }
    }

    vk::BufferUsageFlags VkConvert::bufferUsage(const BufferUsage usage)
    {
        constexpr vk::BufferUsageFlags flags = vk::BufferUsageFlagBits::eShaderDeviceAddress;

        if (hasFlag(usage, BufferUsage::Vertex))   return flags | vk::BufferUsageFlagBits::eVertexBuffer;
        if (hasFlag(usage, BufferUsage::Index))    return flags | vk::BufferUsageFlagBits::eIndexBuffer;
        if (hasFlag(usage, BufferUsage::Uniform))  return flags | vk::BufferUsageFlagBits::eUniformBuffer;
        if (hasFlag(usage, BufferUsage::Storage))  return flags | vk::BufferUsageFlagBits::eStorageBuffer;
        if (hasFlag(usage, BufferUsage::Indirect)) return flags | vk::BufferUsageFlagBits::eIndirectBuffer;

        return flags;
    }

    vk::DescriptorType VkConvert::descriptorType(const refl::ResType type)
    {
        switch (type)
        {
            case refl::ResType::Buffer:
                return vk::DescriptorType::eStorageBuffer;
            case refl::ResType::CBuffer:
                return vk::DescriptorType::eUniformBuffer;
            case refl::ResType::Texture:
                return vk::DescriptorType::eSampledImage;
            case refl::ResType::Sampler:
                return vk::DescriptorType::eSampler;
            case refl::ResType::Image:
                return vk::DescriptorType::eStorageImage;
            default:
                return vk::DescriptorType::eUniformBuffer;
        }
    }

    vk::ShaderStageFlags VkConvert::shaderStage(const ShaderStage stage)
    {
        vk::ShaderStageFlags flags{};

        if ((stage & ShaderStage::Vertex) == ShaderStage::Vertex)
            flags |= vk::ShaderStageFlagBits::eVertex;
        if ((stage & ShaderStage::Fragment) == ShaderStage::Fragment)
            flags |= vk::ShaderStageFlagBits::eFragment;
        if ((stage & ShaderStage::Compute) == ShaderStage::Compute)
            flags |= vk::ShaderStageFlagBits::eCompute;

        return flags;
    }

    vk::ShaderStageFlagBits VkConvert::shaderStageBits(ShaderStage stage)
    {
        if (stage == ShaderStage::Vertex)
            return vk::ShaderStageFlagBits::eVertex;
        if (stage == ShaderStage::Fragment)
            return vk::ShaderStageFlagBits::eFragment;
        if (stage == ShaderStage::Compute)
            return vk::ShaderStageFlagBits::eCompute;

        return vk::ShaderStageFlagBits::eAll;
    }

    vk::Format VkConvert::format(const refl::DataType type)
    {
        switch (type)
        {
            case refl::DataType::Float:  return vk::Format::eR32Sfloat;
            case refl::DataType::Float2: return vk::Format::eR32G32Sfloat;
            case refl::DataType::Float3: return vk::Format::eR32G32B32Sfloat;;
            case refl::DataType::Float4: return vk::Format::eR32G32B32A32Sfloat;;
            case refl::DataType::Int:    return vk::Format::eR32Sint;
            case refl::DataType::Int2:   return vk::Format::eR32G32Sint;
            case refl::DataType::Int3:   return vk::Format::eR32G32B32Sint;
            case refl::DataType::Int4:   return vk::Format::eR32G32B32A32Sint;
            case refl::DataType::UInt:   return vk::Format::eR32Uint;
            case refl::DataType::UInt2:  return vk::Format::eR32G32Uint;
            case refl::DataType::UInt3:  return vk::Format::eR32G32B32Uint;
            case refl::DataType::UInt4:  return vk::Format::eR32G32B32A32Uint;
            default: return vk::Format::eUndefined;;
        }
    }

    vk::AttachmentLoadOp VkConvert::loadOp(const LoadOp loadOp)
    {
        switch (loadOp)
        {
            case LoadOp::Clear:
                return vk::AttachmentLoadOp::eClear;
            case LoadOp::Load:
                return vk::AttachmentLoadOp::eLoad;
            default:
                return vk::AttachmentLoadOp::eDontCare;
        }
    }

    vk::AttachmentStoreOp VkConvert::storeOp(const StoreOp storeOp)
    {
        switch (storeOp)
        {
            case StoreOp::Store:
                return vk::AttachmentStoreOp::eStore;
            default:
                return vk::AttachmentStoreOp::eDontCare;
        }
    }

    vk::ClearValue VkConvert::clearValue(ClearValue clearValue)
    {
        vk::ClearValue out{};
        std::visit([&out]<typename T>(T const &v)
        {
            if constexpr (std::is_same_v<T, ClearColorFloat>)
            {
                out.color.float32[0] = v.r;
                out.color.float32[1] = v.g;
                out.color.float32[2] = v.b;
                out.color.float32[3] = v.a;
            }
            else if constexpr (std::is_same_v<T, ClearColorInt>)
            {
                out.color.int32[0] = v.r;
                out.color.int32[1] = v.g;
                out.color.int32[2] = v.b;
                out.color.int32[3] = v.a;
            }
            else if constexpr (std::is_same_v<T, ClearColorUint>)
            {
                out.color.uint32[0] = v.r;
                out.color.uint32[1] = v.g;
                out.color.uint32[2] = v.b;
                out.color.uint32[3] = v.a;
            }
        }, clearValue);
        return out;
    }

    vk::ImageType VkConvert::textureType(const TextureType textureType)
    {
        switch (textureType)
        {
            case TextureType::Texture1D:
                return vk::ImageType::e1D;
            case TextureType::Texture2D:
                return vk::ImageType::e2D;
            case TextureType::Texture3D:
                return vk::ImageType::e3D;
            default:
                return vk::ImageType::e2D;
        }
    }

    vk::ImageUsageFlags VkConvert::textureUsage(const TextureUsage usage)
    {
        vk::ImageUsageFlags flags{};

        if ((usage & TextureUsage::Sampled) == TextureUsage::Sampled)
            flags |= vk::ImageUsageFlagBits::eSampled;
        if ((usage & TextureUsage::ColorTarget) == TextureUsage::ColorTarget)
            flags |= vk::ImageUsageFlagBits::eColorAttachment;
        if ((usage & TextureUsage::DepthStencilTarget) == TextureUsage::DepthStencilTarget)
            flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
        if ((usage & TextureUsage::Storage) == TextureUsage::Storage)
            flags |= vk::ImageUsageFlagBits::eStorage;

        return flags;
    }

    vk::ImageAspectFlags VkConvert::aspectMask(const PixelFormat format)
    {
        switch (format)
        {
            case PixelFormat::Depth24PlusStencil8:
                return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
            case PixelFormat::Depth32Float:
            case PixelFormat::Depth16UNorm:
                return vk::ImageAspectFlagBits::eDepth;
            default:
                return vk::ImageAspectFlagBits::eColor;
        }
    }

    vk::ImageViewType VkConvert::textureViewType(const TextureType textureType)
    {
        switch (textureType)
        {
            case TextureType::Texture1D:
                return vk::ImageViewType::e1D;
            case TextureType::Texture2D:
                return vk::ImageViewType::e2D;
            case TextureType::Texture3D:
                return vk::ImageViewType::e3D;
            default:
                return vk::ImageViewType::e2D;
        }
    }

    vk::PrimitiveTopology VkConvert::primitiveType(const PrimitiveType type)
    {
        switch (type)
        {
            case PrimitiveType::TriangleList:
                return vk::PrimitiveTopology::eTriangleList;
            case PrimitiveType::TriangleStrip:
                return vk::PrimitiveTopology::eTriangleStrip;
            case PrimitiveType::LineList:
                return vk::PrimitiveTopology::eLineList;
            case PrimitiveType::LineStrip:
                return vk::PrimitiveTopology::eLineStrip;
            case PrimitiveType::PointList:
                return vk::PrimitiveTopology::ePointList;
            default:
                return vk::PrimitiveTopology::eTriangleList;
        }
    }

    vk::PolygonMode VkConvert::fillMode(const FillMode fillMode)
    {
        switch (fillMode)
        {
            case FillMode::Fill:
                return vk::PolygonMode::eFill;
            case FillMode::Line:
                return vk::PolygonMode::eLine;
            default:
                return vk::PolygonMode::eFill;
        }
    }

    vk::CullModeFlags VkConvert::cullMode(CullMode cullMode)
    {
        switch (cullMode)
        {
            case CullMode::None:
                return vk::CullModeFlagBits::eNone;
            case CullMode::Front:
                return vk::CullModeFlagBits::eFront;
            case CullMode::Back:
                return vk::CullModeFlagBits::eBack;
            default:
                return vk::CullModeFlagBits::eNone;
        }
    }

    vk::CompareOp VkConvert::compareOp(const CompareOp compareOp)
    {
        switch (compareOp)
        {
            case CompareOp::Less:
                return vk::CompareOp::eLess;
            case CompareOp::LessOrEqual:
                return vk::CompareOp::eLessOrEqual;
            case CompareOp::Greater:
                return vk::CompareOp::eGreater;
            case CompareOp::GreaterOrEqual:
                return vk::CompareOp::eGreaterOrEqual;
            case CompareOp::Never:
                return vk::CompareOp::eNever;
            case CompareOp::Always:
                return vk::CompareOp::eAlways;
            case CompareOp::NotEqual:
                return vk::CompareOp::eNotEqual;
            case CompareOp::Equal:
                return vk::CompareOp::eEqual;
            default:
                return vk::CompareOp::eLess;
        }
    }

    vk::ColorComponentFlags VkConvert::colorWriteMask(const ColorWriteMask mask)
    {
        vk::ColorComponentFlags flags{};

        if((mask & ColorWriteMask::A) == ColorWriteMask::A)
            flags |= vk::ColorComponentFlagBits::eA;
        if((mask & ColorWriteMask::R) == ColorWriteMask::R)
            flags |= vk::ColorComponentFlagBits::eR;
        if((mask & ColorWriteMask::G) == ColorWriteMask::G)
            flags |= vk::ColorComponentFlagBits::eG;
        if((mask & ColorWriteMask::B) == ColorWriteMask::B)
            flags |= vk::ColorComponentFlagBits::eB;

        return flags;
    }

    vk::BlendFactor VkConvert::blendFactor(const BlendFactor factor)
    {
        switch (factor)
        {
            case BlendFactor::One:
                return vk::BlendFactor::eOne;
            case BlendFactor::Zero:
                return vk::BlendFactor::eZero;
            case BlendFactor::SrcAlpha:
                return vk::BlendFactor::eSrcAlpha;
            case BlendFactor::InvSrcAlpha:
                return vk::BlendFactor::eOneMinusSrcAlpha;
            default:
                return vk::BlendFactor::eOne;
        }
    }

    vk::BlendOp VkConvert::blendOp(const BlendOp op)
    {
        switch (op)
        {
            case BlendOp::Add:
                return vk::BlendOp::eAdd;
            case BlendOp::Subtract:
                return vk::BlendOp::eSubtract;
            case BlendOp::Max:
                return vk::BlendOp::eMax;
            case BlendOp::Min:
                return vk::BlendOp::eMin;
            case BlendOp::RevSubtract:
                return vk::BlendOp::eReverseSubtract;
            default:
                return vk::BlendOp::eAdd;
        }
    }

    vk::IndexType VkConvert::indexFormat(const IndexFormat format)
    {
        switch (format)
        {
            case IndexFormat::UInt32:
                return vk::IndexType::eUint32;
            case IndexFormat::UInt16:
                return vk::IndexType::eUint16;
            default:
                return vk::IndexType::eUint32;
        }
    }

    vk::Filter VkConvert::filter(const TextureFilter filter)
    {
        switch (filter)
        {
            case TextureFilter::Linear:
                return vk::Filter::eLinear;
            case TextureFilter::Nearest:
                return vk::Filter::eNearest;
            default:
                return vk::Filter::eLinear;
        }
    }

    vk::SamplerMipmapMode VkConvert::mipmapFilter(const MipmapFilter filter)
    {
        switch (filter)
        {
            case MipmapFilter::Linear:
                return vk::SamplerMipmapMode::eLinear;
            case MipmapFilter::Nearest:
                return vk::SamplerMipmapMode::eNearest;
            default:
                return vk::SamplerMipmapMode::eLinear;
        }
    }

    vk::SamplerAddressMode VkConvert::addressMode(const AddressMode mode)
    {
        switch (mode)
        {
            case AddressMode::Repeat:
                return vk::SamplerAddressMode::eRepeat;
            case AddressMode::MirroredRepeat:
                return vk::SamplerAddressMode::eMirroredRepeat;
            case AddressMode::ClampToBorder:
                return vk::SamplerAddressMode::eClampToBorder;
            case AddressMode::ClampToEdge:
                return vk::SamplerAddressMode::eClampToEdge;
            case AddressMode::MirrorClampToEdge:
                return vk::SamplerAddressMode::eMirrorClampToEdge;
            default:
                return vk::SamplerAddressMode::eRepeat;
        }
    }

    vk::BorderColor VkConvert::borderColor(const BorderColor color)
    {
        switch (color)
        {
            case BorderColor::OpaqueBlack:
                return vk::BorderColor::eFloatOpaqueBlack;
            case BorderColor::OpaqueWhite:
                return vk::BorderColor::eFloatOpaqueWhite;
            case BorderColor::TransparentBlack:
                return vk::BorderColor::eFloatTransparentBlack;
            default:
                return vk::BorderColor::eFloatOpaqueBlack;
        }
    }

    uint32_t VkConvert::pixelFormatBytes(const PixelFormat format, const VkDevice* device)
    {
        switch (format)
        {
            // 1 Byte
            case PixelFormat::R8UNorm:
            case PixelFormat::R8SNorm:
            case PixelFormat::R8UInt:
            case PixelFormat::R8SInt:
            case PixelFormat::Stencil8:
                return 1;

            // 2 Bytes
            case PixelFormat::R16UInt:
            case PixelFormat::R16SInt:
            case PixelFormat::R16Float:
            case PixelFormat::RG8UNorm:
            case PixelFormat::RG8SNorm:
            case PixelFormat::RG8UInt:
            case PixelFormat::RG8SInt:
            case PixelFormat::Depth16UNorm:
                return 2;

            // 4 Bytes (Standard 32-bit types)
            case PixelFormat::R32Float:
            case PixelFormat::R32UInt:
            case PixelFormat::R32SInt:
            case PixelFormat::RG16UInt:
            case PixelFormat::RG16SInt:
            case PixelFormat::RG16Float:
            case PixelFormat::RGBA8UNorm:
            case PixelFormat::RGBA8SNorm:
            case PixelFormat::RGBA8UInt:
            case PixelFormat::RGBA8SInt:
            case PixelFormat::RGBA8UNormSrgb:
            case PixelFormat::BGRA8UNorm:
            case PixelFormat::BGRA8UNormSrgb:
            case PixelFormat::RGB10A2UNorm:
            case PixelFormat::RGB10A2UInt:
            case PixelFormat::RG11B10UFloat:
            case PixelFormat::RGB9E5UFloat:
            case PixelFormat::Depth32Float:
                return 4;

            // 8 Bytes
            case PixelFormat::RG32Float:
            case PixelFormat::RG32UInt:
            case PixelFormat::RG32SInt:
            case PixelFormat::RGBA16UInt:
            case PixelFormat::RGBA16SInt:
            case PixelFormat::RGBA16Float:
                return 8;

            // 16 Bytes
            case PixelFormat::RGBA32Float:
            case PixelFormat::RGBA32UInt:
            case PixelFormat::RGBA32SInt:
                return 16;

            case PixelFormat::Depth24PlusStencil8:
            {
                switch (device->depth24PlusStencil8Format())
                {
                    case vk::Format::eD16Unorm:
                        return 2;

                    case vk::Format::eD24UnormS8Uint:
                    case vk::Format::eX8D24UnormPack32:
                        return 4;

                    case vk::Format::eD32Sfloat:
                        return 4;

                    case vk::Format::eD32SfloatS8Uint:
                        return 8;
                    default:
                        break;
                }
            }

            case PixelFormat::Unknown:
            default:
                return 0;
        }
    }
}
