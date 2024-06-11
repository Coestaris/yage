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

  XSelectInput(
        display, win,
        KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask
              | PointerMotionMask);

  wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, win, &wmDeleteMessage, 1);

  opened = true;

  return yage::core::error::OK;
}

yage::graphics::key convert_key(KeySym sym)
{
  static const std::unordered_map<KeySym, yage::graphics::key> map = {
    { XK_space, yage::graphics::key::space },
    { XK_exclam, yage::graphics::key::exclam },
    { XK_quotedbl, yage::graphics::key::quotedbl },
    { XK_numbersign, yage::graphics::key::numbersign },
    { XK_dollar, yage::graphics::key::dollar },
    { XK_percent, yage::graphics::key::percent },
    { XK_ampersand, yage::graphics::key::ampersand },
    { XK_apostrophe, yage::graphics::key::apostrophe },
    { XK_quoteright, yage::graphics::key::quoteright },
    { XK_parenleft, yage::graphics::key::parenleft },
    { XK_parenright, yage::graphics::key::parenright },
    { XK_asterisk, yage::graphics::key::asterisk },
    { XK_plus, yage::graphics::key::plus },
    { XK_comma, yage::graphics::key::comma },
    { XK_minus, yage::graphics::key::minus },
    { XK_period, yage::graphics::key::period },
    { XK_slash, yage::graphics::key::slash },
    { XK_0, yage::graphics::key::k_0 },
    { XK_1, yage::graphics::key::k_1 },
    { XK_2, yage::graphics::key::k_2 },
    { XK_3, yage::graphics::key::k_3 },
    { XK_4, yage::graphics::key::k_4 },
    { XK_5, yage::graphics::key::k_5 },
    { XK_6, yage::graphics::key::k_6 },
    { XK_7, yage::graphics::key::k_7 },
    { XK_8, yage::graphics::key::k_8 },
    { XK_9, yage::graphics::key::k_9 },
    { XK_colon, yage::graphics::key::colon },
    { XK_semicolon, yage::graphics::key::semicolon },
    { XK_less, yage::graphics::key::less },
    { XK_equal, yage::graphics::key::equal },
    { XK_greater, yage::graphics::key::greater },
    { XK_question, yage::graphics::key::question },
    { XK_at, yage::graphics::key::at },
    { XK_A, yage::graphics::key::k_A },
    { XK_B, yage::graphics::key::k_B },
    { XK_C, yage::graphics::key::k_C },
    { XK_D, yage::graphics::key::k_D },
    { XK_E, yage::graphics::key::k_E },
    { XK_F, yage::graphics::key::k_F },
    { XK_G, yage::graphics::key::k_G },
    { XK_H, yage::graphics::key::k_H },
    { XK_I, yage::graphics::key::k_I },
    { XK_J, yage::graphics::key::k_J },
    { XK_K, yage::graphics::key::k_K },
    { XK_L, yage::graphics::key::k_L },
    { XK_M, yage::graphics::key::k_M },
    { XK_N, yage::graphics::key::k_N },
    { XK_O, yage::graphics::key::k_O },
    { XK_P, yage::graphics::key::k_P },
    { XK_Q, yage::graphics::key::k_Q },
    { XK_R, yage::graphics::key::k_R },
    { XK_S, yage::graphics::key::k_S },
    { XK_T, yage::graphics::key::k_T },
    { XK_U, yage::graphics::key::k_U },
    { XK_V, yage::graphics::key::k_V },
    { XK_W, yage::graphics::key::k_W },
    { XK_X, yage::graphics::key::k_X },
    { XK_Y, yage::graphics::key::k_Y },
    { XK_Z, yage::graphics::key::k_Z },
    { XK_bracketleft, yage::graphics::key::bracketleft },
    { XK_backslash, yage::graphics::key::backslash },
    { XK_bracketright, yage::graphics::key::bracketright },
    { XK_asciicircum, yage::graphics::key::asciicircum },
    { XK_underscore, yage::graphics::key::underscore },
    { XK_grave, yage::graphics::key::grave },
    { XK_quoteleft, yage::graphics::key::quoteleft },
    { XK_a, yage::graphics::key::k_a },
    { XK_b, yage::graphics::key::k_b },
    { XK_c, yage::graphics::key::k_c },
    { XK_d, yage::graphics::key::k_d },
    { XK_e, yage::graphics::key::k_e },
    { XK_f, yage::graphics::key::k_f },
    { XK_g, yage::graphics::key::k_g },
    { XK_h, yage::graphics::key::k_h },
    { XK_i, yage::graphics::key::k_i },
    { XK_j, yage::graphics::key::k_j },
    { XK_k, yage::graphics::key::k_k },
    { XK_l, yage::graphics::key::k_l },
    { XK_m, yage::graphics::key::k_m },
    { XK_n, yage::graphics::key::k_n },
    { XK_o, yage::graphics::key::k_o },
    { XK_p, yage::graphics::key::k_p },
    { XK_q, yage::graphics::key::k_q },
    { XK_r, yage::graphics::key::k_r },
    { XK_s, yage::graphics::key::k_s },
    { XK_t, yage::graphics::key::k_t },
    { XK_u, yage::graphics::key::k_u },
    { XK_v, yage::graphics::key::k_v },
    { XK_w, yage::graphics::key::k_w },
    { XK_x, yage::graphics::key::k_x },
    { XK_y, yage::graphics::key::k_y },
    { XK_z, yage::graphics::key::k_z },
    { XK_braceleft, yage::graphics::key::braceleft },
    { XK_bar, yage::graphics::key::bar },
    { XK_braceright, yage::graphics::key::braceright },
    { XK_asciitilde, yage::graphics::key::asciitilde },
    { XK_Home, yage::graphics::key::Home },
    { XK_Left, yage::graphics::key::Left },
    { XK_Up, yage::graphics::key::Up },
    { XK_Right, yage::graphics::key::Right },
    { XK_Down, yage::graphics::key::Down },
    { XK_Prior, yage::graphics::key::Prior },
    { XK_Page_Up, yage::graphics::key::Page_Up },
    { XK_Next, yage::graphics::key::Next },
    { XK_Page_Down, yage::graphics::key::Page_Down },
    { XK_End, yage::graphics::key::End },
    { XK_Begin, yage::graphics::key::Begin },
    { XK_Select, yage::graphics::key::Select },
    { XK_Print, yage::graphics::key::Print },
    { XK_Execute, yage::graphics::key::Execute },
    { XK_Insert, yage::graphics::key::Insert },
    { XK_Undo, yage::graphics::key::Undo },
    { XK_Redo, yage::graphics::key::Redo },
    { XK_Menu, yage::graphics::key::Menu },
    { XK_Find, yage::graphics::key::Find },
    { XK_Cancel, yage::graphics::key::Cancel },
    { XK_Help, yage::graphics::key::Help },
    { XK_Break, yage::graphics::key::Break },
    { XK_Mode_switch, yage::graphics::key::Mode_switch },
    { XK_script_switch, yage::graphics::key::script_switch },
    { XK_Num_Lock, yage::graphics::key::Num_Lock },
    { XK_KP_Space, yage::graphics::key::KP_Space },
    { XK_KP_Tab, yage::graphics::key::KP_Tab },
    { XK_KP_Enter, yage::graphics::key::KP_Enter },
    { XK_KP_Home, yage::graphics::key::KP_Home },
    { XK_KP_Left, yage::graphics::key::KP_Left },
    { XK_KP_Up, yage::graphics::key::KP_Up },
    { XK_KP_Right, yage::graphics::key::KP_Right },
    { XK_KP_Down, yage::graphics::key::KP_Down },
    { XK_KP_Prior, yage::graphics::key::KP_Prior },
    { XK_KP_Page_Up, yage::graphics::key::KP_Page_Up },
    { XK_KP_Next, yage::graphics::key::KP_Next },
    { XK_KP_Page_Down, yage::graphics::key::KP_Page_Down },
    { XK_KP_End, yage::graphics::key::KP_End },
    { XK_KP_Begin, yage::graphics::key::KP_Begin },
    { XK_KP_Insert, yage::graphics::key::KP_Insert },
    { XK_KP_Delete, yage::graphics::key::KP_Delete },
    { XK_KP_Equal, yage::graphics::key::KP_Equal },
    { XK_KP_Multiply, yage::graphics::key::KP_Multiply },
    { XK_KP_Add, yage::graphics::key::KP_Add },
    { XK_KP_Separator, yage::graphics::key::KP_Separator },
    { XK_KP_Subtract, yage::graphics::key::KP_Subtract },
    { XK_KP_Decimal, yage::graphics::key::KP_Decimal },
    { XK_KP_Divide, yage::graphics::key::KP_Divide },
    { XK_KP_0, yage::graphics::key::KP_0 },
    { XK_KP_1, yage::graphics::key::KP_1 },
    { XK_KP_2, yage::graphics::key::KP_2 },
    { XK_KP_3, yage::graphics::key::KP_3 },
    { XK_KP_4, yage::graphics::key::KP_4 },
    { XK_KP_5, yage::graphics::key::KP_5 },
    { XK_KP_6, yage::graphics::key::KP_6 },
    { XK_KP_7, yage::graphics::key::KP_7 },
    { XK_KP_8, yage::graphics::key::KP_8 },
    { XK_KP_9, yage::graphics::key::KP_9 },
    { XK_F1, yage::graphics::key::F1 },
    { XK_F2, yage::graphics::key::F2 },
    { XK_F3, yage::graphics::key::F3 },
    { XK_F4, yage::graphics::key::F4 },
    { XK_F5, yage::graphics::key::F5 },
    { XK_F6, yage::graphics::key::F6 },
    { XK_F7, yage::graphics::key::F7 },
    { XK_F8, yage::graphics::key::F8 },
    { XK_F9, yage::graphics::key::F9 },
    { XK_F10, yage::graphics::key::F10 },
    { XK_F11, yage::graphics::key::F11 },
    { XK_F12, yage::graphics::key::F12 },
    { XK_Shift_L, yage::graphics::key::Shift_L },
    { XK_Shift_R, yage::graphics::key::Shift_R },
    { XK_Control_L, yage::graphics::key::Control_L },
    { XK_Control_R, yage::graphics::key::Control_R },
    { XK_Caps_Lock, yage::graphics::key::Caps_Lock },
    { XK_Shift_Lock, yage::graphics::key::Shift_Lock },
    { XK_Meta_L, yage::graphics::key::Meta_L },
    { XK_Meta_R, yage::graphics::key::Meta_R },
    { XK_Alt_L, yage::graphics::key::Alt_L },
    { XK_Alt_R, yage::graphics::key::Alt_R },
    { XK_Super_L, yage::graphics::key::Super_L },
    { XK_Super_R, yage::graphics::key::Super_R },
    { XK_Hyper_L, yage::graphics::key::Hyper_L },
    { XK_Hyper_R, yage::graphics::key::Hyper_R },
  };

  /* Fall if key is not found */
  return map.at(sym);
}

yage::graphics::mouse_button convert_button(unsigned int button)
{
  switch (button)
  {
    case Button1:
      return yage::graphics::mouse_button::LEFT;
    case Button2:
      return yage::graphics::mouse_button::MIDDLE;
    case Button3:
      return yage::graphics::mouse_button::RIGHT;
    case Button4:
      return yage::graphics::mouse_button::X1;
    case Button5:
      return yage::graphics::mouse_button::X2;
    default:
      return yage::graphics::mouse_button::UNKNOWN;
  }
}

yage::core::error yage::platform::graphics::x11::window::update()
{
  if (!opened)
  {
    return yage::core::error::OK;
  }

  while (XPending(display))
  {
    XEvent ev;
    XNextEvent(display, &ev);

    switch (ev.type)
    {
      case Expose:
        break;

      case ClientMessage:
        /* Handle close event */
        if ((long)ev.xclient.data.l[0] == (long)wmDeleteMessage)
        {
          close();
          return yage::core::error::OK;
        }
        break;

      case KeyPress:
      {
        KeySym key = XLookupKeysym(&ev.xkey, 0);
        key_pressed.publish(convert_key(key));
        break;
      }

      case KeyRelease:
      {
        KeySym key = XLookupKeysym(&ev.xkey, 0);
        key_released.publish(convert_key(key));
        break;
      }

      case ButtonPress:
      {
        mouse_pressed.publish(std::pair(
              convert_button(ev.xbutton.button),
              (yage::graphics::mouse_position) { ev.xbutton.x, ev.xbutton.y }));
        break;
      }

      case ButtonRelease:
      {
        mouse_released.publish(std::pair(
              convert_button(ev.xbutton.button),
              (yage::graphics::mouse_position) { ev.xbutton.x, ev.xbutton.y }));
        break;
      }

      case MotionNotify:
      {
        mouse_moved.publish(
              (yage::graphics::mouse_position) { ev.xbutton.x, ev.xbutton.y });
        break;
      }

      default:
        break;
    }
  }

  glXSwapBuffers(display, win);

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::x11::window::close()
{
  yage::core::event<void> event;
  window_closed.publish(event);

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
