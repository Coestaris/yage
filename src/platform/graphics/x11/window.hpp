/** ========================================================================
 *
 * @file window.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <stdexcept>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include "graphics/window.hpp"
#include "platform/graphics/gl/graphics.hpp"
#include "core/util/named_thread.hpp"

namespace yage::platform::graphics::x11
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class window : public yage::graphics::window
{
private:
  yage::platform::graphics::gl::graphics context;

  Colormap      colormap;
  XIC           xic;
  Window        win;
  Display *     display;
  XVisualInfo * visual;
  GLXContext    glx_context = nullptr;
  bool          opened      = false;

  class event_loop : public yage::core::util::named_thread
  {
  private:
    window * win;

  public:
    event_loop(window * win)
    : yage::core::util::named_thread("event_loop")
    , win(win)
    {
    }

    void run() override;

  } event_loop;

  Atom wmDeleteMessage;
  bool should_close = false;

public:
  window(yage::graphics::settings & settings)
  : yage::graphics::window(settings)
  , event_loop(this)
  {
  }

  ~window() override
  {
    event_loop.join();
    close();
  }

  yage::core::error          open() override;
  yage::core::error          update() override;
  yage::core::error          close() override;
  bool                       is_open() override;
  yage::graphics::graphics * get_graphics() override;

  yage::core::error set_mode(yage::graphics::settings::window_mode mode) override;
  yage::core::error set_geometry(yage::graphics::geometry & geometry) override;
  yage::core::error set_title(std::string title) override;

  yage::core::error set_max_fps(int max_fps) override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::error set_vsync(bool vsync) override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::error set_icon(yage::graphics::image & icon) override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::error get_icon(yage::graphics::image & icon) override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::error set_cursor(yage::graphics::image & cursor) override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::error get_cursor(yage::graphics::image & cursor) override
  {
    throw std::runtime_error("Not implemented");
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}
