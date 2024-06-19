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
#include <unordered_map>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <stdexcept>
#include <cstring>
#include <thread>

/* Defines ================================================================== */
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define FPS           30

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

  // loads the XMODIFIERS environment variable to see what IME to use
  XSetLocaleModifiers("");

  XIM xim = XOpenIM(display, 0, 0, 0);
  if (!xim)
  {
    // fallback to internal input method
    XSetLocaleModifiers("@im=none");
    xim = XOpenIM(display, 0, 0, 0);
  }

  /* Show the window */
  XClearWindow(display, win);
  XMapRaised(display, win);

  xic = XCreateIC(
        xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow,
        win, XNFocusWindow, win, NULL);

  XSetICFocus(xic);

  XSelectInput(
        display, win,
        KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask
              | PointerMotionMask);

  wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, win, &wmDeleteMessage, 1);

  event_loop.start();

  opened = true;

  return yage::core::error::OK;
}

yage::core::io::key convert_key(uint32_t keycode)
{
  static const std::unordered_map<uint32_t, yage::core::io::key> keymap = {
    /* Control keys */
    { 0x24, yage::core::io::key::ENTER },
    { 0x77, yage::core::io::key::DEL },
    { 0x16, yage::core::io::key::BACKSPACE },
    { 0x09, yage::core::io::key::ESCAPE },
    { 0x17, yage::core::io::key::TAB },
    { 0x41, yage::core::io::key::SPACE },
    { 0x4e, yage::core::io::key::SCROLL_LOCK },
    { 0x7f, yage::core::io::key::PAUSE },
    // { XK_Sys_Req, yage::core::io::key::SYS_REQ },
    { 0x6e, yage::core::io::key::HOME },
    { 0x71, yage::core::io::key::LEFT },
    { 0x6f, yage::core::io::key::UP },
    { 0x72, yage::core::io::key::RIGHT },
    { 0x74, yage::core::io::key::DOWN },
    // { XK_Prior, yage::core::io::key::PRIOR },
    { 0x70, yage::core::io::key::PAGE_UP },
    // { XK_Next, yage::core::io::key::NEXT },
    { 0x75, yage::core::io::key::PAGE_DOWN },
    { 0x73, yage::core::io::key::END },
    // { XK_Begin, yage::core::io::key::BEGIN },

    /* Special keys */
    { 0x2f, yage::core::io::key::SK1 },
    { 0x3d, yage::core::io::key::SK2 },
    { 0x33, yage::core::io::key::SK3 },
    { 0x22, yage::core::io::key::SK4 },
    { 0x23, yage::core::io::key::SK5 },
    { 0x30, yage::core::io::key::SK6 },
    { 0x31, yage::core::io::key::SK7 },
    { 0x3b, yage::core::io::key::SK8 },
    { 0x3c, yage::core::io::key::SK9 },
    { 0x14, yage::core::io::key::SK10 },
    { 0x15, yage::core::io::key::SK11 },

    /* Letters */
    { 0x26, yage::core::io::key::A },
    { 0x38, yage::core::io::key::B },
    { 0x36, yage::core::io::key::C },
    { 0x28, yage::core::io::key::D },
    { 0x1a, yage::core::io::key::E },
    { 0x29, yage::core::io::key::F },
    { 0x2a, yage::core::io::key::G },
    { 0x2b, yage::core::io::key::H },
    { 0x1f, yage::core::io::key::I },
    { 0x2c, yage::core::io::key::J },
    { 0x2d, yage::core::io::key::K },
    { 0x2e, yage::core::io::key::L },
    { 0x3a, yage::core::io::key::M },
    { 0x39, yage::core::io::key::N },
    { 0x20, yage::core::io::key::O },
    { 0x21, yage::core::io::key::P },
    { 0x18, yage::core::io::key::Q },
    { 0x1b, yage::core::io::key::R },
    { 0x27, yage::core::io::key::S },
    { 0x1c, yage::core::io::key::T },
    { 0x1e, yage::core::io::key::U },
    { 0x37, yage::core::io::key::V },
    { 0x19, yage::core::io::key::W },
    { 0x35, yage::core::io::key::X },
    { 0x1d, yage::core::io::key::Y },
    { 0x34, yage::core::io::key::Z },

    /* Numbers */
    { 0x13, yage::core::io::key::K0 },
    { 0xa, yage::core::io::key::K1 },
    { 0xb, yage::core::io::key::K2 },
    { 0xc, yage::core::io::key::K3 },
    { 0xd, yage::core::io::key::K4 },
    { 0xe, yage::core::io::key::K5 },
    { 0xf, yage::core::io::key::K6 },
    { 0x10, yage::core::io::key::K7 },
    { 0x11, yage::core::io::key::K8 },
    { 0x12, yage::core::io::key::K9 },

    /* Misc functions */
    //    { XK_space, yage::core::io::key::SELECT },
    //    { XK_space, yage::core::io::key::PRINT },
    //    { XK_space, yage::core::io::key::EXECUTE },
    { 0x76, yage::core::io::key::INSERT },
    //    { XK_space, yage::core::io::key::UNDO },
    //    { XK_space, yage::core::io::key::REDO },
    //    { XK_space, yage::core::io::key::MENU },
    //    { XK_space, yage::core::io::key::FIND },
    //    { XK_space, yage::core::io::key::CANCEL },
    //    { XK_space, yage::core::io::key::HELP },
    //    { XK_space, yage::core::io::key::BREAK },
    //    { XK_space, yage::core::io::key::CLEAR },
    { 0x4d, yage::core::io::key::NUM_LOCK },

    /* Keypad functions */
    //    { XK_space, yage::core::io::key::KP_SPACE },
    //    { XK_space, yage::core::io::key::KP_TAB },
    { 0x68, yage::core::io::key::KP_ENTER },
    //    { XK_space, yage::core::io::key::KP_HOME },
    //    { XK_space, yage::core::io::key::KP_LEFT },
    //    { XK_space, yage::core::io::key::KP_UP },
    //    { XK_space, yage::core::io::key::KP_RIGHT },
    //    { XK_space, yage::core::io::key::KP_DOWN },
    //    { XK_space, yage::core::io::key::KP_PRIOR },
    //    { XK_space, yage::core::io::key::KP_PAGE_UP },
    //    { XK_space, yage::core::io::key::KP_NEXT },
    //    { XK_space, yage::core::io::key::KP_PAGE_DOWN },
    //    { XK_space, yage::core::io::key::KP_END },
    //    { XK_space, yage::core::io::key::KP_BEGIN },
    //    { XK_space, yage::core::io::key::KP_INSERT },
    //    { XK_space, yage::core::io::key::KP_EQUAL },
    { 0x3f, yage::core::io::key::KP_MULTIPLY },
    { 0x56, yage::core::io::key::KP_ADD },
    //    { XK_space, yage::core::io::key::KP_SEPARATOR },
    { 0x52, yage::core::io::key::KP_SUBTRACT },
    { 0x5b, yage::core::io::key::KP_DECIMAL },
    { 0x6a, yage::core::io::key::KP_DIVIDE },
    { 0x5a, yage::core::io::key::KP_0 },
    { 0x57, yage::core::io::key::KP_1 },
    { 0x58, yage::core::io::key::KP_2 },
    { 0x59, yage::core::io::key::KP_3 },
    { 0x53, yage::core::io::key::KP_4 },
    { 0x54, yage::core::io::key::KP_5 },
    { 0x55, yage::core::io::key::KP_6 },
    { 0x4f, yage::core::io::key::KP_7 },
    { 0x50, yage::core::io::key::KP_8 },
    { 0x51, yage::core::io::key::KP_9 },

    /* Function keys */
    { 0x43, yage::core::io::key::F1 },
    { 0x44, yage::core::io::key::F2 },
    { 0x45, yage::core::io::key::F3 },
    { 0x46, yage::core::io::key::F4 },
    { 0x47, yage::core::io::key::F5 },
    { 0x48, yage::core::io::key::F6 },
    { 0x49, yage::core::io::key::F7 },
    { 0x4a, yage::core::io::key::F8 },
    { 0x4b, yage::core::io::key::F9 },
    { 0x4c, yage::core::io::key::F10 },
    { 0x5f, yage::core::io::key::F11 },
    { 0x60, yage::core::io::key::F12 },

    /* Key state modifier keys */
    { 0x32, yage::core::io::key::SHIFT_L },
    { 0x3e, yage::core::io::key::SHIFT_R },
    { 0x25, yage::core::io::key::CONTROL_L },
    { 0x69, yage::core::io::key::CONTROL_R },
    { 0x42, yage::core::io::key::CAPS_LOCK },
    //    { XK_space, yage::core::io::key::SHIFT_LOCK },
    { 0x85, yage::core::io::key::META_L },
    { 0x86, yage::core::io::key::META_R },
    { 0x40, yage::core::io::key::ALT_L },
    { 0x6c, yage::core::io::key::ALT_R },
    //    { XK_space, yage::core::io::key::SUPER_L },
    //    { XK_space, yage::core::io::key::SUPER_R },
    //    { XK_space, yage::core::io::key::HYPER_L },
    //    { XK_space, yage::core::io::key::HYPER_R },
  };

#if USE_DEBUG
  if (keymap.find(keycode) == keymap.end())
  {
    log(LOG_WARN, "Key not found: 0x%x (%c)", keycode, (char)keycode);
    return yage::core::io::key::UNKNOWN;
  }
#endif

  /* Fall if key is not found */
  return keymap.at(keycode);
}

yage::core::io::mouse convert_button(unsigned int button)
{
  switch (button)
  {
    case Button1:
      return yage::core::io::mouse::LEFT;
    case Button2:
      return yage::core::io::mouse::MIDDLE;
    case Button3:
      return yage::core::io::mouse::RIGHT;
    case Button4:
      return yage::core::io::mouse::X1;
    case Button5:
      return yage::core::io::mouse::X2;
    default:
      return yage::core::io::mouse::UNKNOWN;
  }
}

yage::core::error yage::platform::graphics::x11::window::update()
{
  if (should_close)
  {
    close();
    return yage::core::error::OK;
  }
  if (!opened)
  {
    return yage::core::error::OK;
  }

  glXSwapBuffers(display, win);

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::x11::window::close()
{
  if (!opened)
  {
    return yage::core::error::OK;
  }

  if (!should_close)
  {
    should_close = true;

    /* send event to unblock event loop */
    XEvent ev;
    memset(&ev, 0, sizeof(ev));
    ev.type                 = ClientMessage;
    ev.xclient.window       = win;
    ev.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", True);
    ev.xclient.format       = 32;
    ev.xclient.data.l[0]    = wmDeleteMessage;
    ev.xclient.data.l[1]    = CurrentTime;
    XSendEvent(display, win, False, NoEventMask, &ev);
    XFlush(display);

    event_loop.join();
  }

  log(LOG_DEBUG, "Destroying X11 window");
  glXDestroyContext(display, glx_context);

  log(LOG_DEBUG, "Closing X11 display");
  XFree(visual);
  XFreeColormap(display, windowAttribs.colormap);
  XDestroyWindow(display, win);
  XCloseDisplay(display);

  opened = false;

  return yage::core::error::OK;
}

bool yage::platform::graphics::x11::window::is_open()
{
  return opened;
}

yage::graphics::graphics * yage::platform::graphics::x11::window::get_graphics()
{
  return &context;
}

void yage::platform::graphics::x11::window::event_loop::run()
{
  while (1)
  {
    XEvent ev;
    XNextEvent(win->display, &ev);
    if (win->should_close)
    {
      return;
    }

    switch (ev.type)
    {
      case Expose:
        break;

      case ClientMessage:
        /* Handle close event */
        if ((long)ev.xclient.data.l[0] == (long)win->wmDeleteMessage)
        {
          win->should_close = true;
          return;
        }
        break;

      case KeyPress:
      {
        yage::core::io::key key = convert_key(ev.xkey.keycode);
        if (key != yage::core::io::key::UNKNOWN)
        {
          win->input.key_pressed.push(key);
        }

        /* Check is there any character input */
        char   buf[32];
        KeySym sym;
        Status status;
        Xutf8LookupString(win->xic, &ev.xkey, buf, sizeof(buf), &sym, &status);
        if (status == XLookupChars || status == XLookupBoth)
        {
          char * p = buf;
          size_t ret;

          for (wchar_t wc;
               (ret = mbrtowc(&wc, p, buf + sizeof(buf) - p, nullptr)) > 0;
               p += ret)
          {
            win->input.char_entered.push(wc);
          }
        }

        break;
      }

      case KeyRelease:
      {
        yage::core::io::key key = convert_key(ev.xkey.keycode);
        if (key != yage::core::io::key::UNKNOWN)
        {
          win->input.key_released.push(key);
        }

        break;
      }

      case ButtonPress:
      {
        struct yage::graphics::mouse_click_data click_data
              = { convert_button(ev.xbutton.button), (float)ev.xbutton.x,
                  (float)ev.xbutton.y };
        win->input.mouse_pressed.push(click_data);
        break;
      }

      case ButtonRelease:
      {
        struct yage::graphics::mouse_click_data click_data
              = { convert_button(ev.xbutton.button), (float)ev.xbutton.x,
                  (float)ev.xbutton.y };
        win->input.mouse_released.push(click_data);
        break;
      }

      case MotionNotify:
      {
        struct yage::graphics::mouse_moved_data pos
              = { (float)ev.xmotion.x, (float)ev.xmotion.y };
        win->input.mouse_moved.push(pos);
        break;
      }

      default:
        break;
    }
  }
}
