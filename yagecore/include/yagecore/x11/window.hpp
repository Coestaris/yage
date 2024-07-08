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

#include "yagecore/graphics/window.hpp"
#include "yagecore/gl/graphics.hpp"
#include "yagecore/util/named_thread.hpp"
#include "yagecore/error.hpp"

namespace yc::x11
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class window : public yc::graphics::window
{
private:
  std::shared_ptr<yc::gl::graphics> graphics;

  Colormap        colormap;
  XIC             xic;
  Window          win;
  Display *       display;
  XVisualInfo *   visual;
  std::thread::id working_thread;
  GLXContext      glx_context = nullptr;
  bool            opened      = false;

  class event_loop : public yc::util::named_thread
  {
  private:
    window * win;

  public:
    event_loop(window * win)
    : yc::util::named_thread("event_loop")
    , win(win)
    {
    }

    void run() override;

  } event_loop;

  Atom wmDeleteMessage;
  bool should_close = false;

public:
  window(yc::graphics::settings & settings)
  : yc::graphics::window(settings)
  , event_loop(this)
  {
  }

  ~window() override
  {
    event_loop.join();
    close();
  }

  yc::error                               open() override;
  yc::error                               update() override;
  yc::error                               close() override;
  bool                                    is_open() override;
  std::shared_ptr<yc::graphics::graphics> get_graphics() override;

  yc::error set_mode(yc::graphics::settings::window_mode mode) override;
  yc::error set_geometry(yc::graphics::geometry & geometry) override;
  yc::error set_title(std::string title) override;

  yc::error set_max_fps(int max_fps) override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::error set_vsync(bool vsync) override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::error set_icon(yc::graphics::texture & icon) override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::error get_icon(yc::graphics::texture & icon) override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::error set_cursor(yc::graphics::texture & cursor) override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::error get_cursor(yc::graphics::texture & cursor) override
  {
    throw std::runtime_error("Not implemented");
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}
