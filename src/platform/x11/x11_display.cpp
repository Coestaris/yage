/** ========================================================================
 *
 * @file X11Display.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "x11_display.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <stdexcept>

/* Defines ================================================================== */
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define FPS           30
#define TEST_LOCAL

/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::platform::x11::x11_display::x11_display()
{
}

void yage::platform::x11::x11_display::open()
{
  Window   window;
  Screen * screen;

  /* Open the display */
  Display * display = XOpenDisplay(nullptr);
  if (display == nullptr)
  {
    throw std::runtime_error("Cannot open display");
  }
}
