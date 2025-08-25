#pragma once
#include "commandListOGL.h"
#include "buffer.h"
#include <glad/glad.h>

namespace NRHI
{
class BufferOGL final : public Buffer
{
public:
    explicit BufferOGL(GLenum target);
    void bind() const;
    void unbind() const;
    void reserveSpace(size_t size);
    void uploadData(const void* data, size_t size) const;
    void bindBase(uint32_t binding) const;

    [[nodiscard]] GLenum getTarget() const;
private:
    GLuint handle{};
    GLenum target{};
    size_t capacity{};
};
}
