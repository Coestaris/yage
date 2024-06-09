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
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Declaration For WndProc

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

LRESULT CALLBACK WndProc(
      HWND   hWnd,   // Handle For This Window
      UINT   uMsg,   // Message For This Window
      WPARAM wParam, // Additional Message Information
      LPARAM lParam) // Additional Message Information
{
  // Pass All Unhandled Messages To DefWindowProc
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
  wc.lpfnWndProc   = (WNDPROC)WndProc;
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
  /* Read messages to make Windows happy */
  MSG msg;
  if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
  {
    if (msg.message == WM_QUIT)
    {
      close();
      return yage::core::error::OK;
    }

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
