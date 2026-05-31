#pragma once
#include "readbackRequest.h"

#include <glad/gl.h>


namespace urhi
{
class GlDevice;
class GlReadbackRequest final : public ReadbackRequest
{
public:
    GlReadbackRequest() = default;
    ~GlReadbackRequest() override;
    [[nodiscard]] bool isReady() const override;
    void wait() const override;

    [[nodiscard]] const void* data() const override;
    [[nodiscard]] size_t size() const override;
private:
    friend class GlCommandListEmitter;
    GlDevice* m_device{};

    mutable void* m_mapped{};
    size_t m_size{};

    GLuint m_buffer{};
    GLsync m_fence{};
};
}
