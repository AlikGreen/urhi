#include "bufferOGL.h"

#include <glad/glad.h>


namespace NRHI
{
    BufferOGL::BufferOGL(const GLenum target) : target(target)
    {
        glGenBuffers(1, &handle);
    }

    void BufferOGL::bind() const
    {
        glBindBuffer(target, handle);
    }

    void BufferOGL::unbind() const
    {
        glBindBuffer(target, handle);
    }

    void BufferOGL::reserveSpace(const size_t size)
    {
        this->capacity = size;
        bind();
        glBufferData(target, static_cast<uint32_t>(size), nullptr, GL_DYNAMIC_DRAW);
        unbind();
    }

    void BufferOGL::uploadData(const void *data, const size_t size) const
    {
        // Assert::check(size <= this->capacity, "Data to be uploaded has a greater size than the capacity of the buffer.");
        bind();
        glBufferSubData(target, 0, static_cast<uint32_t>(size), data);
        unbind();
    }

    void BufferOGL::bindBase(const uint32_t binding) const
    {
        glBindBufferBase(target, binding, handle);
    }

    GLenum BufferOGL::getTarget() const
    {
        return target;
    }
}
