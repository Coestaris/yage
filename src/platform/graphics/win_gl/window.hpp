/** ========================================================================
 *
 * @file X11Display.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <windows.h>

#include "graphics/window.hpp"
#include "platform/graphics/gl/graphics.hpp"

namespace yage::platform::graphics::win_gl
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
  yage::platform::graphics::gl::graphics context;

  static LRESULT CALLBACK wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
