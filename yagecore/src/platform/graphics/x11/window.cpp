/** ========================================================================
 *
 * @file window.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <unordered_map>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <GL/gl.h>
#include <cstring>
#include <thread>

#include "yagecore/x11/window.hpp"
#include "yagecore/log.hpp"
#include "yagecore/keyboard.hpp"
#include "yagecore/mouse.hpp"
#include "yagecore/error.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
static yc::key convert_key(uint32_t keycode)
{
  static const std::unordered_map<uint32_t, yc::key> keymap = {
    /* Control keys */
    { 0x24, yc::key::ENTER },
    { 0x77, yc::key::DEL },
    { 0x16, yc::key::BACKSPACE },
    { 0x09, yc::key::ESCAPE },
    { 0x17, yc::key::TAB },
    { 0x41, yc::key::SPACE },
    { 0x4e, yc::key::SCROLL_LOCK },
    { 0x7f, yc::key::PAUSE },
    // { XK_Sys_Req, yc::key::SYS_REQ },
    { 0x6e, yc::key::HOME },
    { 0x71, yc::key::LEFT },
    { 0x6f, yc::key::UP },
    { 0x72, yc::key::RIGHT },
    { 0x74, yc::key::DOWN },
    // { XK_Prior, yc::key::PRIOR },
    { 0x70, yc::key::PAGE_UP },
    // { XK_Next, yc::key::NEXT },
    { 0x75, yc::key::PAGE_DOWN },
    { 0x73, yc::key::END },
    // { XK_Begin, yc::key::BEGIN },

    /* Special keys */
    { 0x2f, yc::key::SK1 },
    { 0x3d, yc::key::SK2 },
    { 0x33, yc::key::SK3 },
    { 0x22, yc::key::SK4 },
    { 0x23, yc::key::SK5 },
    { 0x30, yc::key::SK6 },
    { 0x31, yc::key::SK7 },
    { 0x3b, yc::key::SK8 },
    { 0x3c, yc::key::SK9 },
    { 0x14, yc::key::SK10 },
    { 0x15, yc::key::SK11 },

    /* Letters */
    { 0x26, yc::key::A },
    { 0x38, yc::key::B },
    { 0x36, yc::key::C },
    { 0x28, yc::key::D },
    { 0x1a, yc::key::E },
    { 0x29, yc::key::F },
    { 0x2a, yc::key::G },
    { 0x2b, yc::key::H },
    { 0x1f, yc::key::I },
    { 0x2c, yc::key::J },
    { 0x2d, yc::key::K },
    { 0x2e, yc::key::L },
    { 0x3a, yc::key::M },
    { 0x39, yc::key::N },
    { 0x20, yc::key::O },
    { 0x21, yc::key::P },
    { 0x18, yc::key::Q },
    { 0x1b, yc::key::R },
    { 0x27, yc::key::S },
    { 0x1c, yc::key::T },
    { 0x1e, yc::key::U },
    { 0x37, yc::key::V },
    { 0x19, yc::key::W },
    { 0x35, yc::key::X },
    { 0x1d, yc::key::Y },
    { 0x34, yc::key::Z },

    /* Numbers */
    { 0x13, yc::key::K0 },
    { 0xa, yc::key::K1 },
    { 0xb, yc::key::K2 },
    { 0xc, yc::key::K3 },
    { 0xd, yc::key::K4 },
    { 0xe, yc::key::K5 },
    { 0xf, yc::key::K6 },
    { 0x10, yc::key::K7 },
    { 0x11, yc::key::K8 },
    { 0x12, yc::key::K9 },

    /* Misc functions */
    //    { XK_space, yc::key::SELECT },
    //    { XK_space, yc::key::PRINT },
    //    { XK_space, yc::key::EXECUTE },
    { 0x76, yc::key::INSERT },
    //    { XK_space, yc::key::UNDO },
    //    { XK_space, yc::key::REDO },
    //    { XK_space, yc::key::MENU },
    //    { XK_space, yc::key::FIND },
    //    { XK_space, yc::key::CANCEL },
    //    { XK_space, yc::key::HELP },
    //    { XK_space, yc::key::BREAK },
    //    { XK_space, yc::key::CLEAR },
    { 0x4d, yc::key::NUM_LOCK },

    /* Keypad functions */
    //    { XK_space, yc::key::KP_SPACE },
    //    { XK_space, yc::key::KP_TAB },
    { 0x68, yc::key::KP_ENTER },
    //    { XK_space, yc::key::KP_HOME },
    //    { XK_space, yc::key::KP_LEFT },
    //    { XK_space, yc::key::KP_UP },
    //    { XK_space, yc::key::KP_RIGHT },
    //    { XK_space, yc::key::KP_DOWN },
    //    { XK_space, yc::key::KP_PRIOR },
    //    { XK_space, yc::key::KP_PAGE_UP },
    //    { XK_space, yc::key::KP_NEXT },
    //    { XK_space, yc::key::KP_PAGE_DOWN },
    //    { XK_space, yc::key::KP_END },
    //    { XK_space, yc::key::KP_BEGIN },
    //    { XK_space, yc::key::KP_INSERT },
    //    { XK_space, yc::key::KP_EQUAL },
    { 0x3f, yc::key::KP_MULTIPLY },
    { 0x56, yc::key::KP_ADD },
    //    { XK_space, yc::key::KP_SEPARATOR },
    { 0x52, yc::key::KP_SUBTRACT },
    { 0x5b, yc::key::KP_DECIMAL },
    { 0x6a, yc::key::KP_DIVIDE },
    { 0x5a, yc::key::KP_0 },
    { 0x57, yc::key::KP_1 },
    { 0x58, yc::key::KP_2 },
    { 0x59, yc::key::KP_3 },
    { 0x53, yc::key::KP_4 },
    { 0x54, yc::key::KP_5 },
    { 0x55, yc::key::KP_6 },
    { 0x4f, yc::key::KP_7 },
    { 0x50, yc::key::KP_8 },
    { 0x51, yc::key::KP_9 },

    /* Function keys */
    { 0x43, yc::key::F1 },
    { 0x44, yc::key::F2 },
    { 0x45, yc::key::F3 },
    { 0x46, yc::key::F4 },
    { 0x47, yc::key::F5 },
    { 0x48, yc::key::F6 },
    { 0x49, yc::key::F7 },
    { 0x4a, yc::key::F8 },
    { 0x4b, yc::key::F9 },
    { 0x4c, yc::key::F10 },
    { 0x5f, yc::key::F11 },
    { 0x60, yc::key::F12 },

    /* Key state modifier keys */
    { 0x32, yc::key::SHIFT_L },
    { 0x3e, yc::key::SHIFT_R },
    { 0x25, yc::key::CONTROL_L },
    { 0x69, yc::key::CONTROL_R },
    { 0x42, yc::key::CAPS_LOCK },
    //    { XK_space, yc::key::SHIFT_LOCK },
    { 0x85, yc::key::META_L },
    { 0x86, yc::key::META_R },
    { 0x40, yc::key::ALT_L },
    { 0x6c, yc::key::ALT_R },
    //    { XK_space, yc::key::SUPER_L },
    //    { XK_space, yc::key::SUPER_R },
    //    { XK_space, yc::key::HYPER_L },
    //    { XK_space, yc::key::HYPER_R },
  };

#if USE_DEBUG
  if (keymap.find(keycode) == keymap.end())
  {
    logging(
          LOG_WARN, "Key not found: 0x%x (%c)", keycode,
          static_cast<char>(keycode));
    return yc::key::UNKNOWN;
  }
#endif

  /* Fall if key is not found */
  return keymap.at(keycode);
}

yc::mouse convert_button(unsigned int button)
{
  switch (button)
  {
    case Button1:
      return yc::mouse::LEFT;
    case Button2:
      return yc::mouse::MIDDLE;
    case Button3:
      return yc::mouse::RIGHT;
    case Button4:
      return yc::mouse::X1;
    case Button5:
      return yc::mouse::X2;
    default:
      return yc::mouse::UNKNOWN;
  }
}

/* Shared functions ========================================================= */
yc::error yc::x11::window::open()
{
  if (opened)
  {
    logging(LOG_ERROR, "Window is already opened!");
    return yc::error::INVALID_STATE;
  }

  if (settings.width <= 0 || settings.height <= 0)
  {
    logging(LOG_ERROR, "Invalid window size");
    return yc::error::INVALID_ARGUMENT;
  }

  working_thread = std::this_thread::get_id();

  logging(LOG_DEBUG, "Opening X11 window");
  display = XOpenDisplay(nullptr);
  if (display == nullptr)
  {
    logging(LOG_ERROR, "Cannot open display");
    return yc::error::EXTERNAL_API_ERROR;
  }
  int screenId = DefaultScreen(display);

  /* Check GLX version */
  GLint majorGLX, minorGLX = 0;
  glXQueryVersion(display, &majorGLX, &minorGLX);
  logging(LOG_DEBUG, "GLX version: %d.%d", majorGLX, minorGLX);
  if (majorGLX <= 1 && minorGLX < 2)
  {
    logging(LOG_ERROR, "GLX 1.2 or greater is required");
    XCloseDisplay(display);
    return yc::error::EXTERNAL_API_ERROR;
  }

  /*
   * Pick the FB config/visual with the most samples per pixel
   */
  // clang-format off
  GLint glxAttribs[] = {
    GLX_X_RENDERABLE,   True,
    GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,    GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
    GLX_RED_SIZE,       8,
    GLX_GREEN_SIZE,     8,
    GLX_BLUE_SIZE,      8,
    GLX_ALPHA_SIZE,     8,
    GLX_DEPTH_SIZE,     24,
    GLX_STENCIL_SIZE,   8,
    GLX_DOUBLEBUFFER,   True,
    None,
  };
  // clang-format on

  logging(LOG_DEBUG, "Getting framebuffer config for screen %d", screenId);
  int           fbcount;
  GLXFBConfig * fbc
        = glXChooseFBConfig(display, screenId, glxAttribs, &fbcount);
  if (fbc == nullptr)
  {
    logging(LOG_ERROR, "Failed to retrieve a framebuffer config");
    XCloseDisplay(display);
    return yc::error::EXTERNAL_API_ERROR;
  }

  int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
  for (int i = 0; i < fbcount; ++i)
  {
    XVisualInfo * vi = glXGetVisualFromFBConfig(display, fbc[i]);
    if (vi != 0)
    {
      int samp_buf, samples;
      glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
      glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

      logging(
            LOG_DEBUG,
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
    logging(LOG_ERROR, "Could not create correct visual window");
    XCloseDisplay(display);
    return yc::error::EXTERNAL_API_ERROR;
  }

  if (screenId != visual->screen)
  {
    logging(LOG_ERROR, "screenId != visual->screen");
    XCloseDisplay(display);
    return yc::error::EXTERNAL_API_ERROR;
  }

  /*
   * Open the window
   */
  XSetWindowAttributes windowAttribs;
  windowAttribs.border_pixel     = BlackPixel(display, screenId);
  windowAttribs.background_pixel = WhitePixel(display, screenId);
  windowAttribs.colormap
        = (colormap = XCreateColormap(
                 display, RootWindow(display, screenId), visual->visual,
                 AllocNone));
  windowAttribs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask
                             | ButtonPressMask | ButtonReleaseMask
                             | PointerMotionMask;
  win = XCreateWindow(
        display, RootWindow(display, screenId), 0, 0, settings.width,
        settings.height, 0, visual->depth, InputOutput, visual->visual,
        CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &windowAttribs);

  /*
   * Setup window properties
   */
  set_mode(settings.win_mode);
  yc::graphics::geometry geometry = { 0, 0, settings.width, settings.height };
  set_geometry(geometry);
  set_title(settings.title);

  /*
   * Setup GLX
   */
  logging(LOG_DEBUG, "Creating GLX context");
  glx_context
        = glXCreateNewContext(display, bestFbc, GLX_RGBA_TYPE, nullptr, True);
  if (!glx_context)
  {
    logging(LOG_ERROR, "Failed to create GLX context");
    XCloseDisplay(display);
    return yc::error::EXTERNAL_API_ERROR;
  }

  XSync(display, False);

  /* Verifying that context is a direct context */
  if (!glXIsDirect(display, glx_context))
  {
    logging(LOG_INFO, "Indirect GLX rendering context obtained");
  }
  else
  {
    logging(LOG_INFO, "Direct GLX rendering context obtained");
  }

  logging(LOG_DEBUG, "Making GLX context current");
  glXMakeCurrent(display, win, glx_context);

  graphics = std::make_shared<yc::gl::graphics>(shared_from_this());
  if (graphics->init(16, settings.width, settings.height) != yc::error::OK)
  {
    logging(LOG_ERROR, "Failed to initialize graphics context");
    XCloseDisplay(display);
    return yc::error::EXTERNAL_API_ERROR;
  }

  /*
   * Setup input method
   */
  // loads the XMODIFIERS environment variable to see what IME to use
  XSetLocaleModifiers("");

  XIM xim = XOpenIM(display, nullptr, nullptr, nullptr);
  if (!xim)
  {
    // fallback to internal input method
    XSetLocaleModifiers("@im=none");
    xim = XOpenIM(display, nullptr, nullptr, nullptr);
  }

  /* Show the window */
  XClearWindow(display, win);
  XMapRaised(display, win);

  xic = XCreateIC(
        xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow,
        win, XNFocusWindow, win, nullptr);

  XSetICFocus(xic);

  wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", false);
  XSetWMProtocols(display, win, &wmDeleteMessage, 1);

  event_loop.start();

  opened = true;

  return yc::error::OK;
}

yc::error yc::x11::window::set_mode(yc::graphics::settings::window_mode mode)
{
  if (working_thread != std::this_thread::get_id())
  {
    logging(LOG_ERROR, "Close called from different thread!");
    return yc::error::INVALID_THREAD;
  }

  logging(LOG_DEBUG, "Setting window mode to %d", static_cast<int>(mode));
  settings.win_mode = mode;

  switch (settings.win_mode)
  {
    case yc::graphics::settings::window_mode::FULLSCREEN:
    {
      Atom atoms[2]
            = { XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False), None };
      XChangeProperty(
            display, win, XInternAtom(display, "_NET_WM_STATE", False), 4, 32,
            PropModeReplace, reinterpret_cast<const unsigned char *>(atoms), 1);

      settings.width  = DisplayWidth(display, DefaultScreen(display));
      settings.height = DisplayHeight(display, DefaultScreen(display));
    }
    break;

    case yc::graphics::settings::window_mode::WINDOWED_RESIZABLE:
    {
      XSizeHints sizeHints = {};
      sizeHints.flags      = PSize | PMinSize | PMaxSize;
      sizeHints.min_width  = 100; // Minimum width
      sizeHints.min_height = 100; // Minimum height
      sizeHints.max_width  = DisplayWidth(display, DefaultScreen(display));
      sizeHints.max_height = DisplayHeight(display, DefaultScreen(display));
      XSetWMNormalHints(display, win, &sizeHints);
    }
    break;

    case yc::graphics::settings::window_mode::WINDOWED_FIXED:
    {
      XSizeHints sizeHints = {};
      sizeHints.flags      = PSize | PMinSize | PMaxSize;
      sizeHints.min_width  = settings.width;
      sizeHints.min_height = settings.height;
      sizeHints.max_width  = settings.width;
      sizeHints.max_height = settings.height;
      XSetWMNormalHints(display, win, &sizeHints);
    }
    break;
  }

  return yc::error::OK;
}

yc::error yc::x11::window::set_geometry(yc::graphics::geometry & geometry)
{
  if (working_thread != std::this_thread::get_id())
  {
    logging(LOG_ERROR, "Close called from different thread!");
    return yc::error::INVALID_THREAD;
  }

  logging(
        LOG_DEBUG, "Setting window geometry to %d x %d", geometry.width,
        geometry.height);

  settings.width  = geometry.width;
  settings.height = geometry.height;

  XWindowChanges changes;
  changes.width  = geometry.width;
  changes.height = geometry.height;
  XConfigureWindow(display, win, CWWidth | CWHeight, &changes);

  return yc::error::OK;
}

yc::error yc::x11::window::set_title(std::string title)
{
  if (working_thread != std::this_thread::get_id())
  {
    logging(LOG_ERROR, "Close called from different thread!");
    return yc::error::INVALID_THREAD;
  }

  logging(LOG_DEBUG, "Setting window title to %s", title.c_str());

  settings.title = title;

  /* Set window title */
  if (!title.empty())
  {
    XStoreName(display, win, title.c_str());
  }

  return yc::error::OK;
}

yc::error yc::x11::window::update()
{
  if (working_thread != std::this_thread::get_id())
  {
    logging(LOG_ERROR, "Close called from different thread!");
    return yc::error::INVALID_THREAD;
  }

  if (should_close)
  {
    close();
    return yc::error::OK;
  }
  if (!opened)
  {
    return yc::error::OK;
  }

  glXSwapBuffers(display, win);

  return yc::error::OK;
}

yc::error yc::x11::window::close()
{
  should_close = true;

  if (!opened)
  {
    return yc::error::OK;
  }

  if (working_thread != std::this_thread::get_id())
  {
    logging(LOG_WARN, "Close called from different thread!");
    return yc::error::OK;
  }

  logging(LOG_INFO, "Closing window");

  /* send event to unblock event loop */
  XEvent ev;
  memset(&ev, 0, sizeof(ev));
  ev.type                 = ClientMessage;
  ev.xclient.window       = win;
  ev.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", True);
  ev.xclient.format       = 32;
  ev.xclient.data.l[0]    = wmDeleteMessage;
  ev.xclient.data.l[1]    = CurrentTime;
  logging(LOG_DEBUG, "Sending close signal to event_loop");
  XSendEvent(display, win, False, NoEventMask, &ev);
  XFlush(display);
  event_loop.join();

  if (before_close_callback)
    before_close_callback();

  logging(LOG_DEBUG, "Destroying X11 window");
  glXDestroyContext(display, glx_context);

  logging(LOG_DEBUG, "Closing X11 display");
  XFree(visual);
  XFreeColormap(display, colormap);
  XDestroyWindow(display, win);
  XCloseDisplay(display);

  opened = false;

  return yc::error::OK;
}

bool yc::x11::window::is_open()
{
  return opened;
}

std::shared_ptr<yc::graphics::graphics> yc::x11::window::get_graphics()
{
  return graphics;
}

void yc::x11::window::event_loop::run()
{
  while (true)
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
        if (static_cast<long>(ev.xclient.data.l[0])
            == static_cast<long>(win->wmDeleteMessage))
        {
          win->should_close = true;
          return;
        }
        break;

      case KeyPress:
      {
        yc::key key = convert_key(ev.xkey.keycode);
        if (key != yc::key::UNKNOWN)
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

          //          for (wchar_t wc;
          //               (ret = mbrtowc(&wc, p, buf + sizeof(buf) - p,
          //               nullptr)) > 0; p += ret)
          //          {
          //            win->input.char_entered.push(wc);
          //          }
        }

        break;
      }

      case KeyRelease:
      {
        yc::key key = convert_key(ev.xkey.keycode);
        if (key != yc::key::UNKNOWN)
        {
          win->input.key_released.push(key);
        }

        break;
      }

      case ButtonPress:
      {
        struct yc::graphics::mouse_click_data click_data
              = { convert_button(ev.xbutton.button),
                  static_cast<float>(ev.xbutton.x),
                  static_cast<float>(ev.xbutton.y) };
        win->input.mouse_pressed.push(click_data);
        break;
      }

      case ButtonRelease:
      {
        struct yc::graphics::mouse_click_data click_data
              = { convert_button(ev.xbutton.button),
                  static_cast<float>(ev.xbutton.x),
                  static_cast<float>(ev.xbutton.y) };
        win->input.mouse_released.push(click_data);
        break;
      }

      case MotionNotify:
      {
        struct yc::graphics::mouse_moved_data pos
              = { static_cast<float>(ev.xmotion.x),
                  static_cast<float>(ev.xmotion.y) };
        win->input.mouse_moved.push(pos);
        break;
      }

      default:
        break;
    }
  }
}
