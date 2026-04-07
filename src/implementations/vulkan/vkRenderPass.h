#pragma once
#include "renderPass.h"

#include "vkCommandList.h"
#include "descriptions/renderPassDesc.h"


namespace urhi
{
class VkDevice;
class VkPipeline;
class VkTexture;
class VkRenderPass final : public RenderPass
{
public:
    VkRenderPass(std::vector<Command> &commands, const RenderPassDesc &desc);
    void setPipeline(const grl::Rc<Pipeline>& pipeline) override;

    void setBuffer(const std::string &name, const grl::Rc<Buffer> &buffer) override;
    void setTexture(const std::string &name, const grl::Rc<TextureView> &texture) override;
    void setSampler(const std::string &name, const grl::Rc<Sampler> &sampler) override;

    void pushConstants(void* data, size_t size) override;

    void setVertexBuffer(uint32_t index, const grl::Rc<Buffer> &vertexBuffer) override;
    void setIndexBuffer(const grl::Rc<Buffer> &indexBuffer, IndexFormat indexFormat) override;

    void setScissor(Rect2D rect) override;
    void setViewport(Viewport viewport) override;

    void end() override;
protected:
    void drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
    void drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance) override;
private:
    std::vector<Command>& m_commands;
};
}
