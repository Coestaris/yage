/** ========================================================================
 *
 * @file platform.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "platform.hpp"
#include "core/log.hpp"

#if YAGE_PLATFORM_X11
#include "platform/graphics/x11/window.hpp"
#elif YAGE_PLATFORM_WAYLAND
#include "platform/graphics/wayland/window.hpp"
#elif YAGE_PLATFORM_WIN_GL
#include "platform/graphics/win_gl/window.hpp"
#elif YAGE_PLATFORM_WIN_DX
#include "platform/graphics/win_dx/window.hpp"
#elif YAGE_PLATFORM_DARWIN
#include "platform/graphics/darwin/window.hpp"
#else
#error "Unsupported platform"
#endif

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::graphics::window *
yage::platform::graphics::create_window(yage::graphics::settings & settings)
{
#if YAGE_PLATFORM_X11
  log(LOG_INFO, "Creating X11 window");
  return new yage::platform::graphics::x11::window(settings);
#elif YAGE_PLATFORM_WAYLAND
  log(LOG_INFO, "Creating wayland window");
  return new yage::platform::graphics::wayland::window(settings);
#elif YAGE_PLATFORM_WIN_GL
  log(LOG_INFO, "Creating Windows (OpenGL) window");
  return new yage::platform::graphics::win_gl::window(settings);
#elif YAGE_PLATFORM_WIN_DX
  log(LOG_INFO, "Creating Windows (DirectX) window");
  return new yage::platform::graphics::win_dx::window(settings);
#elif YAGE_PLATFORM_DARWIN
  log(LOG_INFO, "Creating Darwin window");
  return new yage::platform::graphics::darwin::window(settings);
#else
  throw std::runtime_error("Unsupported platform");
#endif
}

void yage::platform::graphics::destroy_window(yage::graphics::window * window)
{
  delete window;
}
