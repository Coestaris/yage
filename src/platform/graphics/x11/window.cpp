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

#include <GL/gl.h>
#include <cstring>
#include <thread>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
static yage::core::key convert_key(uint32_t keycode)
{
  static const std::unordered_map<uint32_t, yage::core::key> keymap = {
    /* Control keys */
    { 0x24, yage::core::key::ENTER },
    { 0x77, yage::core::key::DEL },
    { 0x16, yage::core::key::BACKSPACE },
    { 0x09, yage::core::key::ESCAPE },
    { 0x17, yage::core::key::TAB },
    { 0x41, yage::core::key::SPACE },
    { 0x4e, yage::core::key::SCROLL_LOCK },
    { 0x7f, yage::core::key::PAUSE },
    // { XK_Sys_Req, yage::core::key::SYS_REQ },
    { 0x6e, yage::core::key::HOME },
    { 0x71, yage::core::key::LEFT },
    { 0x6f, yage::core::key::UP },
    { 0x72, yage::core::key::RIGHT },
    { 0x74, yage::core::key::DOWN },
    // { XK_Prior, yage::core::key::PRIOR },
    { 0x70, yage::core::key::PAGE_UP },
    // { XK_Next, yage::core::key::NEXT },
    { 0x75, yage::core::key::PAGE_DOWN },
    { 0x73, yage::core::key::END },
    // { XK_Begin, yage::core::key::BEGIN },

    /* Special keys */
    { 0x2f, yage::core::key::SK1 },
    { 0x3d, yage::core::key::SK2 },
    { 0x33, yage::core::key::SK3 },
    { 0x22, yage::core::key::SK4 },
    { 0x23, yage::core::key::SK5 },
    { 0x30, yage::core::key::SK6 },
    { 0x31, yage::core::key::SK7 },
    { 0x3b, yage::core::key::SK8 },
    { 0x3c, yage::core::key::SK9 },
    { 0x14, yage::core::key::SK10 },
    { 0x15, yage::core::key::SK11 },

    /* Letters */
    { 0x26, yage::core::key::A },
    { 0x38, yage::core::key::B },
    { 0x36, yage::core::key::C },
    { 0x28, yage::core::key::D },
    { 0x1a, yage::core::key::E },
    { 0x29, yage::core::key::F },
    { 0x2a, yage::core::key::G },
    { 0x2b, yage::core::key::H },
    { 0x1f, yage::core::key::I },
    { 0x2c, yage::core::key::J },
    { 0x2d, yage::core::key::K },
    { 0x2e, yage::core::key::L },
    { 0x3a, yage::core::key::M },
    { 0x39, yage::core::key::N },
    { 0x20, yage::core::key::O },
    { 0x21, yage::core::key::P },
    { 0x18, yage::core::key::Q },
    { 0x1b, yage::core::key::R },
    { 0x27, yage::core::key::S },
    { 0x1c, yage::core::key::T },
    { 0x1e, yage::core::key::U },
    { 0x37, yage::core::key::V },
    { 0x19, yage::core::key::W },
    { 0x35, yage::core::key::X },
    { 0x1d, yage::core::key::Y },
    { 0x34, yage::core::key::Z },

    /* Numbers */
    { 0x13, yage::core::key::K0 },
    { 0xa, yage::core::key::K1 },
    { 0xb, yage::core::key::K2 },
    { 0xc, yage::core::key::K3 },
    { 0xd, yage::core::key::K4 },
    { 0xe, yage::core::key::K5 },
    { 0xf, yage::core::key::K6 },
    { 0x10, yage::core::key::K7 },
    { 0x11, yage::core::key::K8 },
    { 0x12, yage::core::key::K9 },

    /* Misc functions */
    //    { XK_space, yage::core::key::SELECT },
    //    { XK_space, yage::core::key::PRINT },
    //    { XK_space, yage::core::key::EXECUTE },
    { 0x76, yage::core::key::INSERT },
    //    { XK_space, yage::core::key::UNDO },
    //    { XK_space, yage::core::key::REDO },
    //    { XK_space, yage::core::key::MENU },
    //    { XK_space, yage::core::key::FIND },
    //    { XK_space, yage::core::key::CANCEL },
    //    { XK_space, yage::core::key::HELP },
    //    { XK_space, yage::core::key::BREAK },
    //    { XK_space, yage::core::key::CLEAR },
    { 0x4d, yage::core::key::NUM_LOCK },

    /* Keypad functions */
    //    { XK_space, yage::core::key::KP_SPACE },
    //    { XK_space, yage::core::key::KP_TAB },
    { 0x68, yage::core::key::KP_ENTER },
    //    { XK_space, yage::core::key::KP_HOME },
    //    { XK_space, yage::core::key::KP_LEFT },
    //    { XK_space, yage::core::key::KP_UP },
    //    { XK_space, yage::core::key::KP_RIGHT },
    //    { XK_space, yage::core::key::KP_DOWN },
    //    { XK_space, yage::core::key::KP_PRIOR },
    //    { XK_space, yage::core::key::KP_PAGE_UP },
    //    { XK_space, yage::core::key::KP_NEXT },
    //    { XK_space, yage::core::key::KP_PAGE_DOWN },
    //    { XK_space, yage::core::key::KP_END },
    //    { XK_space, yage::core::key::KP_BEGIN },
    //    { XK_space, yage::core::key::KP_INSERT },
    //    { XK_space, yage::core::key::KP_EQUAL },
    { 0x3f, yage::core::key::KP_MULTIPLY },
    { 0x56, yage::core::key::KP_ADD },
    //    { XK_space, yage::core::key::KP_SEPARATOR },
    { 0x52, yage::core::key::KP_SUBTRACT },
    { 0x5b, yage::core::key::KP_DECIMAL },
    { 0x6a, yage::core::key::KP_DIVIDE },
    { 0x5a, yage::core::key::KP_0 },
    { 0x57, yage::core::key::KP_1 },
    { 0x58, yage::core::key::KP_2 },
    { 0x59, yage::core::key::KP_3 },
    { 0x53, yage::core::key::KP_4 },
    { 0x54, yage::core::key::KP_5 },
    { 0x55, yage::core::key::KP_6 },
    { 0x4f, yage::core::key::KP_7 },
    { 0x50, yage::core::key::KP_8 },
    { 0x51, yage::core::key::KP_9 },

    /* Function keys */
    { 0x43, yage::core::key::F1 },
    { 0x44, yage::core::key::F2 },
    { 0x45, yage::core::key::F3 },
    { 0x46, yage::core::key::F4 },
    { 0x47, yage::core::key::F5 },
    { 0x48, yage::core::key::F6 },
    { 0x49, yage::core::key::F7 },
    { 0x4a, yage::core::key::F8 },
    { 0x4b, yage::core::key::F9 },
    { 0x4c, yage::core::key::F10 },
    { 0x5f, yage::core::key::F11 },
    { 0x60, yage::core::key::F12 },

    /* Key state modifier keys */
    { 0x32, yage::core::key::SHIFT_L },
    { 0x3e, yage::core::key::SHIFT_R },
    { 0x25, yage::core::key::CONTROL_L },
    { 0x69, yage::core::key::CONTROL_R },
    { 0x42, yage::core::key::CAPS_LOCK },
    //    { XK_space, yage::core::key::SHIFT_LOCK },
    { 0x85, yage::core::key::META_L },
    { 0x86, yage::core::key::META_R },
    { 0x40, yage::core::key::ALT_L },
    { 0x6c, yage::core::key::ALT_R },
    //    { XK_space, yage::core::key::SUPER_L },
    //    { XK_space, yage::core::key::SUPER_R },
    //    { XK_space, yage::core::key::HYPER_L },
    //    { XK_space, yage::core::key::HYPER_R },
  };

#if USE_DEBUG
  if (keymap.find(keycode) == keymap.end())
  {
    logging(LOG_WARN, "Key not found: 0x%x (%c)", keycode,
        static_cast<char>(keycode));
    return yage::core::key::UNKNOWN;
  }
#endif

  /* Fall if key is not found */
  return keymap.at(keycode);
}

yage::core::mouse convert_button(unsigned int button)
{
  switch (button)
  {
    case Button1:
      return yage::core::mouse::LEFT;
    case Button2:
      return yage::core::mouse::MIDDLE;
    case Button3:
      return yage::core::mouse::RIGHT;
    case Button4:
      return yage::core::mouse::X1;
    case Button5:
      return yage::core::mouse::X2;
    default:
      return yage::core::mouse::UNKNOWN;
  }
}

/* Shared functions ========================================================= */
yage::core::error yage::platform::graphics::x11::window::open()
{
  if (opened)
  {
    logging(LOG_ERROR, "Window is already opened!");
    return yage::core::error::INVALID_STATE;
  }

  if (settings.width <= 0 || settings.height <= 0)
  {
    logging(LOG_ERROR, "Invalid window size");
    return yage::core::error::INVALID_ARGUMENT;
  }

  working_thread = std::this_thread::get_id();

  logging(LOG_DEBUG, "Opening X11 window");
  display = XOpenDisplay(NULL);
  if (display == NULL)
  {
    logging(LOG_ERROR, "Cannot open display");
    return yage::core::error::EXTERNAL_API_ERROR;
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
    return yage::core::error::EXTERNAL_API_ERROR;
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
    return yage::core::error::EXTERNAL_API_ERROR;
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

      logging(LOG_DEBUG,
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
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  if (screenId != visual->screen)
  {
    logging(LOG_ERROR, "screenId != visual->screen");
    XCloseDisplay(display);
    return yage::core::error::EXTERNAL_API_ERROR;
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
  yage::graphics::geometry geometry = { 0, 0, settings.width, settings.height };
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
    return yage::core::error::EXTERNAL_API_ERROR;
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

  if (context.init(16, settings.width, settings.height)
      != yage::core::error::OK)
  {
    logging(LOG_ERROR, "Failed to initialize graphics context");
    XCloseDisplay(display);
    return yage::core::error::EXTERNAL_API_ERROR;
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

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::x11::window::set_mode(
      yage::graphics::settings::window_mode mode)
{
  if (working_thread != std::this_thread::get_id())
  {
    logging(LOG_ERROR, "Close called from different thread!");
    return yage::core::error::INVALID_THREAD;
  }

  logging(LOG_DEBUG, "Setting window mode to %d", static_cast<int>(mode));
  settings.win_mode = mode;

  switch (settings.win_mode)
  {
    case yage::graphics::settings::window_mode::FULLSCREEN:
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

    case yage::graphics::settings::window_mode::WINDOWED_RESIZABLE:
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

    case yage::graphics::settings::window_mode::WINDOWED_FIXED:
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

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::x11::window::set_geometry(
      yage::graphics::geometry & geometry)
{
  if (working_thread != std::this_thread::get_id())
  {
    logging(LOG_ERROR, "Close called from different thread!");
    return yage::core::error::INVALID_THREAD;
  }

  logging(LOG_DEBUG, "Setting window geometry to %d x %d", geometry.width,
      geometry.height);

  settings.width  = geometry.width;
  settings.height = geometry.height;

  XWindowChanges changes;
  changes.width  = geometry.width;
  changes.height = geometry.height;
  XConfigureWindow(display, win, CWWidth | CWHeight, &changes);

  return yage::core::error::OK;
}

yage::core::error
yage::platform::graphics::x11::window::set_title(std::string title)
{
  if (working_thread != std::this_thread::get_id())
  {
    logging(LOG_ERROR, "Close called from different thread!");
    return yage::core::error::INVALID_THREAD;
  }

  logging(LOG_DEBUG, "Setting window title to %s", title.c_str());

  settings.title = title;

  /* Set window title */
  if (!title.empty())
  {
    XStoreName(display, win, title.c_str());
  }

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::x11::window::update()
{
  if (working_thread != std::this_thread::get_id())
  {
    logging(LOG_ERROR, "Close called from different thread!");
    return yage::core::error::INVALID_THREAD;
  }

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
  should_close = true;

  if (!opened)
  {
    return yage::core::error::OK;
  }

  if (working_thread != std::this_thread::get_id())
  {
    logging(LOG_WARN, "Close called from different thread!");
    return yage::core::error::OK;
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
        yage::core::key key = convert_key(ev.xkey.keycode);
        if (key != yage::core::key::UNKNOWN)
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
        yage::core::key key = convert_key(ev.xkey.keycode);
        if (key != yage::core::key::UNKNOWN)
        {
          win->input.key_released.push(key);
        }

        break;
      }

      case ButtonPress:
      {
        struct yage::graphics::mouse_click_data click_data
              = { convert_button(ev.xbutton.button),
                  static_cast<float>(ev.xbutton.x),
                  static_cast<float>(ev.xbutton.y) };
        win->input.mouse_pressed.push(click_data);
        break;
      }

      case ButtonRelease:
      {
        struct yage::graphics::mouse_click_data click_data
              = { convert_button(ev.xbutton.button),
                  static_cast<float>(ev.xbutton.x),
                  static_cast<float>(ev.xbutton.y) };
        win->input.mouse_released.push(click_data);
        break;
      }

      case MotionNotify:
      {
        struct yage::graphics::mouse_moved_data pos
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
