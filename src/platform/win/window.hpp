/** ========================================================================
 *
 * @file X11Display.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics/window.hpp"
#include "platform/gl/graphics.hpp"
#include <windows.h>

namespace yage::platform::win
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class window : public yage::graphics::window
{
  HDC       hDC  = NULL; /* Private GDI Device Context */
  HGLRC     hRC  = NULL; /* Permanent Rendering Context */
  HWND      hWnd = NULL; /* Holds Our Window Handle */
  HINSTANCE hInstance;   /* Holds The Instance Of The Application */
  bool      opened;
  yage::platform::gl::graphics context;

public:
  window(yage::graphics::settings & settings)
  : yage::graphics::window(settings)
  {
  }

  yage::core::error
  open(std::string &              title,
       yage::graphics::geometry & geometry,
       bool                       fullscreen) override;

  bool                       is_open() override;
  yage::core::error          update() override;
  yage::core::error          close() override;
  yage::graphics::graphics * get_graphics() override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
