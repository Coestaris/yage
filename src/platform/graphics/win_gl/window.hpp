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
#include <stdexcept>

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

  static LRESULT CALLBACK
  wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  window(yage::graphics::settings & settings)
  : yage::graphics::window(settings)
  {
  }
  ~window() override
  {
    close();
  }

  yage::core::error          open() override;
  bool                       is_open() override;
  yage::core::error          update() override;
  yage::core::error          close() override;
  yage::graphics::graphics * get_graphics() override;

  yage::core::error
  set_mode(yage::graphics::settings::window_mode mode) override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::error set_title(std::string title) override
  {

    throw std::runtime_error("Not implemented");
  }

  yage::core::error set_max_fps(int max_fps) override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::error set_vsync(bool vsync) override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::error get_icon(yage::graphics::texture & icon) override
  {
    throw std::runtime_error("Not implemented");
  }
  yage::core::error set_icon(yage::graphics::texture & icon) override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::error get_cursor(yage::graphics::texture & cursor) override
  {
    throw std::runtime_error("Not implemented");
  }
  yage::core::error set_cursor(yage::graphics::texture & cursor) override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::error set_geometry(yage::graphics::geometry & geometry) override
  {
    throw std::runtime_error("Not implemented");
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
