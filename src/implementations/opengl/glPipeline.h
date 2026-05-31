#pragma once

#include "pipeline.h"
#include "glShader.h"
#include <glad/gl.h>

namespace urhi
{
    class GlBindGroup;
    class GlDevice;
class GlPipeline final : public Pipeline
{
public:
    struct CombinedSamplerUnit;

    GlPipeline(GlDevice* device, const GraphicsPipelineDesc &desc);
    GlPipeline(GlDevice* device, const ComputePipelineDesc &desc);

    void createProgram(const std::vector<grl::Rc<Shader>> &shaders);
    void extractReflection(GlShader* shader);

    uint32_t vertexStride(uint32_t bindingIndex) const;

    ~GlPipeline() override;

    [[nodiscard]] GLuint shaderProgram() const { return m_shaderProgram; }
    const std::vector<CombinedSamplerUnit>& samplerUnits(const uint32_t nameHash) { return m_bindings[nameHash]; }

    PrimitiveType primitiveType() const { return m_primitiveType; }

    GLuint vao() const { return m_vao; }

    uint32_t pushConstantBinding() const { return m_pushConstantBinding; }

    int bufferBinding(const uint32_t nameHash)
    {
        const auto it = m_bufferBindings.find(nameHash);
        if(it != m_bufferBindings.end())
            return it->second;

        return -1;
    }

    void bind() const;
protected:
    GlDevice* m_device;

    GLuint m_shaderProgram{};
    GLuint m_vao{};

    PrimitiveType m_primitiveType{};

    int m_pushConstantBinding = -1;
    std::vector<uint32_t> m_vertexStrides;
    std::unordered_map<uint32_t, std::vector<CombinedSamplerUnit>> m_bindings;
    std::unordered_map<uint32_t, int> m_bufferBindings;

    struct CombinedSamplerUnit
    {
        uint32_t unit;
        int location;
    };
};
}
