#pragma once
#include <vector>
#include <neonCore/neonCore.h>

#include "buffer.h"
#include "framebuffer.h"
#include "pipeline.h"
#include "sampler.h"
#include "swapchain.h"
#include "textureView.h"
#include "descriptions/swapchainDescription.h"

namespace Neon::RHI
{
class DeviceOGL;

class SwapchainOGL final : public Swapchain
{
public:
    explicit SwapchainOGL(const SwapchainDescription& desc, DeviceOGL* device);

    uint32_t acquireNextImage() override;
    void present(uint32_t imageIndex) override;
    [[nodiscard]] const std::vector<Rc<Texture>>& getTextures() const override;

    void resize(uint32_t width, uint32_t height) override;
private:
    DeviceOGL* device;

    Rc<Window> window;
    Rc<Pipeline> pipeline;
    Rc<Buffer> vertexBuffer;
    Rc<Buffer> indexBuffer;

    std::vector<Rc<Texture>> textures{};
    std::vector<Rc<TextureView>> textureViews{};
    std::vector<Rc<Sampler>> samplers{};
    uint32_t width, height;
};
}
