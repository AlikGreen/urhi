#pragma once
#include "device.h"
#include "event.h"
#include "glm/glm.hpp"

namespace urhi
{
class Window
{
public:
  Window() = default;
  virtual ~Window() = default;

  Window(const Window&) = delete;
  Window& operator= (const Window&) = delete;

  virtual void close() = 0;
  virtual void pollEvents(std::function<void(Event&)> callback) = 0;

  virtual uint32_t getWidth() = 0;
  virtual uint32_t getHeight() = 0;
  virtual glm::ivec2 getSize() = 0;

  virtual void setWidth(uint32_t width) = 0;
  virtual void setHeight(uint32_t height) = 0;
  virtual void setSize(glm::ivec2 size) = 0;

  virtual std::string getTitle() = 0;
  virtual void setTitle(std::string title) = 0;

  virtual void setCursorLocked(bool locked) = 0;
  virtual void setCursorVisible(bool visible) = 0;
};
}
