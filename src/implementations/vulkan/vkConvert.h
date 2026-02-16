#pragma once
#include <vulkan/vulkan.hpp>

#include "descriptions/clearValue.h"
#include "descriptions/shaderReflection.h"
#include "enums/bufferUsage.h"
#include "enums/colorSpace.h"
#include "enums/loadOp.h"
#include "enums/pixelFormat.h"
#include "enums/presentMode.h"
#include "enums/storeOp.h"
#include "enums/textureType.h"
#include "enums/textureUsage.h"
#include "input/keyCodes.h"

namespace urhi
{
class VkConvert
{
public:
    static KeyCode     keyCode(int glfwKey);
    static KeyMod      keyMod(int glfwMods);
    static MouseButton mouseButton(int glfwButton);

    static vk::PresentModeKHR presentMode(PresentMode presentMode);
    static vk::ColorSpaceKHR colorSpace(ColorSpace colorSpace);

    static vk::Format pixelFormat(PixelFormat pixelFormat);
    static vk::BufferUsageFlags bufferUsage(BufferUsage bufferUsage);

    static vk::DescriptorType resourceType(ShaderReflection::ResourceType type);
    static vk::ShaderStageFlags shaderStage(ShaderStage stage);

    static vk::AttachmentLoadOp loadOp(LoadOp loadOp);
    static vk::AttachmentStoreOp storeOp(StoreOp storeOp);

    static vk::ClearValue clearValue(ClearValue clearValue);

    static vk::ImageType textureType(TextureType textureType);
    static vk::ImageUsageFlags textureUsage(TextureUsage usage);

    static vk::ImageAspectFlags aspectMask(PixelFormat format);
    static vk::ImageViewType textureViewType(TextureType textureType);
};
}
