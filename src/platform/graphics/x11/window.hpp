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

namespace yage::platform::graphics::x11
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class window : public yage::graphics::window
{
private:
  XSetWindowAttributes                   windowAttribs;
  Window                                 win;
  Display *                              display;
  XVisualInfo *                          visual;
  GLXContext                             glx_context = 0;
  yage::platform::graphics::gl::graphics context;

public:
  window(yage::graphics::settings & settings)
  : yage::graphics::window(settings)
  {
  }

  yage::core::error
  open(std::string &              title,
       yage::graphics::geometry & geometry,
       bool                       fullscreen);

  yage::core::error          update();
  yage::core::error          close();
  bool                       is_open();
  yage::graphics::graphics * get_graphics();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}
