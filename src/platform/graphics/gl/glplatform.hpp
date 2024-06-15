/** ========================================================================
 *
 * @file glplatform.hpp
 * @date 15-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#if YAGE_PLATFORM_X11
#include <GL/glx.h>
#elif __WIN32__ || _WIN32
#include <GL/glext.h>
#endif

namespace yage::platform::graphics::gl
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes=============================================================== */
template <typename... Args>
yage::core::error g_dyn_call(const char * func, Args... args)
{
#if WIN32
  auto proc = wglGetProcAddress(func);
#else
  auto proc = glXGetProcAddress((const GLubyte *)func);
#endif
  if (proc)
  {
    auto f = (void (*)(Args...))(void *)proc;
    f(args...);
    return yage::core::error::OK;
  }
  else
  {
    log(LOG_ERROR, "Function %s not found", func);
    return yage::core::error::NOT_IMPLEMENTED;
  }
}

}
