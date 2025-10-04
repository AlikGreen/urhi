#pragma once

#include <functional>
#include <vector>
#include "commandList.h"

namespace Neon::RHI
{
    class PipelineOGL;

    class CommandListOGL final : public CommandList
    {
    public:
        CommandListOGL() = default;

        void begin() override;

        void setUniformBuffer(const std::string& name, Buffer* buffer) override;

        void setTexture(const std::string& name, TextureView* texture) override;
        void setSampler(const std::string& name, Sampler* sampler) override;
        void generateMipmaps(Texture* texture) override;

        void setPipeline(Pipeline* pipeline) override;
        void setFramebuffer(Framebuffer* frameBuffer) override;

        void setVertexBuffer(uint32_t index, Buffer* vertexBuffer) override;
        void setIndexBuffer(Buffer* indexBuffer, IndexFormat indexFormat) override;

        void clearColorTarget(uint32_t target, glm::vec4 color) override;
        void clearDepthStencil(float value) override;

        void updateTexture(Texture* texture, TextureUploadDescription uploadDescription) override;
        void reserveBuffer(Buffer* buffer, size_t size) override;

        void dispatch(const glm::ivec3& numGroups) override;

        void executeCommands();
    protected:
        void drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
        void drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance) override;
        void updateBufferImpl(Buffer* buffer, void* data, uint32_t size) override;

    private:
        std::vector<std::function<void()>> commands{};
        PipelineOGL* pipeline{};

        [[nodiscard]] PipelineOGL* getPipeline() const;
    };
}