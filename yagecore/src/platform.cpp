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
std::shared_ptr<yc::graphics::window>
yc::create_window(yc::graphics::settings & settings)
{
#if YAGE_PLATFORM_X11
  logging(LOG_INFO, "Creating X11 window");
  return std::make_shared<yc::x11::window>(settings);
#elif YAGE_PLATFORM_WAYLAND
  logging(LOG_INFO, "Creating wayland window");
  return std::make_shared<yc::wayland::window>(settings);
#elif YAGE_PLATFORM_WIN_GL
  logging(LOG_INFO, "Creating Windows (OpenGL) window");
  return std::make_shared<yc::win_gl::window>(settings);
#elif YAGE_PLATFORM_WIN_DX
  logging(LOG_INFO, "Creating Windows (DirectX) window");
  return std::make_shared<yc::win_dx::window>(settings);
#elif YAGE_PLATFORM_DARWIN
  logging(LOG_INFO, "Creating Darwin window");
  return std::make_shared<yc::darwin::window>(settings);
#else
  throw std::runtime_error("Unsupported platform");
#endif
}
