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

#include <cinttypes>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstring>

#include "yagecore/log.hpp"
#include "yagecore/gl/graphics.hpp"
#include "yagecore/gl/glplatform.hpp"
#include "yagecore/gl/shader.hpp"
#include "yagecore/gl/texture.hpp"
#include "yagecore/gl/node.hpp"

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
  const char * _source;
  const char * _type;
  const char * _severity;

  switch (source)
  {
    case GL_DEBUG_SOURCE_API:
      _source = "API";
      break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      _source = "WINDOW SYSTEM";
      break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      _source = "SHADER COMPILER";
      break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
      _source = "THIRD PARTY";
      break;

    case GL_DEBUG_SOURCE_APPLICATION:
      _source = "APPLICATION";
      break;

    case GL_DEBUG_SOURCE_OTHER:
      _source = "UNKNOWN";
      break;

    default:
      _source = "UNKNOWN";
      break;
  }

  switch (type)
  {
    case GL_DEBUG_TYPE_ERROR:
      _type = "ERROR";
      break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      _type = "DEPRECATED BEHAVIOR";
      break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      _type = "UDEFINED BEHAVIOR";
      break;

    case GL_DEBUG_TYPE_PORTABILITY:
      _type = "PORTABILITY";
      break;

    case GL_DEBUG_TYPE_PERFORMANCE:
      _type = "PERFORMANCE";
      break;

    case GL_DEBUG_TYPE_OTHER:
      _type = "OTHER";
      break;

    case GL_DEBUG_TYPE_MARKER:
      _type = "MARKER";
      break;

    default:
      _type = "UNKNOWN";
      break;
  }

  yc::log_level level;
  switch (severity)
  {
    case GL_DEBUG_SEVERITY_HIGH:
      _severity = "HIGH";
      level     = LOG_FATAL;
      break;

    case GL_DEBUG_SEVERITY_MEDIUM:
      _severity = "MEDIUM";
      level     = LOG_ERROR;
      break;

    case GL_DEBUG_SEVERITY_LOW:
      _severity = "LOW";
      level     = LOG_WARN;
      break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
      _severity = "NOTIFICATION";
      level     = LOG_DEBUG;
      break;

    default:
      _severity = "UNKNOWN";
      level     = LOG_WARN;
      break;
  }

  logging(
        level, "GL Message. ID=%d, Type=%s, Severity=%s, Source=%s. %s", id,
        _type, _severity, _source, msg);
}
#endif

/* Shared functions ========================================================= */
std::shared_ptr<yc::graphics::shader> yc::gl::graphics::create_shader(
      std::string                             name,
      std::shared_ptr<yc::app::asset_cacher>  cacher,
      std::shared_ptr<yc::graphics::graphics> graphics)
{
  return std::make_shared<yc::gl::shader>(std::move(name), cacher, graphics);
}

std::shared_ptr<yc::graphics::texture> yc::gl::graphics::create_texture(
      std::string                             name,
      std::shared_ptr<yc::app::asset_cacher>  cacher,
      std::shared_ptr<yc::graphics::graphics> graphics)
{
  return std::make_shared<yc::gl::texture>(std::move(name), cacher, graphics);
}

std::shared_ptr<yc::graphics::node> yc::gl::graphics::create_node(
      std::string                             name,
      std::shared_ptr<yc::app::asset_cacher>  cacher,
      std::shared_ptr<yc::graphics::graphics> graphics)
{
  return std::make_shared<yc::gl::node>(std::move(name), cacher, graphics);
}

void yc::gl::graphics::cache_extensions()
{
  const char * ext = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

  if (!ext)
  {
    logging(LOG_ERROR, "Failed to get gl extensions");
    return;
  }

  std::string copy(ext);

  std::istringstream iss(copy);
  std::string        token;

  while (std::getline(iss, token, ' '))
  {
    extensions.push_back(token);
  }

  std::sort(extensions.begin(), extensions.end());
}

void yc::gl::graphics::print_extensions()
{
  logging(LOG_INFO, "gl extensions: %" PRIuPTR, extensions.size());
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
    logging(
          LOG_DEBUG, " [%4" PRIuPTR "-%4" PRIuPTR "] %s", i, i + MAX_EXTENSIONS,
          buf.str().c_str());
  }
}

bool yc::gl::graphics::has_extension(const std::string & extension)
{
  return std::find(extensions.begin(), extensions.end(), extension)
         != extensions.end();
}

yc::error yc::gl::graphics::init(int bits_per_pixel, int width, int height)
{
  logging(LOG_INFO, "gl vendor: %s", glGetString(GL_VENDOR));
  logging(LOG_INFO, "gl renderer: %s", glGetString(GL_RENDERER));
  logging(LOG_INFO, "gl version: %s", glGetString(GL_VERSION));
  logging(
        LOG_INFO, "gl shading language version: %s",
        glGetString(GL_SHADING_LANGUAGE_VERSION));

  gl_update_cache();

  cache_extensions();
  print_extensions();

  /* Set error callback */
#if USE_DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  if (has_extension("GL_KHR_debug"))
  {
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    glDebugMessageCallback(GLDebugMessageCallback, nullptr);
  }
  else
  {
    logging(
          LOG_WARN, "GL_KHR_debug not found. Some debug features may not work");
  }
#endif

  /* get binary format */
  GLint length;
  glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &length);
  std::vector<GLint> formats(length);
  glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, formats.data());
  logging(LOG_INFO, "Detected %d binary formats", length);
  for (auto & format : formats)
  {
    logging(LOG_INFO, "  Binary format: 0x%x", format);
  }

  glShadeModel(GL_SMOOTH); // Enable Smooth Shading
  glEnable(GL_DEPTH_TEST); // Enables Depth Testing
  glDepthFunc(GL_LEQUAL);  // The Type Of Depth Testing To Do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  return yc::error::OK;
}
yc::error yc::gl::graphics::clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  return yc::error::OK;
}

yc::error yc::gl::graphics::flush()
{
  glFinish();

  return yc::error::OK;
}
