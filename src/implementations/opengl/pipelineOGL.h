#pragma once
#include "shaderOGL.h"
#include "pipeline.h"
#include "descriptions/computePipelineDesc.h"
#include "descriptions/graphicsPipelineDesc.h"
#include "glm/glm.hpp"

namespace Neon::RHI
{
struct VertexAttributeOGL
{
    GLuint index;
    GLint size;
    GLenum type;
    GLsizei stride;
    const void *pointer;
};

class PipelineOGL final : public Pipeline
{
public:
    explicit PipelineOGL(const GraphicsPipelineDesc &description);
    explicit PipelineOGL(const ComputePipelineDesc &description);
    ~PipelineOGL() override;

    [[nodiscard]] std::vector<VertexAttributeOGL> getVertexAttributes() const;

    void bind() const;
    [[nodiscard]] Rc<ShaderOGL> getShader() const;

    GLuint vao{};
private:
    bool isComputePipeline = false;
    glm::ivec3 theadGroupSize{};
    std::vector<VertexAttributeOGL> vertexAttributesOGL;
    Rc<ShaderOGL> shader;
    GraphicsPipelineDesc description;
};
}
