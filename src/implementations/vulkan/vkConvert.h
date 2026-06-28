#pragma once
#include <vulkan/vulkan.hpp>

#include "descriptions/clearValue.h"
#include "descriptions/shaderReflection.h"
#include "enums/addressMode.h"
#include "enums/blendFactor.h"
#include "enums/blendOp.h"
#include "enums/borderColor.h"
#include "enums/bufferUsage.h"
#include "enums/colorSpace.h"
#include "enums/colorWriteMask.h"
#include "enums/compareOp.h"
#include "enums/cullMode.h"
#include "enums/fillMode.h"
#include "enums/indexFormat.h"
#include "enums/loadOp.h"
#include "enums/mipmapFilter.h"
#include "enums/pixelFormat.h"
#include "enums/presentMode.h"
#include "enums/primitiveType.h"
#include "enums/shaderStage.h"
#include "enums/storeOp.h"
#include "enums/textureFilter.h"
#include "enums/textureType.h"
#include "enums/textureUsage.h"
#include "input/keyCodes.h"

namespace urhi
{
class VkDevice;
class VkConvert
{
public:
    static KeyCode     keyCode(int glfwKey);
    static KeyMod      keyMod(int glfwMods);
    static MouseButton mouseButton(int glfwButton);

    static vk::PresentModeKHR presentMode(PresentMode presentMode);
    static vk::ColorSpaceKHR colorSpace(ColorSpace colorSpace);

    static vk::Format pixelFormat(PixelFormat pixelFormat, const VkDevice* device);
    static PixelFormat pixelFormat(vk::Format format, const VkDevice* device);

    static vk::BufferUsageFlags bufferUsage(BufferUsage usage);

    static vk::DescriptorType descriptorType(refl::ResType type);
    static vk::ShaderStageFlags shaderStage(ShaderStage stage);
    static vk::ShaderStageFlagBits shaderStageBits(ShaderStage stage);
    static vk::Format format(refl::DataType type);

    static vk::AttachmentLoadOp loadOp(LoadOp loadOp);
    static vk::AttachmentStoreOp storeOp(StoreOp storeOp);

    static vk::ClearValue clearValue(ClearValue clearValue);

    static vk::ImageType textureType(TextureType textureType);
    static vk::ImageUsageFlags textureUsage(TextureUsage usage);

    static vk::ImageAspectFlags aspectMask(PixelFormat format);
    static vk::ImageViewType textureViewType(TextureType textureType);

    static vk::PrimitiveTopology primitiveType(PrimitiveType type);
    static vk::PolygonMode fillMode(FillMode fillMode);
    static vk::CullModeFlags cullMode(CullMode cullMode);
    static vk::CompareOp compareOp(CompareOp compareOp);

    static vk::ColorComponentFlags colorWriteMask(ColorWriteMask mask);
    static vk::BlendFactor blendFactor(BlendFactor factor);
    static vk::BlendOp blendOp(BlendOp op);

    static vk::IndexType indexFormat(IndexFormat format);

    static vk::Filter filter(TextureFilter filter);
    static vk::SamplerMipmapMode mipmapFilter(MipmapFilter filter);
    static vk::SamplerAddressMode addressMode(AddressMode mode);
    static vk::BorderColor borderColor(BorderColor color);
    static uint32_t pixelFormatBytes(PixelFormat format, const VkDevice* device);
};
}
