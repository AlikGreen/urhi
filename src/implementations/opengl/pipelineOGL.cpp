#include "pipelineOGL.h"

#include <utility>

#include "convertOGL.h"

namespace Neon::RHI
{
    PipelineOGL::PipelineOGL(const GraphicsPipelineDescription &description) : description(description)
    {
        shader = dynamic_cast<ShaderOGL*>(description.shader);

        uint32_t offset = 0;
        for (const auto& attr : description.inputLayout.getVertexAttributes())
        {
            const GLenum glType = ConvertOGL::typeinfoToGL(attr.type);
            VertexAttributeOGL attrib =
            {
                attr.location,
                static_cast<int>(ConvertOGL::getComponentCount(attr.type)), glType,
                static_cast<int>(description.inputLayout.getStride()),
                reinterpret_cast<const void*>(offset)
            };

            vertexAttributesOGL.push_back(attrib);

            offset += attr.size;
        }

        glGenVertexArrays(1, &vao);
    }

    PipelineOGL::PipelineOGL(const ComputePipelineDescription &description)
    {
        isComputePipeline = true;
        shader = dynamic_cast<ShaderOGL*>(description.shader);
        theadGroupSize = description.threadGroupSize;
    }

    std::vector<VertexAttributeOGL> PipelineOGL::getVertexAttributes() const
    {
        return vertexAttributesOGL;
    }

    void PipelineOGL::bind() const
    {
        glBindVertexArray( vao);
        shader->bind();

        if(description.depthState.enableDepthTest)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        if(description.enableScissorTest)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
    }

    ShaderOGL* PipelineOGL::getShader() const
    {
        return shader;
    }
}
