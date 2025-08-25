#pragma once
#include "frameBuffer.h"
#include <glad/glad.h>

namespace NRHI
{
class FrameBufferOGL final : public FrameBuffer
{
public:
    explicit FrameBufferOGL(GLuint existing);

    void bind() const;
private:
    GLuint handle;
};
}
