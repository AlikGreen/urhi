#pragma once
#include "shaderOGL.h"
#include "pipeline.h"
#include "descriptions/computePipelineDescription.h"
#include "descriptions/graphicsPipelineDescription.h"
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
    explicit PipelineOGL(const GraphicsPipelineDescription &description);
    explicit PipelineOGL(const ComputePipelineDescription &description);

    [[nodiscard]] std::vector<VertexAttributeOGL> getVertexAttributes() const;

    void bind() const;
    [[nodiscard]] ShaderOGL* getShader() const;

    GLuint vao{};
private:
    bool isComputePipeline = false;
    glm::ivec3 theadGroupSize{};
    std::vector<VertexAttributeOGL> vertexAttributesOGL;
    ShaderOGL* shader;
    GraphicsPipelineDescription description;
};
}
