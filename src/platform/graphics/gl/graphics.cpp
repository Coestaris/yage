/** ========================================================================
 *
 * @file graphics.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>

#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <functional>
#include <cstring>

#include "graphics.hpp"
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
#if USE_DEBUG
void APIENTRY GLDebugMessageCallback(
      GLenum         source,
      GLenum         type,
      GLuint         id,
      GLenum         severity,
      GLsizei        length,
      const GLchar * msg,
      const void *   data)
{
  char * _source;
  char * _type;
  char * _severity;

  switch (source)
  {
    case GL_DEBUG_SOURCE_API:
      _source = (char *)"API";
      break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      _source = (char *)"WINDOW SYSTEM";
      break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      _source = (char *)"SHADER COMPILER";
      break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
      _source = (char *)"THIRD PARTY";
      break;

    case GL_DEBUG_SOURCE_APPLICATION:
      _source = (char *)"APPLICATION";
      break;

    case GL_DEBUG_SOURCE_OTHER:
      _source = (char *)"UNKNOWN";
      break;

    default:
      _source = (char *)"UNKNOWN";
      break;
  }

  switch (type)
  {
    case GL_DEBUG_TYPE_ERROR:
      _type = (char *)"ERROR";
      break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      _type = (char *)"DEPRECATED BEHAVIOR";
      break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      _type = (char *)"UDEFINED BEHAVIOR";
      break;

    case GL_DEBUG_TYPE_PORTABILITY:
      _type = (char *)"PORTABILITY";
      break;

    case GL_DEBUG_TYPE_PERFORMANCE:
      _type = (char *)"PERFORMANCE";
      break;

    case GL_DEBUG_TYPE_OTHER:
      _type = (char *)"OTHER";
      break;

    case GL_DEBUG_TYPE_MARKER:
      _type = (char *)"MARKER";
      break;

    default:
      _type = (char *)"UNKNOWN";
      break;
  }

  switch (severity)
  {
    case GL_DEBUG_SEVERITY_HIGH:
      _severity = (char *)"HIGH";
      break;

    case GL_DEBUG_SEVERITY_MEDIUM:
      _severity = (char *)"MEDIUM";
      break;

    case GL_DEBUG_SEVERITY_LOW:
      _severity = (char *)"LOW";
      break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
      _severity = (char *)"NOTIFICATION";
      break;

    default:
      _severity = (char *)"UNKNOWN";
      break;
  }

  log(LOG_ERROR, "GL Error. ID=%d, Type=%s, Severity=%s, Source=%s. %s", id,
      _type, _severity, _source, msg);
}
#endif

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

/* Shared functions ========================================================= */
void yage::platform::graphics::gl::graphics::cache_extensions()
{
  const GLubyte * ext = glGetString(GL_EXTENSIONS);

  if (!ext)
  {
    log(LOG_ERROR, "Failed to get gl extensions");
    return;
  }

  size_t len  = strlen((const char *)ext);
  char * copy = new char[len + 1];
  strcpy(copy, (const char *)ext);

  char * token = strtok(copy, " ");
  while (token)
  {
    extensions.emplace_back(token);
    token = strtok(nullptr, " ");
  }

  delete[] copy;
}

void yage::platform::graphics::gl::graphics::print_extensions()
{
  log(LOG_INFO, "gl extensions: %zu", extensions.size());
#define MAX_EXTENSIONS 3
  std::stringbuf buf;
  std::ostream   os(&buf);
  for (size_t i = 0; i < extensions.size(); i += 4)
  {
    buf.str("");
    for (size_t j = 0; j < MAX_EXTENSIONS && i + j < extensions.size(); j++)
    {
      os << std::setw(40) << extensions[i + j] << " ";
    }
    log(LOG_DEBUG, " [%4zu-%4zu] %s", i, i + MAX_EXTENSIONS, buf.str().c_str());
  }
}

bool yage::platform::graphics::gl::graphics::has_extension(
      const char * extension)
{
  return std::find(extensions.begin(), extensions.end(), extension)
         != extensions.end();
}

yage::core::error yage::platform::graphics::gl::graphics::init(
      int bits_per_pixel, int width, int height)
{
  log(LOG_INFO, "gl vendor: %s", glGetString(GL_VENDOR));
  log(LOG_INFO, "gl renderer: %s", glGetString(GL_RENDERER));
  log(LOG_INFO, "gl version: %s", glGetString(GL_VERSION));
  log(LOG_INFO, "gl shading language version: %s",
      glGetString(GL_SHADING_LANGUAGE_VERSION));

  cache_extensions();
  print_extensions();

  /* Set error callback */
#if USE_DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  if (has_extension("GL_KHR_debug"))
  {
    g_dyn_call(
          "glDebugMessageControl", GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
          nullptr, GL_TRUE);
    g_dyn_call("glDebugMessageCallback", GLDebugMessageCallback, nullptr);
  }
  else
  {
    log(LOG_WARN, "GL_KHR_debug not found. Some debug features may not work");
  }
#endif

  glShadeModel(GL_SMOOTH); // Enable Smooth Shading
  glEnable(GL_DEPTH_TEST); // Enables Depth Testing
  glDepthFunc(GL_LEQUAL);  // The Type Of Depth Testing To Do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  return yage::core::error::OK;
}
yage::core::error yage::platform::graphics::gl::graphics::clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::graphics::present()
{
  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::graphics::draw_line(
      int x1, int y1, int x2, int y2)
{
  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::graphics::draw_rect(
      int x, int y, int width, int height)
{
  /* Draw a triangle */
  glBegin(GL_TRIANGLES);          // Drawing Using Triangles
  glColor3f(1.0f, 0.0f, 0.0f);    // Set The Color To Red
  glVertex3f(0.0f, 1.0f, 0.0f);   // Top
  glColor3f(0.0f, 1.0f, 0.0f);    // Set The Color To Green
  glVertex3f(-1.0f, -1.0f, 0.0f); // Bottom Left
  glColor3f(0.0f, 0.0f, 1.0f);    // Set The Color To Blue
  glVertex3f(1.0f, -1.0f, 0.0f);  // Bottom Right
  glEnd();                        // Finished Drawing The Triangle
  return yage::core::error::OK;
}
