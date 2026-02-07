#include "bufferOGL.h"

#include <glad/gl.h>

#include <clogr.h>

namespace urhi
{
    BufferOGL::BufferOGL(const GLenum target) : target(target)
    {
        clogr::ensure(target == GL_ARRAY_BUFFER ||
                     target == GL_ELEMENT_ARRAY_BUFFER ||
                     target == GL_UNIFORM_BUFFER ||
                     target == GL_SHADER_STORAGE_BUFFER ||
                     target == GL_COPY_READ_BUFFER ||
                     target == GL_COPY_WRITE_BUFFER ||
                     target == GL_PIXEL_UNPACK_BUFFER ||
                     target == GL_PIXEL_PACK_BUFFER ||
                     target == GL_TEXTURE_BUFFER ||
                     target == GL_TRANSFORM_FEEDBACK_BUFFER ||
                     target == GL_QUERY_BUFFER,
                     "Invalid buffer target {}", target);

        glGenBuffers(1, &handle);
        clogr::ensure(handle != 0, "Failed to generate OpenGL buffer object");
    }

    BufferOGL::~BufferOGL()
    {
        glDeleteBuffers(1, &handle);
    }

    void BufferOGL::bind() const
    {
        clogr::ensure(handle != 0, "Binding null buffer");
        glBindBuffer(target, handle);
    }

    void BufferOGL::unbind() const
    {
        clogr::ensure(handle != 0, "Unbinding null buffer");
        glBindBuffer(target, 0);
    }

    void BufferOGL::reserveSpace(const size_t size)
    {
        clogr::ensure(handle != 0, "Reserving space on null buffer");
        clogr::ensure(size > 0, "Attempting to reserve zero or negative buffer size: {}", size);

        this->capacity = size;
        bind();
        glBufferData(target, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
        unbind();
    }

    void BufferOGL::uploadData(const void *data, const size_t size) const
    {
        clogr::ensure(handle != 0, "Uploading data to null buffer");
        clogr::ensure(data != nullptr, "Uploading null data pointer");
        clogr::ensure(size > 0, "Uploading zero bytes");
        clogr::ensure(size <= this->capacity, "Data to be uploaded exceeds buffer capacity: {} > {}", size, this->capacity);

        bind();
        glBufferSubData(target, 0, static_cast<GLsizeiptr>(size), data);
        unbind();
    }

    void BufferOGL::bindBase(const uint32_t binding) const
    {
        clogr::ensure(handle != 0, "Binding null buffer to base {}", binding);
        clogr::ensure(glIsBuffer(handle) == GL_TRUE, "Binding invalid buffer handle {} to base {}", handle, binding);
        clogr::ensure(target == GL_UNIFORM_BUFFER || target == GL_SHADER_STORAGE_BUFFER ||
                     target == GL_TRANSFORM_FEEDBACK_BUFFER || target == GL_ATOMIC_COUNTER_BUFFER,
                     "bindBase called on unsupported target {}", target);

        glBindBufferBase(target, binding, handle);
    }

    GLenum BufferOGL::getTarget() const
    {
        clogr::ensure(handle != 0, "getTarget called on null buffer");
        return target;
    }
}
