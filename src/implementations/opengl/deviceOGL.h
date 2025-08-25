#pragma once
#include "memory.h"
#include "device.h"
#include "descriptions/samplerDescription.h"

namespace NRHI
{
class WindowOGL;
class DeviceOGL final : public Device
{
public:
    explicit DeviceOGL(WindowOGL* window);

    GraphicsPipeline* createGraphicsPipeline(GraphicsPipelineDescription graphicsPipelineDescription) override;
    CommandList* createCommandList() override;

    Buffer* createIndexBuffer() override;
    Buffer* createUniformBuffer() override;
    Buffer* createVertexBuffer() override;

    Texture* createTexture(TextureDescription textureDescription) override;
    Sampler* createSampler(SamplerDescription samplerDescription) override;

    void submit(CommandList* commandList) override;
    void swapBuffers() override;

    FrameBuffer* getSwapChainFrameBuffer() override;
protected:
    Shader* createShaderFromSpirvImpl(std::unordered_map<ShaderType, std::vector<uint32_t>> shadersSpirv) override;
private:
    WindowOGL* window;
};
}
