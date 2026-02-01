#pragma once

#include <functional>
#include <vector>
#include "commandList.h"
#include "sampler.h"

namespace Neon::RHI
{
    class DeviceOGL;
    class PipelineOGL;

    class CommandListOGL final : public CommandList
    {
    public:
        explicit CommandListOGL(DeviceOGL* device);

        void begin() override;

        void setUniformBuffer(const std::string& name, const Rc<Buffer>& buffer) override;

        void setTexture(const std::string& name, const Rc<TextureView>& texture) override;
        void setSampler(const std::string& name, const Rc<Sampler>& sampler) override;
        void setImage(const std::string &name, const Rc<TextureView> &texture, ImageAccess access) override;

        void beginRenderPass(const RenderPassDesc &desc) override;
        void endRenderPass() override;

        void generateMipmaps(const Rc<Texture>& texture) override;

        void setPipeline(const Rc<Pipeline>& pipeline) override;

        void setVertexBuffer(uint32_t index, const Rc<Buffer>& vertexBuffer) override;
        void setIndexBuffer(const Rc<Buffer>& indexBuffer, IndexFormat indexFormat) override;

        void setScissor(ScissorRect rect) override;

        void updateTexture(const Rc<Texture>& texture, const TextureUploadDesc& desc) override;
        void reserveBuffer(const Rc<Buffer>& buffer, size_t size) override;

        void dispatch(const glm::ivec3& numGroups) override;

        void resourceBarrier(const Rc<Texture> &texture, ImageAccess nextAccess) override;

        void addCustomCommand(const std::function<void()> &command);

        void executeCommands();
    protected:
        void drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
        void drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance) override;
        void updateBufferImpl(const Rc<Buffer>& buffer, void* data, uint32_t size) override;
        void readTextureImpl(const Rc<TextureView>& texture, const TextureReadDesc& desc, size_t destSize, void* dest) override;
    private:
        DeviceOGL* m_device;

        std::vector<std::function<void()>> m_commands{};

        Rc<PipelineOGL> m_pipeline{};
        IndexFormat m_indexFormat = IndexFormat::UInt32;
        RenderPassDesc m_renderPassDesc;

        [[nodiscard]] const Rc<PipelineOGL>& getPipeline() const;
    };
}