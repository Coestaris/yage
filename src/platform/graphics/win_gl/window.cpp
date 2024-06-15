/** ========================================================================
 *
 * @file X11Display.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "window.hpp"
#include "core/log.hpp"

#include <gl/gl.h>    // Header File For The OpenGL32 Library
#include <gl/glu.h>   // Header File For The GLu32 Library
#include <gl/glaux.h> // Header File For The Glaux Library

/* Defines ================================================================== */
#define BITS_PER_PIXEL 8

/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */

/* Static functions ========================================================= */
/* Shared functions ========================================================= */

static void log_pfd(PIXELFORMATDESCRIPTOR * pdf)
{
  log(LOG_DEBUG, "PIXELFORMATDESCRIPTOR:");
  log(LOG_DEBUG, "    nSize = %d", pdf->nSize);
  log(LOG_DEBUG, "    nVersion = %d", pdf->nVersion);
  log(LOG_DEBUG, "    dwFlags = %lu", pdf->dwFlags);
  log(LOG_DEBUG, "    iPixelType = %d", pdf->iPixelType);
  log(LOG_DEBUG, "    cColorBits = %d", pdf->cColorBits);
  log(LOG_DEBUG, "    cRedBits = %d", pdf->cRedBits);
  log(LOG_DEBUG, "    cRedShift = %d", pdf->cRedShift);
  log(LOG_DEBUG, "    cGreenBits = %d", pdf->cGreenBits);
  log(LOG_DEBUG, "    cGreenShift = %d", pdf->cGreenShift);
  log(LOG_DEBUG, "    cBlueBits = %d", pdf->cBlueBits);
  log(LOG_DEBUG, "    cBlueShift = %d", pdf->cBlueShift);
  log(LOG_DEBUG, "    cAlphaBits = %d", pdf->cAlphaBits);
  log(LOG_DEBUG, "    cAlphaShift = %d", pdf->cAlphaShift);
  log(LOG_DEBUG, "    cAccumBits = %d", pdf->cAccumBits);
  log(LOG_DEBUG, "    cAccumRedBits = %d", pdf->cAccumRedBits);
  log(LOG_DEBUG, "    cAccumGreenBits = %d", pdf->cAccumGreenBits);
  log(LOG_DEBUG, "    cAccumBlueBits = %d", pdf->cAccumBlueBits);
  log(LOG_DEBUG, "    cAccumAlphaBits = %d", pdf->cAccumAlphaBits);
  log(LOG_DEBUG, "    cDepthBits = %d", pdf->cDepthBits);
  log(LOG_DEBUG, "    cStencilBits = %d", pdf->cStencilBits);
  log(LOG_DEBUG, "    cAuxBuffers = %d", pdf->cAuxBuffers);
  log(LOG_DEBUG, "    iLayerType = %d", pdf->iLayerType);
  log(LOG_DEBUG, "    bReserved = %d", pdf->bReserved);
  log(LOG_DEBUG, "    dwLayerMask = %lu", pdf->dwLayerMask);
  log(LOG_DEBUG, "    dwVisibleMask = %lu", pdf->dwVisibleMask);
  log(LOG_DEBUG, "    dwDamageMask = %lu", pdf->dwDamageMask);
}

/* TODO: can be moved to common part,
 * since have a lot of common code with DirectX */
yage::core::error yage::platform::graphics::win_gl::window::open(
      std::string & title, yage::graphics::geometry & geometry, bool fullscreen)
{
  log(LOG_INFO, "Opening window: %s. Geometry: %d %d %d %d. Fullscreen: %d",
      title.c_str(), geometry.x, geometry.y, geometry.width, geometry.height,
      fullscreen);

  RECT WindowRect;
  WindowRect.left   = (long)0;
  WindowRect.right  = (long)geometry.width;
  WindowRect.top    = (long)0;
  WindowRect.bottom = (long)geometry.height;

  hInstance = GetModuleHandle(NULL);

  log(LOG_DEBUG, "hInstance: 0x%p", hInstance);

  WNDCLASS wc;
  /* Redraw On Size, And Own DC For Window. */
  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc   = (WNDPROC)wnd_proc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hInstance;
  wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO); /* Load The Default Icon */
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW); /* Load The Arrow Pointer */
  wc.hbrBackground = NULL;     /* No Background Required For GL */
  wc.lpszMenuName  = NULL;     /* We Don't Want A Menu */
  wc.lpszClassName = "OpenGL"; /* Set The Class Name */

  log(LOG_DEBUG, "Trying to register window class");
  if (!RegisterClass(&wc))
  {
    log(LOG_ERROR, "Failed To Register The Window Class: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  GLuint PixelFormat; /* Holds The Results After Searching For A Match */
  DWORD  dwExStyle;   /* Window Extended Style */
  DWORD  dwStyle;     /* Window Style */

  if (fullscreen)
  {
    DEVMODE dmScreenSettings;
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = geometry.width;
    dmScreenSettings.dmPelsHeight = geometry.height;
    dmScreenSettings.dmBitsPerPel = BITS_PER_PIXEL;
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    /* NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar. */
    log(LOG_DEBUG, "Trying to change display settings");
    if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN)
        != DISP_CHANGE_SUCCESSFUL)
    {
      log(LOG_ERROR, "The Requested Fullscreen Mode Is Not Supported: %lu",
          GetLastError());
      return yage::core::error::EXTERNAL_API_ERROR;
    }

    dwExStyle = WS_EX_APPWINDOW; /* Window Extended Style */
    dwStyle   = WS_POPUP;        /* Windows Style */
    ShowCursor(FALSE);           /* Hide Mouse Pointer */
  }
  else
  {
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; /* Window Extended Style */
    dwStyle   = WS_OVERLAPPEDWINDOW;                /* Windows Style */
  }

  log(LOG_DEBUG,
      "Trying to adjust window to true requested size. dw=0x%lx, dwEx=0x%lx",
      dwStyle, dwExStyle);
  if (!AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle))
  {
    log(LOG_ERROR, "Failed To Adjust Window Rect: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  log(LOG_DEBUG, "Trying to create window");
  if (!(hWnd = CreateWindowEx(
              dwExStyle,              /* Extended Style For The Window */
              "OpenGL",               /* Class Name */
              title.c_str(),          /* Window Title */
              dwStyle |               /* Defined Window Style */
                    WS_CLIPSIBLINGS | /* Required Window Style */
                    WS_CLIPCHILDREN,  /* Required Window Style */
              geometry.x,             /* Window Position */
              geometry.y,             /* Window Position */
              WindowRect.right - WindowRect.left, /* Calculate Window Width */
              WindowRect.bottom - WindowRect.top, /* Calculate Window Height */

              NULL,      /* No Parent Window */
              NULL,      /* No Menu */
              hInstance, /* Instance */
              NULL       /* Don't Pass Anything To WM_CREATE */
              )))
  {
    close();
    log(LOG_ERROR, "Window Creation Error: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  /* pfd Tells Windows How We Want Things To Be */
  static PIXELFORMATDESCRIPTOR pfd;
  pfd.nSize    = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags
        = PFD_DRAW_TO_WINDOW |     /* Format Must Support Window */
          PFD_SUPPORT_OPENGL |     /* Format Must Support OpenGL */
          PFD_DOUBLEBUFFER |       /* Must Support Double Buffering */
          PFD_GENERIC_ACCELERATED; /* Must Support Hardware Acceleration */

  pfd.iPixelType      = PFD_TYPE_RGBA;        /* Request An RGBA Format */
  pfd.cColorBits      = (BYTE)BITS_PER_PIXEL; /* Select Our Color Depth */
  pfd.cRedBits        = 0;
  pfd.cRedShift       = 0;
  pfd.cGreenBits      = 0;
  pfd.cGreenShift     = 0;
  pfd.cBlueBits       = 0;
  pfd.cBlueShift      = 0; /* Color Bits Ignored */
  pfd.cAlphaBits      = 0; /* No Alpha Buffer */
  pfd.cAlphaShift     = 0; /* Shift Bit Ignored */
  pfd.cAccumBits      = 0; /* No Accumulation Buffer */
  pfd.cAccumRedBits   = 0;
  pfd.cAccumGreenBits = 0;
  pfd.cAccumBlueBits  = 0;
  pfd.cAccumAlphaBits = 0;              /* Accumulation Bits Ignored */
  pfd.cDepthBits      = 16;             /* 16Bit Z-Buffer (Depth Buffer) */
  pfd.cStencilBits    = 0;              /* No Stencil Buffer */
  pfd.cAuxBuffers     = 0;              /* No Auxiliary Buffer */
  pfd.iLayerType      = PFD_MAIN_PLANE; /* Main Drawing Layer */
  pfd.bReserved       = 0;              /* Reserved */
  pfd.dwLayerMask     = 0;
  pfd.dwVisibleMask   = 0;
  pfd.dwDamageMask    = 0; /* Layer Masks Ignored */

  log_pfd(&pfd);

  log(LOG_DEBUG, "Trying to get device context. hWnd=0x%p", hWnd);
  if (!(hDC = GetDC(hWnd)))
  {
    close();
    log(LOG_ERROR, "Can't Create A GL Device Context: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  log(LOG_DEBUG, "Trying to find a suitable pixel format. hDC=0x%p", hDC);
  if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
  {
    close();
    log(LOG_ERROR, "Can't Find A Suitable PixelFormat: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  log(LOG_DEBUG, "Trying to set pixel format: %u", PixelFormat);
  if (!SetPixelFormat(hDC, PixelFormat, &pfd))
  {
    close();
    log(LOG_ERROR, "Can't Set The PixelFormat: %lu", GetLastError());
  }

  log(LOG_DEBUG, "Trying to create rendering context. hDC=0x%p", hDC);
  if (!(hRC = wglCreateContext(hDC)))
  {
    close();
    log(LOG_ERROR, "Can't Create A GL Rendering Context: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  log(LOG_DEBUG, "Trying to activate rendering context. hDC=0x%p, hRC=0x%p",
      hDC, hRC);
  if (!wglMakeCurrent(hDC, hRC))
  {
    close();
    log(LOG_ERROR, "Can't Activate The GL Rendering Context: %lu",
        GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  /* Set GWLP_USERDATA to this pointer */
  SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);

  ShowWindow(hWnd, SW_SHOW); /* Show The Window */
  SetForegroundWindow(hWnd); /* Slightly Higher Priority */
  SetFocus(hWnd);            /* Sets Keyboard Focus To The Window */

  if (context.init(BITS_PER_PIXEL, geometry.width, geometry.height)
      != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to initialize graphics context");
    close();
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  opened = true;

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::win_gl::window::close()
{
  yage::core::event<void> event;
  window_closed.publish(event);

  if (hRC)
  {
    /* Try to Release The DC And RC Contexts? */
    if (!wglMakeCurrent(NULL, NULL))
    {
      log(LOG_WARN, "Release Of DC And RC Failed");
    }

    /* Try to Delete The RC? */
    if (!wglDeleteContext(hRC))
    {
      log(LOG_WARN, "Release Rendering Context Failed");
    }

    hRC = NULL;
  }

  /* Try to Release The DC */
  if (hDC && !ReleaseDC(hWnd, hDC))
  {
    log(LOG_WARN, "Release Device Context Failed");
    hDC = NULL; // Set DC To NULL
  }

  /* Try to destroy The Window? */
  if (hWnd && !DestroyWindow(hWnd))
  {
    log(LOG_WARN, "Could Not Release hWnd");
    hWnd = NULL; /* Set hWnd To NULL */
  }

  /* Try to Unregister Class */
  if (!UnregisterClass("OpenGL", hInstance))
  {
    log(LOG_WARN, "Could Not Unregister Class");
    hInstance = NULL; // Set hInstance To NULL
  }

  opened = false;

  return yage::core::error::OK;
}

yage::graphics::graphics *
yage::platform::graphics::win_gl::window::get_graphics()
{
  return &context;
}
yage::core::error yage::platform::graphics::win_gl::window::update()
{
  if (!opened)
  {
    return yage::core::error::OK;
  }

  /* Read messages to make Windows happy */
  MSG msg;
  if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if (!SwapBuffers(hDC))
  {
    log(LOG_ERROR, "SwapBuffers failed: %lu. hDC=0x%p", GetLastError(), hDC);
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  return yage::core::error::OK;
}

bool yage::platform::graphics::win_gl::window::is_open()
{
  return opened;
}

static yage::core::io::key convert_key(WPARAM winkey)
{
  static const std::unordered_map<WPARAM, yage::core::io::key> keymap = {
    /* Control keys */
    { VK_RETURN, yage::core::io::key::ENTER },
    { VK_DELETE, yage::core::io::key::DEL },
    { VK_BACK, yage::core::io::key::BACKSPACE },
    { VK_ESCAPE, yage::core::io::key::ESCAPE },
    { VK_TAB, yage::core::io::key::TAB },
    { VK_SPACE, yage::core::io::key::SPACE },
    { VK_SNAPSHOT, yage::core::io::key::PAUSE },
    { VK_PAUSE, yage::core::io::key::SYS_REQ },
    { VK_SCROLL, yage::core::io::key::SCROLL_LOCK },
    { VK_HOME, yage::core::io::key::HOME },
    { VK_LEFT, yage::core::io::key::LEFT },
    { VK_UP, yage::core::io::key::UP },
    { VK_RIGHT, yage::core::io::key::RIGHT },
    { VK_DOWN, yage::core::io::key::DOWN },
    { VK_PRIOR, yage::core::io::key::PAGE_UP },
    { VK_NEXT, yage::core::io::key::PAGE_DOWN },
    { VK_END, yage::core::io::key::END },

    /* Special keys */
    { VK_OEM_1, yage::core::io::key::SK1 },
    { VK_OEM_2, yage::core::io::key::SK2 },
    { VK_OEM_102, yage::core::io::key::SK3 },
    { VK_OEM_4, yage::core::io::key::SK4 },
    { VK_OEM_5, yage::core::io::key::SK3 },
    { VK_OEM_6, yage::core::io::key::SK5 },
    { VK_OEM_7, yage::core::io::key::SK6 },
    { VK_OEM_3, yage::core::io::key::SK7 },
    { VK_OEM_COMMA, yage::core::io::key::SK8 },
    { VK_OEM_PERIOD, yage::core::io::key::SK9 },
    { VK_OEM_MINUS, yage::core::io::key::SK10 },
    { VK_OEM_PLUS, yage::core::io::key::SK11 },

    /* Letters */
    { 'A', yage::core::io::key::A },
    { 'B', yage::core::io::key::B },
    { 'C', yage::core::io::key::C },
    { 'D', yage::core::io::key::D },
    { 'E', yage::core::io::key::E },
    { 'F', yage::core::io::key::F },
    { 'G', yage::core::io::key::G },
    { 'H', yage::core::io::key::H },
    { 'I', yage::core::io::key::I },
    { 'J', yage::core::io::key::J },
    { 'K', yage::core::io::key::K },
    { 'L', yage::core::io::key::L },
    { 'M', yage::core::io::key::M },
    { 'N', yage::core::io::key::N },
    { 'O', yage::core::io::key::O },
    { 'P', yage::core::io::key::P },
    { 'Q', yage::core::io::key::Q },
    { 'R', yage::core::io::key::R },
    { 'S', yage::core::io::key::S },
    { 'T', yage::core::io::key::T },
    { 'U', yage::core::io::key::U },
    { 'V', yage::core::io::key::V },
    { 'W', yage::core::io::key::W },
    { 'X', yage::core::io::key::X },
    { 'Y', yage::core::io::key::Y },
    { 'Z', yage::core::io::key::Z },

    /* Numbers */
    { '0', yage::core::io::key::K0 },
    { '1', yage::core::io::key::K1 },
    { '2', yage::core::io::key::K2 },
    { '3', yage::core::io::key::K3 },
    { '4', yage::core::io::key::K4 },
    { '5', yage::core::io::key::K5 },
    { '6', yage::core::io::key::K6 },
    { '7', yage::core::io::key::K7 },
    { '8', yage::core::io::key::K8 },
    { '9', yage::core::io::key::K9 },

    /* Misc functions */
    { VK_SELECT, yage::core::io::key::SELECT },
    { VK_PRINT, yage::core::io::key::PRINT },
    { VK_EXECUTE, yage::core::io::key::EXECUTE },
    { VK_INSERT, yage::core::io::key::INSERT },
    { VK_MENU, yage::core::io::key::MENU },
    { VK_CANCEL, yage::core::io::key::CANCEL },
    { VK_HELP, yage::core::io::key::HELP },
    { VK_CANCEL, yage::core::io::key::BREAK },
    { VK_NUMLOCK, yage::core::io::key::NUM_LOCK },
    { VK_CLEAR, yage::core::io::key::CLEAR },

    /* Keypad functions */
    { VK_MULTIPLY, yage::core::io::key::KP_MULTIPLY },
    { VK_ADD, yage::core::io::key::KP_ADD },
    { VK_SEPARATOR, yage::core::io::key::KP_SEPARATOR },
    { VK_SUBTRACT, yage::core::io::key::KP_SUBTRACT },
    { VK_DECIMAL, yage::core::io::key::KP_DECIMAL },
    { VK_DIVIDE, yage::core::io::key::KP_DIVIDE },
    { VK_NUMPAD0, yage::core::io::key::KP_0 },
    { VK_NUMPAD1, yage::core::io::key::KP_1 },
    { VK_NUMPAD2, yage::core::io::key::KP_2 },
    { VK_NUMPAD3, yage::core::io::key::KP_3 },
    { VK_NUMPAD4, yage::core::io::key::KP_4 },
    { VK_NUMPAD5, yage::core::io::key::KP_5 },
    { VK_NUMPAD6, yage::core::io::key::KP_6 },
    { VK_NUMPAD7, yage::core::io::key::KP_7 },
    { VK_NUMPAD8, yage::core::io::key::KP_8 },
    { VK_NUMPAD9, yage::core::io::key::KP_9 },

    /* Function keys */
    { VK_F1, yage::core::io::key::F1 },
    { VK_F2, yage::core::io::key::F2 },
    { VK_F3, yage::core::io::key::F3 },
    { VK_F4, yage::core::io::key::F4 },
    { VK_F5, yage::core::io::key::F5 },
    { VK_F6, yage::core::io::key::F6 },
    { VK_F7, yage::core::io::key::F7 },
    { VK_F8, yage::core::io::key::F8 },
    { VK_F9, yage::core::io::key::F9 },
    { VK_F10, yage::core::io::key::F10 },
    { VK_F11, yage::core::io::key::F11 },
    { VK_F12, yage::core::io::key::F12 },

    /* Key state modifier keys */
    { VK_SHIFT, yage::core::io::key::SHIFT_L },
    { VK_LSHIFT, yage::core::io::key::SHIFT_L },
    { VK_RSHIFT, yage::core::io::key::SHIFT_R },
    { VK_CONTROL, yage::core::io::key::CONTROL_L },
    { VK_LCONTROL, yage::core::io::key::CONTROL_L },
    { VK_RCONTROL, yage::core::io::key::CONTROL_R },
    { VK_CAPITAL, yage::core::io::key::CAPS_LOCK },
    { VK_LWIN, yage::core::io::key::META_L },
    { VK_RWIN, yage::core::io::key::META_R },
    { VK_LMENU, yage::core::io::key::ALT_L },
    { VK_RMENU, yage::core::io::key::ALT_R },
  };

#if USE_DEBUG
  if (keymap.find(winkey) == keymap.end())
  {
    log(LOG_WARN, "Key not found: 0x%llx (%c)", winkey, (char)winkey);
  }
#endif

  return keymap.at(winkey);
}

static yage::core::io::mouse convert_button(WPARAM winbutton)
{
  static const std::unordered_map<WPARAM, yage::core::io::mouse> buttonmap
        = { { WM_LBUTTONDOWN, yage::core::io::mouse::LEFT },
            { WM_RBUTTONDOWN, yage::core::io::mouse::RIGHT },
            { WM_MBUTTONDOWN, yage::core::io::mouse::MIDDLE },
            { WM_XBUTTONDOWN, yage::core::io::mouse::X1 },
            { WM_LBUTTONUP, yage::core::io::mouse::LEFT },
            { WM_RBUTTONUP, yage::core::io::mouse::RIGHT },
            { WM_MBUTTONUP, yage::core::io::mouse::MIDDLE },
            { WM_XBUTTONUP, yage::core::io::mouse::X1 } };

#if USE_DEBUG
  if (buttonmap.find(winbutton) == buttonmap.end())
  {
    log(LOG_WARN, "Button not found: 0x%llx", winbutton);
  }
#endif
  return buttonmap.at(winbutton);
}

LRESULT CALLBACK yage::platform::graphics::win_gl::window::wnd_proc(
      HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  /* Get The Window Context */
  auto * win = (yage::platform::graphics::win_gl::window *)GetWindowLongPtr(
        hWnd, GWLP_USERDATA);

  /* Handle key presses */
  switch (uMsg)
  {
    case WM_KEYDOWN:
    {
      win->key_pressed.publish(convert_key(wParam));
      break;
    }

    case WM_KEYUP:
    {
      win->key_released.publish(convert_key(wParam));
      break;
    }

    case WM_SYSCHAR:
    case WM_CHAR:
    {
      auto character = (wchar_t)wParam;
      win->char_entered.publish(character);
      break;
    }

      /* Handle scrolling */
    case WM_MOUSEWHEEL:
    {
      auto amount = GET_WHEEL_DELTA_WPARAM(wParam);
      struct yage::graphics::mouse_scroll_data scroll_data
            = { 0, (float)amount / WHEEL_DELTA };

      win->mouse_scrolled.publish(scroll_data);
      break;
    }

    /* Handle mouse clicks */
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
    {
      struct yage::graphics::mouse_click_data click_data
            = { convert_button(uMsg), 0, 0 };

      POINT p;
      if (GetCursorPos(&p))
      {
        ScreenToClient(hWnd, &p);
        click_data.x = (float)p.x;
        click_data.y = (float)p.y;
      }

      win->mouse_pressed.publish(click_data);
      break;
    }

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    {
      struct yage::graphics::mouse_click_data click_data
            = { convert_button(uMsg), 0, 0 };

      POINT p;
      if (GetCursorPos(&p))
      {
        ScreenToClient(hWnd, &p);
        click_data.x = (float)p.x;
        click_data.y = (float)p.y;
      }

      win->mouse_released.publish(click_data);
      break;
    }

    case WM_MOUSEMOVE:
    {
      struct yage::graphics::mouse_moved_data click_data = { 0, 0 };

      POINT p;
      if (GetCursorPos(&p))
      {
        ScreenToClient(hWnd, &p);
        click_data.x = (float)p.x;
        click_data.y = (float)p.y;
      }

      win->mouse_moved.publish(click_data);
      break;
    }

    case WM_CLOSE:
    {
      win->close();
    }

    default:
    {
      break;
    }
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
