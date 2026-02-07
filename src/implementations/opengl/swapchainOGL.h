#pragma once
#include <vector>
#include <grl/grl.h>

#include "buffer.h"
#include "pipeline.h"
#include "sampler.h"
#include "swapchain.h"
#include "textureView.h"
#include "descriptions/swapchainDesc.h"

namespace urhi
{
class DeviceOGL;

class SwapchainOGL final : public Swapchain
{
public:
    explicit SwapchainOGL(const SwapchainDesc& desc, DeviceOGL* device);

    uint32_t acquireNextImage() override;
    void present(uint32_t imageIndex) override;
    [[nodiscard]] const std::vector<grl::Rc<Texture>>& getTextures() const override;

    void resize(uint32_t width, uint32_t height) override;
private:
    DeviceOGL* device;

    grl::Rc<Window> window;
    grl::Rc<Pipeline> pipeline;
    grl::Rc<Buffer> vertexBuffer;
    grl::Rc<Buffer> indexBuffer;

    std::vector<grl::Rc<Texture>> textures{};
    std::vector<grl::Rc<TextureView>> textureViews{};
    std::vector<grl::Rc<Sampler>> samplers{};
    uint32_t width, height;
};
}
