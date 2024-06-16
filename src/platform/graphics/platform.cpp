/** ========================================================================
 *
 * @file platform.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <stdexcept>

#include "platform.hpp"
#include "core/log.hpp"

#if YAGE_PLATFORM_X11
#include "platform/graphics/x11/window.hpp"
#include "platform/graphics/gl/shader_compiler.hpp"
#include "platform/graphics/gl/shader.hpp"
#elif YAGE_PLATFORM_WAYLAND
#include "platform/graphics/wayland/window.hpp"
#include "platform/graphics/gl/shader_compiler.hpp"
#include "platform/graphics/gl/shader.hpp"
#elif YAGE_PLATFORM_WIN_GL
#include "platform/graphics/win_gl/window.hpp"
#include "platform/graphics/gl/shader_compiler.hpp"
#include "platform/graphics/gl/shader.hpp"
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
#if YAGE_PLATFORM_X11
  log(LOG_INFO, "Destroying X11 window");
  delete (yage::platform::graphics::x11::window *)window;
#elif YAGE_PLATFORM_WAYLAND
  log(LOG_INFO, "Destroying wayland window");
  delete (yage::platform::graphics::wayland::window *)window;
#elif YAGE_PLATFORM_WIN_GL
  log(LOG_INFO, "Destroying Windows (OpenGL) window");
  delete (yage::platform::graphics::win_gl::window *)window;
#elif YAGE_PLATFORM_WIN_DX
  log(LOG_INFO, "Destroying Windows (DirectX) window");
  delete (yage::platform::graphics::win_dx::window *)window;
#elif YAGE_PLATFORM_DARWIN
  log(LOG_INFO, "Destroying Darwin window");
  delete (yage::platform::graphics::darwin::window *)window;
#else
  throw std::runtime_error("Unsupported platform");
#endif
}

yage::graphics::shader_compiler *
yage::platform::graphics::create_shader_compiler()
{
#if YAGE_PLATFORM_X11 || YAGE_PLATFORM_WAYLAND || YAGE_PLATFORM_WIN_GL
  log(LOG_INFO, "Creating OpenGL shader compiler");
  return new yage::platform::graphics::gl::shader_compiler();
#else
  throw std::runtime_error("Unsupported platform");
#endif
}

void yage::platform::graphics::destroy_shader_compiler(
      yage::graphics::shader_compiler * compiler)
{
#if YAGE_PLATFORM_X11 || YAGE_PLATFORM_WAYLAND || YAGE_PLATFORM_WIN_GL
  log(LOG_INFO, "Destroying OpenGL shader compiler");
  delete (yage::platform::graphics::gl::shader_compiler *)compiler;
#else
  throw std::runtime_error("Unsupported platform");
#endif
}

yage::graphics::shader * yage::platform::graphics::create_shader()
{
#if YAGE_PLATFORM_X11 || YAGE_PLATFORM_WAYLAND || YAGE_PLATFORM_WIN_GL
  log(LOG_INFO, "Creating OpenGL shader");
  return new yage::platform::graphics::gl::shader();
#else
  throw std::runtime_error("Unsupported platform");
#endif
}

void yage::platform::graphics::destroy_shader(yage::graphics::shader * shader)
{
#if YAGE_PLATFORM_X11 || YAGE_PLATFORM_WAYLAND || YAGE_PLATFORM_WIN_GL
  log(LOG_INFO, "Destroying OpenGL shader");
  delete (yage::platform::graphics::gl::shader *)shader;
#else
  throw std::runtime_error("Unsupported platform");
#endif
}
