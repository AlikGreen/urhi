#pragma once
#include "renderPass.h"
#include <vulkan/vulkan.hpp>

#include "descriptions/renderPassDesc.h"
#include "descriptions/shaderReflection.h"


namespace urhi
{
class VkDevice;
class VkPipeline;
class VkTexture;
class VkRenderPass final : public RenderPass
{
public:
    VkRenderPass(VkDevice* device, vk::CommandBuffer commandBuffer, const RenderPassDesc &desc);
    void setPipeline(const grl::Rc<Pipeline>& pipeline) override;

    void setUniformBuffer(const std::string& name, const grl::Rc<Buffer> &buffer) override;
    void setStorageBuffer(const std::string& name, const grl::Rc<Buffer> &buffer) override;

    void setTexture(const std::string& name, const grl::Rc<TextureView> &texture) override;
    void setSampler(const std::string& name, const grl::Rc<Sampler> &sampler) override;
    void setImage(const std::string& name, const grl::Rc<TextureView> &texture, ResourceAccess access) override;

    void setVertexBuffer(uint32_t index, const grl::Rc<Buffer> &vertexBuffer) override;
    void setIndexBuffer(const grl::Rc<Buffer> &indexBuffer, IndexFormat indexFormat) override;

    void setScissor(Rect2D rect) override;
    void setViewport(Viewport viewport) override;

    void end() override;
protected:
    void drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
    void drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance) override;
private:
    void pushDescriptors();

    VkDevice* m_device;
    VkPipeline* m_currentPipeline = nullptr;
    vk::CommandBuffer m_cmd;

    struct BoundResource
    {
        ShaderReflection::ResourceType type;
        vk::DescriptorBufferInfo bufferInfo;
        vk::DescriptorImageInfo imageInfo;
    };

    std::unordered_map<std::string, BoundResource> m_boundResources;
};
}
