/** ========================================================================
 *
 * @file platform.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/log.hpp"
#include "yagecore/platform.hpp"

#if YAGE_PLATFORM_X11
#include "yagecore/x11/window.hpp"
#elif YAGE_PLATFORM_WAYLAND
#include "yagecore/wayland/window.hpp"
#elif YAGE_PLATFORM_WIN_GL
#include "yagecore/win_gl/window.hpp"
#elif YAGE_PLATFORM_WIN_DX
#include "yagecore/win_dx/window.hpp"
#elif YAGE_PLATFORM_DARWIN
#include "yagecore/darwin/window.hpp"
#else
#error "Unsupported platform"
#endif

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::graphics::window * yc::create_window(yc::graphics::settings & settings)
{
#if YAGE_PLATFORM_X11
  logging(LOG_INFO, "Creating X11 window");
  return new yc::x11::window(settings);
#elif YAGE_PLATFORM_WAYLAND
  logging(LOG_INFO, "Creating wayland window");
  return new yc::wayland::window(settings);
#elif YAGE_PLATFORM_WIN_GL
  logging(LOG_INFO, "Creating Windows (OpenGL) window");
  return new yc::win_gl::window(settings);
#elif YAGE_PLATFORM_WIN_DX
  logging(LOG_INFO, "Creating Windows (DirectX) window");
  return new yc::win_dx::window(settings);
#elif YAGE_PLATFORM_DARWIN
  logging(LOG_INFO, "Creating Darwin window");
  return new yc::darwin::window(settings);
#else
  throw std::runtime_error("Unsupported platform");
#endif
}

void yc::destroy_window(yc::graphics::window * window)
{
  delete window;
}
