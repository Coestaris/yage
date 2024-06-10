/** ========================================================================
 *
 * @file window.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "platform/graphics/x11/window.hpp"
#include "core/log.hpp"
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <stdexcept>
#include <cstring>

/* Defines ================================================================== */
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define FPS           30
#define TEST_LOCAL

/* Macros =================================================================== */
/* Types ==================================================================== */
typedef GLXContext (*glXCreateContextAttribsARBProc)(
      Display *, GLXFBConfig, GLXContext, Bool, const int *);

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error yage::platform::graphics::x11::window::open(
      std::string & title, yage::graphics::geometry & geometry, bool fullscreen)
{
  int screenId;

  log(LOG_DEBUG, "Opening X11 window");
  display = XOpenDisplay(NULL);
  if (display == NULL)
  {
    log(LOG_ERROR, "Cannot open display");
    return yage::core::error::EXTERNAL_API_ERROR;
  }
  screenId = DefaultScreen(display);

  /* Check GLX version */
  GLint majorGLX, minorGLX = 0;
  glXQueryVersion(display, &majorGLX, &minorGLX);
  log(LOG_DEBUG, "GLX version: %d.%d", majorGLX, minorGLX);
  if (majorGLX <= 1 && minorGLX < 2)
  {
    log(LOG_ERROR, "GLX 1.2 or greater is required");
    XCloseDisplay(display);
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  // clang-format off
  GLint glxAttribs[] = {
    GLX_X_RENDERABLE    , True,
    GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
    GLX_RENDER_TYPE     , GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
    GLX_RED_SIZE        , 8,
    GLX_GREEN_SIZE      , 8,
    GLX_BLUE_SIZE       , 8,
    GLX_ALPHA_SIZE      , 8,
    GLX_DEPTH_SIZE      , 24,
    GLX_STENCIL_SIZE    , 8,
    GLX_DOUBLEBUFFER    , True,
    None,
  };
  // clang-format on

  log(LOG_DEBUG, "Getting framebuffer config for screen %d", screenId);
  int           fbcount;
  GLXFBConfig * fbc
        = glXChooseFBConfig(display, screenId, glxAttribs, &fbcount);
  if (fbc == 0)
  {
    log(LOG_ERROR, "Failed to retrieve a framebuffer config");
    XCloseDisplay(display);
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  /* Pick the FB config/visual with the most samples per pixel */
  int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
  for (int i = 0; i < fbcount; ++i)
  {
    XVisualInfo * vi = glXGetVisualFromFBConfig(display, fbc[i]);
    if (vi != 0)
    {
      int samp_buf, samples;
      glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
      glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

      log(LOG_DEBUG,
          "Matching fbconfig %d, visual ID 0x%2lx: SAMPLE_BUFFERS = %d,"
          " SAMPLES = %d",
          i, vi->visualid, samp_buf, samples);

      if (best_fbc < 0 || (samp_buf && samples > best_num_samp))
      {
        best_fbc      = i;
        best_num_samp = samples;
      }
      if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
        worst_fbc = i;
      worst_num_samp = samples;
    }
    XFree(vi);
  }
  GLXFBConfig bestFbc = fbc[best_fbc];
  XFree(fbc); /* Make sure to free this! */

  visual = glXGetVisualFromFBConfig(display, bestFbc);
  if (visual == 0)
  {
    log(LOG_ERROR, "Could not create correct visual window");
    XCloseDisplay(display);
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  if (screenId != visual->screen)
  {
    log(LOG_ERROR, "screenId != visual->screen");
    XCloseDisplay(display);
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  /* Open the window */
  windowAttribs.border_pixel      = BlackPixel(display, screenId);
  windowAttribs.background_pixel  = WhitePixel(display, screenId);
  windowAttribs.override_redirect = True;
  windowAttribs.colormap          = XCreateColormap(
        display, RootWindow(display, screenId), visual->visual, AllocNone);
  windowAttribs.event_mask = ExposureMask;
  win                      = XCreateWindow(
        display, RootWindow(display, screenId), 0, 0, WINDOW_WIDTH,
        WINDOW_HEIGHT, 0, visual->depth, InputOutput, visual->visual,
        CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &windowAttribs);

  /* Redirect Close */
  Atom atomWmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, win, &atomWmDeleteWindow, 1);

  log(LOG_DEBUG, "Creating GLX context");
  glx_context
        = glXCreateNewContext(display, bestFbc, GLX_RGBA_TYPE, nullptr, True);
  if (!glx_context)
  {
    log(LOG_ERROR, "Failed to create GLX context");
    XCloseDisplay(display);
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  XSync(display, False);

  /* Verifying that context is a direct context */
  if (!glXIsDirect(display, glx_context))
  {
    log(LOG_INFO, "Indirect GLX rendering context obtained");
  }
  else
  {
    log(LOG_INFO, "Direct GLX rendering context obtained");
  }

  log(LOG_DEBUG, "Making GLX context current");
  glXMakeCurrent(display, win, glx_context);

  if (context.init(16, WINDOW_WIDTH, WINDOW_HEIGHT) != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to initialize graphics context");
    XCloseDisplay(display);
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  /* Show the window */
  XClearWindow(display, win);
  XMapRaised(display, win);

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::x11::window::update()
{
  XEvent ev;
  while (XPending(display))
  {
    XNextEvent(display, &ev);
  }

  glXSwapBuffers(display, win);

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::x11::window::close()
{
  /* Cleanup GLX */
  glXDestroyContext(display, glx_context);

  /* Cleanup X11 */
//  XFree(visual);
//  XFreeColormap(display, windowAttribs.colormap);
  XDestroyWindow(display, win);
  XCloseDisplay(display);

  return yage::core::error::OK;
}

bool yage::platform::graphics::x11::window::is_open()
{
  return true;
}

yage::graphics::graphics * yage::platform::graphics::x11::window::get_graphics()
{
  return &context;
}
