#pragma once

#include <functional>
#include <vector>
#include "commandList.h"

namespace NRHI
{
    class GraphicsPipelineOGL;

    class CommandListOGL final : public CommandList
    {
    public:
        CommandListOGL() = default;

        void begin() override;

        void setUniformBuffer(const std::string& name, Buffer* buffer) override;
        void setTexture(      const std::string& name, uint32_t offset, Texture* texture) override;
        void setSampler(      const std::string& name, uint32_t offset, Sampler* sampler) override;

        void setPipeline(GraphicsPipeline* pipeline) override;
        void setFrameBuffer(FrameBuffer* frameBuffer) override;

        void setVertexBuffer(uint32_t index, Buffer* vertexBuffer) override;
        void setIndexBuffer(Buffer* indexBuffer, IndexFormat indexFormat) override;

        void clearColorTarget(uint32_t target, glm::vec4 color) override;
        void clearDepthStencil(float value) override;

        void updateTexture(Texture* texture, const void* data) override;
        void reserveBuffer(Buffer* buffer, size_t size) override;

        void executeCommands();
    protected:
        void drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
        void drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance) override;
        void updateBufferImpl(Buffer* buffer, void* data, uint32_t size) override;

    private:
        std::vector<std::function<void()>> commands{};
        GraphicsPipelineOGL* pipeline{};

        [[nodiscard]] GraphicsPipelineOGL* getPipeline() const;
    };
}