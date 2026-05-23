#pragma once
#include "event.h"
#include <vector>

namespace urhi
{
class Window
{
public:
  Window() = default;
  virtual ~Window() = default;

  Window(const Window&) = delete;
  Window& operator= (const Window&) = delete;

  virtual void show() = 0;
  virtual void hide() = 0;

  virtual void close() = 0;
  virtual std::vector<Event> pollEvents() = 0;

  virtual int32_t width() = 0;
  virtual int32_t height() = 0;

  virtual void width(int32_t width) = 0;
  virtual void height(int32_t height) = 0;

  virtual std::string title() = 0;
  virtual void title(const std::string& title) = 0;

  virtual void setCursorLocked(bool locked) = 0;
  virtual void setCursorVisible(bool visible) = 0;
};
}
