#include "vkConvert.h"

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

    vk::Format VkConvert::pixelFormat(PixelFormat pixelFormat)
{
    switch (pixelFormat)
    {
        // 8-bit R
        case PixelFormat::R8Unorm:             return vk::Format::eR8Unorm;
        case PixelFormat::R8Snorm:             return vk::Format::eR8Snorm;
        case PixelFormat::R8Uint:              return vk::Format::eR8Uint;
        case PixelFormat::R8Int:               return vk::Format::eR8Sint;

        // 16-bit R
        case PixelFormat::R16Unorm:            return vk::Format::eR16Unorm;
        case PixelFormat::R16Snorm:            return vk::Format::eR16Snorm;
        case PixelFormat::R16Uint:             return vk::Format::eR16Uint;
        case PixelFormat::R16Int:              return vk::Format::eR16Sint;
        case PixelFormat::R16Float:            return vk::Format::eR16Sfloat;

        // 32-bit R
        case PixelFormat::R32Float:            return vk::Format::eR32Sfloat;
        case PixelFormat::R32Uint:             return vk::Format::eR32Uint;
        case PixelFormat::R32Int:              return vk::Format::eR32Sint;

        // 8-bit RG
        case PixelFormat::R8G8Unorm:           return vk::Format::eR8G8Unorm;
        case PixelFormat::R8G8Snorm:           return vk::Format::eR8G8Snorm;
        case PixelFormat::R8G8Uint:            return vk::Format::eR8G8Uint;
        case PixelFormat::R8G8Int:             return vk::Format::eR8G8Sint;

        // 16-bit RG
        case PixelFormat::R16G16Unorm:         return vk::Format::eR16G16Unorm;
        case PixelFormat::R16G16Snorm:         return vk::Format::eR16G16Snorm;
        case PixelFormat::R16G16Uint:          return vk::Format::eR16G16Uint;
        case PixelFormat::R16G16Int:           return vk::Format::eR16G16Sint;
        case PixelFormat::R16G16Float:         return vk::Format::eR16G16Sfloat;

        // 32-bit RG
        case PixelFormat::R32G32Float:         return vk::Format::eR32G32Sfloat;
        case PixelFormat::R32G32Uint:          return vk::Format::eR32G32Uint;
        case PixelFormat::R32G32Int:           return vk::Format::eR32G32Sint;

        // 8-bit RGB / BGR
        case PixelFormat::R8G8B8Unorm:         return vk::Format::eR8G8B8Unorm;
        case PixelFormat::R8G8B8Snorm:         return vk::Format::eR8G8B8Snorm;
        case PixelFormat::R8G8B8Uint:          return vk::Format::eR8G8B8Uint;
        case PixelFormat::R8G8B8Int:           return vk::Format::eR8G8B8Sint;
        case PixelFormat::R8G8B8UnormSrgb:     return vk::Format::eR8G8B8Srgb;
        case PixelFormat::B8G8R8Unorm:         return vk::Format::eB8G8R8Unorm;
        case PixelFormat::B8G8R8UnormSrgb:     return vk::Format::eB8G8R8Srgb;

        // 16-bit RGB
        case PixelFormat::R16G16B16Unorm:      return vk::Format::eR16G16B16Unorm;
        case PixelFormat::R16G16B16Snorm:      return vk::Format::eR16G16B16Snorm;
        case PixelFormat::R16G16B16Uint:       return vk::Format::eR16G16B16Uint;
        case PixelFormat::R16G16B16Int:        return vk::Format::eR16G16B16Sint;
        case PixelFormat::R16G16B16Float:      return vk::Format::eR16G16B16Sfloat;

        // 32-bit RGB
        case PixelFormat::R32G32B32Float:      return vk::Format::eR32G32B32Sfloat;
        case PixelFormat::R32G32B32Uint:       return vk::Format::eR32G32B32Uint;
        case PixelFormat::R32G32B32Int:        return vk::Format::eR32G32B32Sint;

        // 8-bit RGBA / BGRA
        case PixelFormat::R8G8B8A8Unorm:       return vk::Format::eR8G8B8A8Unorm;
        case PixelFormat::R8G8B8A8Snorm:       return vk::Format::eR8G8B8A8Snorm;
        case PixelFormat::R8G8B8A8Uint:        return vk::Format::eR8G8B8A8Uint;
        case PixelFormat::R8G8B8A8Int:         return vk::Format::eR8G8B8A8Sint;
        case PixelFormat::R8G8B8A8UnormSrgb:   return vk::Format::eR8G8B8A8Srgb;
        case PixelFormat::B8G8R8A8Unorm:       return vk::Format::eB8G8R8A8Unorm;
        case PixelFormat::B8G8R8A8UnormSrgb:   return vk::Format::eB8G8R8A8Srgb;

        // 16-bit RGBA
        case PixelFormat::R16G16B16A16Unorm:   return vk::Format::eR16G16B16A16Unorm;
        case PixelFormat::R16G16B16A16Snorm:   return vk::Format::eR16G16B16A16Snorm;
        case PixelFormat::R16G16B16A16Uint:    return vk::Format::eR16G16B16A16Uint;
        case PixelFormat::R16G16B16A16Int:     return vk::Format::eR16G16B16A16Sint;
        case PixelFormat::R16G16B16A16Float:   return vk::Format::eR16G16B16A16Sfloat;

        // 32-bit RGBA
        case PixelFormat::R32G32B32A32Float:   return vk::Format::eR32G32B32A32Sfloat;
        case PixelFormat::R32G32B32A32Uint:    return vk::Format::eR32G32B32A32Uint;
        case PixelFormat::R32G32B32A32Int:     return vk::Format::eR32G32B32A32Sint;

        // Packed / Special
        case PixelFormat::R10G10B10A2Unorm:    return vk::Format::eA2B10G10R10UnormPack32;
        case PixelFormat::R11G11B10Ufloat:     return vk::Format::eB10G11R11UfloatPack32;

        // Block Compressed (BC)
        case PixelFormat::BC1RgbaUnorm:        return vk::Format::eBc1RgbaUnormBlock;
        case PixelFormat::BC1RgbaUnormSrgb:    return vk::Format::eBc1RgbaSrgbBlock;
        case PixelFormat::BC2RgbaUnorm:        return vk::Format::eBc2UnormBlock;
        case PixelFormat::BC2RgbaUnormSrgb:    return vk::Format::eBc2SrgbBlock;
        case PixelFormat::BC3RgbaUnorm:        return vk::Format::eBc3UnormBlock;
        case PixelFormat::BC3RgbaUnormSrgb:    return vk::Format::eBc3SrgbBlock;
        case PixelFormat::BC4RUnorm:           return vk::Format::eBc4UnormBlock;
        case PixelFormat::BC5RgUnorm:          return vk::Format::eBc5UnormBlock;
        case PixelFormat::BC7RgbaUnorm:        return vk::Format::eBc7UnormBlock;
        case PixelFormat::BC7RgbaUnormSrgb:    return vk::Format::eBc7SrgbBlock;

        // Depth / Stencil
        case PixelFormat::D24UnormS8Uint:      return vk::Format::eD24UnormS8Uint;
        case PixelFormat::D32FloatS8Uint:      return vk::Format::eD32SfloatS8Uint;

        case PixelFormat::Invalid:
        default:                               return vk::Format::eUndefined;
    }
}

    vk::BufferUsageFlags VkConvert::bufferUsage(const BufferUsage bufferUsage)
    {
        switch (bufferUsage)
        {
            case BufferUsage::Index: return vk::BufferUsageFlagBits::eIndexBuffer;
            case BufferUsage::Vertex: return vk::BufferUsageFlagBits::eVertexBuffer;
            case BufferUsage::ShaderStorage: return vk::BufferUsageFlagBits::eStorageBuffer;
            case BufferUsage::Uniform: return vk::BufferUsageFlagBits::eUniformBuffer;
            default: return vk::BufferUsageFlagBits::eUniformBuffer;
        }
    }

    vk::DescriptorType VkConvert::resourceType(const ShaderReflection::ResourceType type)
    {
        switch (type)
        {
            case ShaderReflection::ResourceType::StorageBuffer:
                return vk::DescriptorType::eStorageBuffer;
            case ShaderReflection::ResourceType::ConstantBuffer:
                return vk::DescriptorType::eUniformBuffer;
            case ShaderReflection::ResourceType::Texture:
                return vk::DescriptorType::eSampledImage;
            case ShaderReflection::ResourceType::Sampler:
                return vk::DescriptorType::eSampler;
            case ShaderReflection::ResourceType::StorageImage:
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
            case PixelFormat::D24UnormS8Uint:
                case PixelFormat::D32FloatS8Uint:
                return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
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
}
