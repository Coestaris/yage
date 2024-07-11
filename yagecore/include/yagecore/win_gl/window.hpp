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
#include <memory>
#include <stdexcept>

#include "yagecore/graphics/graphics.hpp"
#include "yagecore/gl/graphics.hpp"
#include "yagecore/graphics/window.hpp"

namespace yc::win_gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class window : public yc::graphics::window,
               public std::enable_shared_from_this<yc::win_gl::window>
{
  HDC       hDC  = NULL; /* Private GDI Device Context */
  HGLRC     hRC  = NULL; /* Permanent Rendering Context */
  HWND      hWnd = NULL; /* Holds Our Window Handle */
  HINSTANCE hInstance;   /* Holds The Instance Of The Application */
  bool      opened;

  std::shared_ptr<yc::gl::graphics> context;

  static LRESULT CALLBACK
  wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  window(const yc::graphics::settings & settings)
  : yc::graphics::window(settings)
  {
  }
  ~window() override
  {
    close();
  }

  yc::error                               open() override;
  bool                                    is_open() override;
  yc::error                               update() override;
  yc::error                               close() override;
  std::shared_ptr<yc::graphics::graphics> get_graphics() override;

  yc::error set_mode(yc::graphics::settings::window_mode mode) override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::error set_title(std::string title) override
  {

    throw std::runtime_error("Not implemented");
  }

  yc::error set_max_fps(int max_fps) override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::error set_vsync(bool vsync) override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::error get_icon(yc::graphics::texture & icon) override
  {
    throw std::runtime_error("Not implemented");
  }
  yc::error set_icon(yc::graphics::texture & icon) override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::error get_cursor(yc::graphics::texture & cursor) override
  {
    throw std::runtime_error("Not implemented");
  }
  yc::error set_cursor(yc::graphics::texture & cursor) override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::error set_geometry(yc::graphics::geometry & geometry) override
  {
    throw std::runtime_error("Not implemented");
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
