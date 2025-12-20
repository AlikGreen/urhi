#include "pipelineOGL.h"

#include <utility>

#include "convertOGL.h"

namespace Neon::RHI
{
    PipelineOGL::PipelineOGL(const GraphicsPipelineDescription &description) : description(description)
    {
        shader = std::dynamic_pointer_cast<ShaderOGL>(description.shader);

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
        shader = std::dynamic_pointer_cast<ShaderOGL>(description.shader);
        theadGroupSize = description.threadGroupSize;
    }

    PipelineOGL::~PipelineOGL()
    {
        if (!isComputePipeline)
        {
            glDeleteVertexArrays(1, &vao);
        }
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

        if(description.depthState.enableDepthWrite)
            glDepthMask(GL_TRUE);
        else
            glDepthMask(GL_FALSE);

        if(description.rasterizerState.enableScissorTest)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);

        if (description.blendState.enableBlend)
        {
            glEnable(GL_BLEND);

            glBlendFuncSeparate(
            ConvertOGL::blendFactorToGL(description.blendState.srcColorFactor),
            ConvertOGL::blendFactorToGL(description.blendState.dstColorFactor),
            ConvertOGL::blendFactorToGL(description.blendState.srcAlphaFactor),
            ConvertOGL::blendFactorToGL(description.blendState.dstAlphaFactor)
            );

            glBlendEquationSeparate(
                ConvertOGL::blendOpToGL(description.blendState.colorOp),
                ConvertOGL::blendOpToGL(description.blendState.alphaOp)
            );
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }

    Rc<ShaderOGL> PipelineOGL::getShader() const
    {
        return shader;
    }
}
