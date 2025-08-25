#pragma once
#include "shaderOGL.h"
#include "graphicsPipeline.h"
#include "descriptions/graphicsPipelineDescription.h"

namespace NRHI
{
struct VertexAttributeOGL
{
    GLuint index;
    GLint size;
    GLenum type;
    GLsizei stride;
    const void *pointer;
};

class GraphicsPipelineOGL final : public GraphicsPipeline
{
public:
    explicit GraphicsPipelineOGL(GraphicsPipelineDescription &description);

    [[nodiscard]] std::vector<VertexAttributeOGL> getVertexAttributes() const;

    void bind() const;
    [[nodiscard]] ShaderOGL* getShader() const;

    GLuint vao{};
private:
    std::vector<VertexAttributeOGL> vertexAttributesOGL;
    ShaderOGL* shader;
    GraphicsPipelineDescription description;
};
}
