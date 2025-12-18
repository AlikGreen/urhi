#pragma once

#include <functional>
#include <vector>
#include "commandList.h"
#include "framebufferOGL.h"
#include "sampler.h"

namespace Neon::RHI
{
    class PipelineOGL;

    class CommandListOGL final : public CommandList
    {
    public:
        CommandListOGL() = default;

        void begin() override;

        void setUniformBuffer(const std::string& name, const Rc<Buffer>& buffer) override;

        void setTexture(const std::string& name, const Rc<TextureView>& texture) override;
        void setSampler(const std::string& name, const Rc<Sampler>& sampler) override;
        void generateMipmaps(const Rc<Texture>& texture) override;

        void setPipeline(const Rc<Pipeline>& pipeline) override;
        void setFramebuffer(const Rc<Framebuffer>& frameBuffer) override;

        void setVertexBuffer(uint32_t index, const Rc<Buffer>& vertexBuffer) override;
        void setIndexBuffer(const Rc<Buffer>& indexBuffer, IndexFormat indexFormat) override;

        void clearColorTarget(uint32_t target, glm::vec4 color) override;
        void clearDepthStencil(float value) override;

        void setScissor(ScissorRect rect) override;

        void updateTexture(const Rc<Texture>& texture, TextureUploadDescription uploadDescription) override;
        void reserveBuffer(const Rc<Buffer>& buffer, size_t size) override;

        void dispatch(const glm::ivec3& numGroups) override;

        void executeCommands();
    protected:
        void drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
        void drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance) override;
        void updateBufferImpl(const Rc<Buffer>& buffer, void* data, uint32_t size) override;

    private:
        std::vector<std::function<void()>> commands{};
        Rc<PipelineOGL> pipeline{};
        Rc<FramebufferOGL> framebuffer{};
        IndexFormat indexFormat = IndexFormat::UInt32;

        [[nodiscard]] const Rc<PipelineOGL>& getPipeline() const;
    };
}