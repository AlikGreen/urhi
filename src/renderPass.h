#pragma once
#include <grl/mem.h>

#include "buffer.h"
#include "pipeline.h"
#include "sampler.h"
#include "textureView.h"
#include "descriptions/rect2D.h"
#include "descriptions/viewport.h"
#include "enums/indexFormat.h"
#include "enums/resourceAccess.h"

namespace urhi
{
class RenderPass
{
public:
  RenderPass() = default;
  virtual ~RenderPass() = default;

  virtual void setPipeline(const grl::Rc<Pipeline>& pipeline) = 0;

  virtual void setUniformBuffer(const std::string& name, const grl::Rc<Buffer>& buffer) = 0;
  virtual void setStorageBuffer(const std::string& name, const grl::Rc<Buffer>& buffer) = 0;

  virtual void setTexture(const std::string& name, const grl::Rc<TextureView>& texture) = 0;
  virtual void setSampler(const std::string& name, const grl::Rc<Sampler>& sampler) = 0;
  virtual void setImage(const std::string& name, const grl::Rc<TextureView>& texture, ResourceAccess access) = 0;

  virtual void setVertexBuffer(uint32_t index, const grl::Rc<Buffer>& vertexBuffer) = 0;
  virtual void setIndexBuffer(const grl::Rc<Buffer>& indexBuffer, IndexFormat indexFormat) = 0;

  virtual void setScissor(Rect2D rect) = 0;
  virtual void setViewport(Viewport viewport) = 0;

  void draw(const uint32_t vertexCount, const uint32_t instanceCount = 1, const uint32_t firstVertex = 0, const uint32_t firstInstance = 0) { drawImpl(vertexCount, instanceCount, firstVertex, firstInstance); };
  void drawIndexed(const uint32_t indexCount, const uint32_t instanceCount = 1, const uint32_t firstIndex = 0, const int vertexOffset = 0, const uint32_t firstInstance = 0) { drawIndexedImpl(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance); };

  virtual void end() = 0;
protected:
  virtual void drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
  virtual void drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance) = 0;
};
}
