/** ========================================================================
 *
 * @file glplaftorm.cpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#if YAGE_PLATFORM_X11
#include <GL/glx.h>
#elif __WIN32__ || _WIN32
#include <GL/glext.h>
#endif

#include <stdexcept>
#include <unordered_map>

#include "platform/graphics/gl/glplatform.hpp"
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
static std::unordered_map<std::string, void *> cache;

/* Shared functions ========================================================= */
void * yage::platform::graphics::gl::gl_lookup(const char * func, bool required)
{
  if (cache.find(func) != cache.end())
  {
    return cache[func];
  }

#if WIN32
  auto proc = wglGetProcAddress(func);
#else
  auto proc = reinterpret_cast<void *>(
        glXGetProcAddress(reinterpret_cast<const GLubyte *>(func)));
#endif

  if (!proc)
  {
    if (required)
    {
      throw std::runtime_error("Function " + std::string(func) + " not found");
    }
    else
    {
      log(LOG_ERROR, "Function %s not found", func);
    }

    return nullptr;
  }

  cache[func] = proc;
  return proc;
}
