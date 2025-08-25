#pragma once
#include <string>

#include "commandList.h"
#include "graphicsPipeline.h"
#include "shader.h"

#include "descriptions/graphicsPipelineDescription.h"

#include "buffer.h"
#include "texture.h"
#include "descriptions/samplerDescription.h"
#include "util/memory.h"
#include "descriptions/textureDescription.h"

namespace NRHI
{
class Device
{
public:
    virtual ~Device() = default;

    virtual GraphicsPipeline* createGraphicsPipeline(GraphicsPipelineDescription graphicsPipelineDescription) = 0;
    virtual CommandList* createCommandList() = 0;

    virtual Texture* createTexture(TextureDescription textureDescription) = 0;
    virtual Sampler* createSampler(SamplerDescription samplerDescription) = 0;

    Shader* createShaderFromSource(const std::string &source, const std::string &filepath = "");

    virtual Buffer* createIndexBuffer() = 0;
    virtual Buffer* createUniformBuffer() = 0;
    virtual Buffer* createVertexBuffer() = 0;

    virtual void submit(CommandList* commandList) = 0;

    virtual void swapBuffers() = 0;

    virtual FrameBuffer* getSwapChainFrameBuffer() = 0;
protected:
    virtual Shader* createShaderFromSpirvImpl(std::unordered_map<ShaderType, std::vector<uint32_t>> spirv) = 0;
};
}
