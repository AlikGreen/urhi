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
    struct ComputeResourceInfo;

    static constexpr int OPTIMIZED_OUT = -1;
    static constexpr int INVALID_TYPO = -2;

    GlPipeline(GlDevice* device, const GraphicsPipelineDesc &desc);
    GlPipeline(GlDevice* device, const ComputePipelineDesc &desc);

    void createProgram(const std::vector<grl::Rc<Shader>> &shaders);
    void extractReflection(GlShader* shader);

    [[nodiscard]] uint32_t vertexStride(uint32_t bindingIndex) const;

    ~GlPipeline() override;

    [[nodiscard]] GLuint shaderProgram() const { return m_shaderProgram; }

    [[nodiscard]] const std::vector<CombinedSamplerUnit>* textureBinding(uint32_t nameHash) const;
    [[nodiscard]] const std::vector<CombinedSamplerUnit>* samplerBinding(uint32_t nameHash) const;

    [[nodiscard]] PrimitiveType primitiveType() const { return m_primitiveType; }

    [[nodiscard]] GLuint vao() const { return m_vao; }

    [[nodiscard]] uint32_t pushConstantBinding() const { return m_pushConstantBinding; }

    ComputeResourceInfo bufferBinding(uint32_t nameHash);
    ComputeResourceInfo imageBinding(uint32_t nameHash);

    [[nodiscard]] const std::unordered_map<uint32_t, ComputeResourceInfo>& bufferBindings() const { return m_bufferBindings; }

    void bind() const;
protected:
    void resolveBlockBindings();

    GlDevice* m_device;

    // General
    GLuint m_shaderProgram{};

    PrimitiveType m_primitiveType{};

    int m_pushConstantBinding = -1;
    std::string m_pushConstantInstanceName{};
    std::unordered_map<uint32_t, std::vector<CombinedSamplerUnit>> m_textureBindings;
    std::unordered_map<uint32_t, std::vector<CombinedSamplerUnit>> m_samplerBindings;
    std::unordered_map<uint32_t, ComputeResourceInfo> m_imageBindings;
    std::unordered_map<uint32_t, ComputeResourceInfo> m_bufferBindings;

    // Graphics pipeline
    std::vector<uint32_t> m_vertexStrides;
    GLuint m_vao{};

    std::vector<ColorAttachmentDesc> m_colorAttachments{};

    bool m_enableDepthTest;
    bool m_enableDepthWrite;
    GLenum m_depthFunc;

    GLenum m_cullFaceMode;

    bool m_enableScissorTest;

    GLenum m_polygonMode;

    bool m_enableStencilTest = false;


    struct CombinedSamplerUnit
    {
        uint32_t unit;
        int location;
    };

    struct ComputeResourceInfo
    {
        int binding;
        ResourceAccess access;
        GLenum target;
    };
};
}
