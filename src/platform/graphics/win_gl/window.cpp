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
  logging(LOG_DEBUG, "PIXELFORMATDESCRIPTOR:");
  logging(LOG_DEBUG, "    nSize = %d", pdf->nSize);
  logging(LOG_DEBUG, "    nVersion = %d", pdf->nVersion);
  logging(LOG_DEBUG, "    dwFlags = %lu", pdf->dwFlags);
  logging(LOG_DEBUG, "    iPixelType = %d", pdf->iPixelType);
  logging(LOG_DEBUG, "    cColorBits = %d", pdf->cColorBits);
  logging(LOG_DEBUG, "    cRedBits = %d", pdf->cRedBits);
  logging(LOG_DEBUG, "    cRedShift = %d", pdf->cRedShift);
  logging(LOG_DEBUG, "    cGreenBits = %d", pdf->cGreenBits);
  logging(LOG_DEBUG, "    cGreenShift = %d", pdf->cGreenShift);
  logging(LOG_DEBUG, "    cBlueBits = %d", pdf->cBlueBits);
  logging(LOG_DEBUG, "    cBlueShift = %d", pdf->cBlueShift);
  logging(LOG_DEBUG, "    cAlphaBits = %d", pdf->cAlphaBits);
  logging(LOG_DEBUG, "    cAlphaShift = %d", pdf->cAlphaShift);
  logging(LOG_DEBUG, "    cAccumBits = %d", pdf->cAccumBits);
  logging(LOG_DEBUG, "    cAccumRedBits = %d", pdf->cAccumRedBits);
  logging(LOG_DEBUG, "    cAccumGreenBits = %d", pdf->cAccumGreenBits);
  logging(LOG_DEBUG, "    cAccumBlueBits = %d", pdf->cAccumBlueBits);
  logging(LOG_DEBUG, "    cAccumAlphaBits = %d", pdf->cAccumAlphaBits);
  logging(LOG_DEBUG, "    cDepthBits = %d", pdf->cDepthBits);
  logging(LOG_DEBUG, "    cStencilBits = %d", pdf->cStencilBits);
  logging(LOG_DEBUG, "    cAuxBuffers = %d", pdf->cAuxBuffers);
  logging(LOG_DEBUG, "    iLayerType = %d", pdf->iLayerType);
  logging(LOG_DEBUG, "    bReserved = %d", pdf->bReserved);
  logging(LOG_DEBUG, "    dwLayerMask = %lu", pdf->dwLayerMask);
  logging(LOG_DEBUG, "    dwVisibleMask = %lu", pdf->dwVisibleMask);
  logging(LOG_DEBUG, "    dwDamageMask = %lu", pdf->dwDamageMask);
}

/* TODO: can be moved to common part,
 * since have a lot of common code with DirectX */
yage::core::error yage::platform::graphics::win_gl::window::open()
{
  logging(LOG_INFO, "Opening window: %s. Geometry: %d %d. Fullscreen: %s",
      settings.title.c_str(), settings.width, settings.height,
      yage::graphics::to_string(settings.win_mode).c_str());

  RECT WindowRect;
  WindowRect.left   = 0;
  WindowRect.right  = settings.width;
  WindowRect.top    = 0;
  WindowRect.bottom = settings.height;

  hInstance = GetModuleHandle(nullptr);

  logging(LOG_DEBUG, "hInstance: 0x%p", hInstance);

  WNDCLASS wc;
  /* Redraw On Size, And Own DC For Window. */
  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc   = reinterpret_cast<WNDPROC>(wnd_proc);
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hInstance;
  wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO); /* Load The Default Icon */
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW); /* Load The Arrow Pointer */
  wc.hbrBackground = NULL;     /* No Background Required For GL */
  wc.lpszMenuName  = NULL;     /* We Don't Want A Menu */
  wc.lpszClassName = "OpenGL"; /* Set The Class Name */

  logging(LOG_DEBUG, "Trying to register window class");
  if (!RegisterClass(&wc))
  {
    logging(LOG_ERROR, "Failed To Register The Window Class: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  GLuint PixelFormat; /* Holds The Results After Searching For A Match */
  DWORD  dwExStyle;   /* Window Extended Style */
  DWORD  dwStyle;     /* Window Style */

  if (settings.win_mode == yage::graphics::settings::window_mode::FULLSCREEN)
  {
    DEVMODE dmScreenSettings;
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = settings.width;
    dmScreenSettings.dmPelsHeight = settings.height;
    dmScreenSettings.dmBitsPerPel = BITS_PER_PIXEL;
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    /* NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar. */
    logging(LOG_DEBUG, "Trying to change display settings");
    if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN)
        != DISP_CHANGE_SUCCESSFUL)
    {
      logging(LOG_ERROR, "The Requested Fullscreen Mode Is Not Supported: %lu",
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
    dwStyle   = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
              | WS_MAXIMIZEBOX; /* Windows Style */
  }

  logging(LOG_DEBUG,
      "Trying to adjust window to true requested size. dw=0x%lx, dwEx=0x%lx",
      dwStyle, dwExStyle);
  if (!AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle))
  {
    logging(LOG_ERROR, "Failed To Adjust Window Rect: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  logging(LOG_DEBUG, "Trying to create window");
  if (!(hWnd = CreateWindowEx(
              dwExStyle,              /* Extended Style For The Window */
              "OpenGL",               /* Class Name */
              settings.title.c_str(), /* Window Title */
              dwStyle |               /* Defined Window Style */
                    WS_CLIPSIBLINGS | /* Required Window Style */
                    WS_CLIPCHILDREN,  /* Required Window Style */
              0,                      /* Window Position */
              0,                      /* Window Position */
              WindowRect.right - WindowRect.left, /* Calculate Window Width */
              WindowRect.bottom - WindowRect.top, /* Calculate Window Height */

              nullptr,   /* No Parent Window */
              nullptr,   /* No Menu */
              hInstance, /* Instance */
              nullptr    /* Don't Pass Anything To WM_CREATE */
              )))
  {
    close();
    logging(LOG_ERROR, "Window Creation Error: %lu", GetLastError());
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

  pfd.iPixelType      = PFD_TYPE_RGBA;  /* Request An RGBA Format */
  pfd.cColorBits      = BITS_PER_PIXEL; /* Select Our Color Depth */
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

  logging(LOG_DEBUG, "Trying to get device context. hWnd=0x%p", hWnd);
  if (!(hDC = GetDC(hWnd)))
  {
    close();
    logging(LOG_ERROR, "Can't Create A GL Device Context: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  logging(LOG_DEBUG, "Trying to find a suitable pixel format. hDC=0x%p", hDC);
  if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
  {
    close();
    logging(LOG_ERROR, "Can't Find A Suitable PixelFormat: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  logging(LOG_DEBUG, "Trying to set pixel format: %u", PixelFormat);
  if (!SetPixelFormat(hDC, PixelFormat, &pfd))
  {
    close();
    logging(LOG_ERROR, "Can't Set The PixelFormat: %lu", GetLastError());
  }

  logging(LOG_DEBUG, "Trying to create rendering context. hDC=0x%p", hDC);
  if (!(hRC = wglCreateContext(hDC)))
  {
    close();
    logging(LOG_ERROR, "Can't Create A GL Rendering Context: %lu", GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  logging(LOG_DEBUG, "Trying to activate rendering context. hDC=0x%p, hRC=0x%p",
      hDC, hRC);
  if (!wglMakeCurrent(hDC, hRC))
  {
    close();
    logging(LOG_ERROR, "Can't Activate The GL Rendering Context: %lu",
        GetLastError());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  /* Set GWLP_USERDATA to this pointer */
  SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
  ShowWindow(hWnd, SW_SHOW); /* Show The Window */
  SetForegroundWindow(hWnd); /* Slightly Higher Priority */
  SetFocus(hWnd);            /* Sets Keyboard Focus To The Window */

  if (context.init(BITS_PER_PIXEL, settings.width, settings.height)
      != yage::core::error::OK)
  {
    logging(LOG_ERROR, "Failed to initialize graphics context");
    close();
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  opened = true;

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::win_gl::window::close()
{
  if (!opened)
  {
    return yage::core::error::OK;
  }

  if (hRC)
  {
    /* Try to Release The DC And RC Contexts? */
    if (!wglMakeCurrent(NULL, NULL))
    {
      logging(LOG_WARN, "Release Of DC And RC Failed");
    }

    /* Try to Delete The RC? */
    if (!wglDeleteContext(hRC))
    {
      logging(LOG_WARN, "Release Rendering Context Failed");
    }

    hRC = NULL;
  }

  /* Try to Release The DC */
  if (hDC && !ReleaseDC(hWnd, hDC))
  {
    logging(LOG_WARN, "Release Device Context Failed");
    hDC = NULL; // Set DC To NULL
  }

  /* Try to destroy The Window? */
  if (hWnd && !DestroyWindow(hWnd))
  {
    logging(LOG_WARN, "Could Not Release hWnd");
    hWnd = NULL; /* Set hWnd To NULL */
  }

  /* Try to Unregister Class */
  if (!UnregisterClass("OpenGL", hInstance))
  {
    logging(LOG_WARN, "Could Not Unregister Class");
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
    logging(LOG_ERROR, "SwapBuffers failed: %lu. hDC=0x%p", GetLastError(), hDC);
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  return yage::core::error::OK;
}

bool yage::platform::graphics::win_gl::window::is_open()
{
  return opened;
}

static yage::core::key convert_key(WPARAM winkey)
{
  static const std::unordered_map<WPARAM, yage::core::key> keymap = {
    /* Control keys */
    { VK_RETURN, yage::core::key::ENTER },
    { VK_DELETE, yage::core::key::DEL },
    { VK_BACK, yage::core::key::BACKSPACE },
    { VK_ESCAPE, yage::core::key::ESCAPE },
    { VK_TAB, yage::core::key::TAB },
    { VK_SPACE, yage::core::key::SPACE },
    { VK_SNAPSHOT, yage::core::key::PAUSE },
    { VK_PAUSE, yage::core::key::SYS_REQ },
    { VK_SCROLL, yage::core::key::SCROLL_LOCK },
    { VK_HOME, yage::core::key::HOME },
    { VK_LEFT, yage::core::key::LEFT },
    { VK_UP, yage::core::key::UP },
    { VK_RIGHT, yage::core::key::RIGHT },
    { VK_DOWN, yage::core::key::DOWN },
    { VK_PRIOR, yage::core::key::PAGE_UP },
    { VK_NEXT, yage::core::key::PAGE_DOWN },
    { VK_END, yage::core::key::END },

    /* Special keys */
    { VK_OEM_1, yage::core::key::SK1 },
    { VK_OEM_2, yage::core::key::SK2 },
    { VK_OEM_102, yage::core::key::SK3 },
    { VK_OEM_4, yage::core::key::SK4 },
    { VK_OEM_5, yage::core::key::SK3 },
    { VK_OEM_6, yage::core::key::SK5 },
    { VK_OEM_7, yage::core::key::SK6 },
    { VK_OEM_3, yage::core::key::SK7 },
    { VK_OEM_COMMA, yage::core::key::SK8 },
    { VK_OEM_PERIOD, yage::core::key::SK9 },
    { VK_OEM_MINUS, yage::core::key::SK10 },
    { VK_OEM_PLUS, yage::core::key::SK11 },

    /* Letters */
    { 'A', yage::core::key::A },
    { 'B', yage::core::key::B },
    { 'C', yage::core::key::C },
    { 'D', yage::core::key::D },
    { 'E', yage::core::key::E },
    { 'F', yage::core::key::F },
    { 'G', yage::core::key::G },
    { 'H', yage::core::key::H },
    { 'I', yage::core::key::I },
    { 'J', yage::core::key::J },
    { 'K', yage::core::key::K },
    { 'L', yage::core::key::L },
    { 'M', yage::core::key::M },
    { 'N', yage::core::key::N },
    { 'O', yage::core::key::O },
    { 'P', yage::core::key::P },
    { 'Q', yage::core::key::Q },
    { 'R', yage::core::key::R },
    { 'S', yage::core::key::S },
    { 'T', yage::core::key::T },
    { 'U', yage::core::key::U },
    { 'V', yage::core::key::V },
    { 'W', yage::core::key::W },
    { 'X', yage::core::key::X },
    { 'Y', yage::core::key::Y },
    { 'Z', yage::core::key::Z },

    /* Numbers */
    { '0', yage::core::key::K0 },
    { '1', yage::core::key::K1 },
    { '2', yage::core::key::K2 },
    { '3', yage::core::key::K3 },
    { '4', yage::core::key::K4 },
    { '5', yage::core::key::K5 },
    { '6', yage::core::key::K6 },
    { '7', yage::core::key::K7 },
    { '8', yage::core::key::K8 },
    { '9', yage::core::key::K9 },

    /* Misc functions */
    { VK_SELECT, yage::core::key::SELECT },
    { VK_PRINT, yage::core::key::PRINT },
    { VK_EXECUTE, yage::core::key::EXECUTE },
    { VK_INSERT, yage::core::key::INSERT },
    { VK_MENU, yage::core::key::MENU },
    { VK_CANCEL, yage::core::key::CANCEL },
    { VK_HELP, yage::core::key::HELP },
    { VK_CANCEL, yage::core::key::BREAK },
    { VK_NUMLOCK, yage::core::key::NUM_LOCK },
    { VK_CLEAR, yage::core::key::CLEAR },

    /* Keypad functions */
    { VK_MULTIPLY, yage::core::key::KP_MULTIPLY },
    { VK_ADD, yage::core::key::KP_ADD },
    { VK_SEPARATOR, yage::core::key::KP_SEPARATOR },
    { VK_SUBTRACT, yage::core::key::KP_SUBTRACT },
    { VK_DECIMAL, yage::core::key::KP_DECIMAL },
    { VK_DIVIDE, yage::core::key::KP_DIVIDE },
    { VK_NUMPAD0, yage::core::key::KP_0 },
    { VK_NUMPAD1, yage::core::key::KP_1 },
    { VK_NUMPAD2, yage::core::key::KP_2 },
    { VK_NUMPAD3, yage::core::key::KP_3 },
    { VK_NUMPAD4, yage::core::key::KP_4 },
    { VK_NUMPAD5, yage::core::key::KP_5 },
    { VK_NUMPAD6, yage::core::key::KP_6 },
    { VK_NUMPAD7, yage::core::key::KP_7 },
    { VK_NUMPAD8, yage::core::key::KP_8 },
    { VK_NUMPAD9, yage::core::key::KP_9 },

    /* Function keys */
    { VK_F1, yage::core::key::F1 },
    { VK_F2, yage::core::key::F2 },
    { VK_F3, yage::core::key::F3 },
    { VK_F4, yage::core::key::F4 },
    { VK_F5, yage::core::key::F5 },
    { VK_F6, yage::core::key::F6 },
    { VK_F7, yage::core::key::F7 },
    { VK_F8, yage::core::key::F8 },
    { VK_F9, yage::core::key::F9 },
    { VK_F10, yage::core::key::F10 },
    { VK_F11, yage::core::key::F11 },
    { VK_F12, yage::core::key::F12 },

    /* Key state modifier keys */
    { VK_SHIFT, yage::core::key::SHIFT_L },
    { VK_LSHIFT, yage::core::key::SHIFT_L },
    { VK_RSHIFT, yage::core::key::SHIFT_R },
    { VK_CONTROL, yage::core::key::CONTROL_L },
    { VK_LCONTROL, yage::core::key::CONTROL_L },
    { VK_RCONTROL, yage::core::key::CONTROL_R },
    { VK_CAPITAL, yage::core::key::CAPS_LOCK },
    { VK_LWIN, yage::core::key::META_L },
    { VK_RWIN, yage::core::key::META_R },
    { VK_LMENU, yage::core::key::ALT_L },
    { VK_RMENU, yage::core::key::ALT_R },
  };

#if USE_DEBUG
  if (keymap.find(winkey) == keymap.end())
  {
    logging(LOG_WARN, "Key not found: 0x%llx (%c)", winkey,
        static_cast<char>(winkey));
  }
#endif

  return keymap.at(winkey);
}

static yage::core::mouse convert_button(WPARAM winbutton)
{
  static const std::unordered_map<WPARAM, yage::core::mouse> buttonmap
        = { { WM_LBUTTONDOWN, yage::core::mouse::LEFT },
            { WM_RBUTTONDOWN, yage::core::mouse::RIGHT },
            { WM_MBUTTONDOWN, yage::core::mouse::MIDDLE },
            { WM_XBUTTONDOWN, yage::core::mouse::X1 },
            { WM_LBUTTONUP, yage::core::mouse::LEFT },
            { WM_RBUTTONUP, yage::core::mouse::RIGHT },
            { WM_MBUTTONUP, yage::core::mouse::MIDDLE },
            { WM_XBUTTONUP, yage::core::mouse::X1 } };

#if USE_DEBUG
  if (buttonmap.find(winbutton) == buttonmap.end())
  {
    logging(LOG_WARN, "Button not found: 0x%llx", winbutton);
  }
#endif
  return buttonmap.at(winbutton);
}

LRESULT CALLBACK yage::platform::graphics::win_gl::window::wnd_proc(
      HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  /* Get The Window Context */
  auto * win = reinterpret_cast<yage::platform::graphics::win_gl::window *>(
        GetWindowLongPtr(hWnd, GWLP_USERDATA));

  /* Handle key presses */
  switch (uMsg)
  {
    case WM_KEYDOWN:
    {
      win->input.key_pressed.push(convert_key(wParam));
      break;
    }

    case WM_KEYUP:
    {
      win->input.key_released.push(convert_key(wParam));
      break;
    }

    case WM_SYSCHAR:
    case WM_CHAR:
    {
      win->input.char_entered.push(static_cast<wchar_t>(wParam));
      break;
    }

      /* Handle scrolling */
    case WM_MOUSEWHEEL:
    {
      yage::graphics::mouse_scroll_data scroll_data
            = { 0, static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam))
                         / WHEEL_DELTA };
      win->input.mouse_scrolled.push(scroll_data);
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
        click_data.x = static_cast<float>(p.x);
        click_data.y = static_cast<float>(p.y);
      }

      win->input.mouse_pressed.push(click_data);
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
        click_data.x = static_cast<float>(p.x);
        click_data.y = static_cast<float>(p.y);
      }

      win->input.mouse_released.push(click_data);
      break;
    }

    case WM_MOUSEMOVE:
    {
      struct yage::graphics::mouse_moved_data click_data = { 0, 0 };

      POINT p;
      if (GetCursorPos(&p))
      {
        ScreenToClient(hWnd, &p);
        click_data.x = static_cast<float>(p.x);
        click_data.y = static_cast<float>(p.y);
      }

      win->input.mouse_moved.push(click_data);
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
