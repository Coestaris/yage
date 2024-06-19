/** ========================================================================
 *
 * @file window.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
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
  XSetWindowAttributes                   windowAttribs;
  XIC                                    xic;
  Window                                 win;
  Display *                              display;
  XVisualInfo *                          visual;
  GLXContext                             glx_context = nullptr;
  yage::platform::graphics::gl::graphics context;
  bool                                   opened = false;

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
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}
