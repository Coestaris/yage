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

#include "core/log.hpp"
#include "platform/graphics/gl/graphics.hpp"
#include "platform/graphics/gl/glplatform.hpp"
#include "platform/graphics/gl/shader_compiler.hpp"
#include "platform/graphics/gl/shader.hpp"
#include "texture_compiler.hpp"
#include "texture.hpp"

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

  yage::core::log_level level;
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
yage::graphics::shader_compiler *
yage::platform::graphics::gl::graphics::create_shader_compiler()
{
  return new yage::platform::graphics::gl::shader_compiler(this);
}

yage::core::error
yage::platform::graphics::gl::graphics::destroy_shader_compiler(
      yage::graphics::shader_compiler * compiler)
{
  delete compiler;
  return yage::core::error::OK;
}

yage::graphics::shader * yage::platform::graphics::gl::graphics::create_shader()
{
  return new yage::platform::graphics::gl::shader(this);
}

yage::core::error yage::platform::graphics::gl::graphics::destroy_shader(
      yage::graphics::shader * shader)
{
  delete shader;
  return yage::core::OK;
}

yage::graphics::texture_compiler *
yage::platform::graphics::gl::graphics::create_texture_compiler()
{
  return new yage::platform::graphics::gl::texture_compiler(this);
}

yage::core::error
yage::platform::graphics::gl::graphics::destroy_texture_compiler(
      yage::graphics::texture_compiler * compiler)
{
  delete compiler;
  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::graphics::destroy_texture(
      yage::graphics::texture * texture)
{
  delete texture;
  return yage::core::error::OK;
}

void yage::platform::graphics::gl::graphics::cache_extensions()
{
  const char * ext = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

  if (!ext)
  {
    logging(LOG_ERROR, "Failed to get gl extensions");
    return;
  }

  size_t len  = strlen(ext);
  char * copy = new char[len + 1];
  strcpy(copy, ext);

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

bool yage::platform::graphics::gl::graphics::has_extension(
      const char * extension)
{
  return std::find(extensions.begin(), extensions.end(), extension)
         != extensions.end();
}

yage::core::error yage::platform::graphics::gl::graphics::init(
      int bits_per_pixel, int width, int height)
{
  logging(LOG_INFO, "gl vendor: %s", glGetString(GL_VENDOR));
  logging(LOG_INFO, "gl renderer: %s", glGetString(GL_RENDERER));
  logging(LOG_INFO, "gl version: %s", glGetString(GL_VERSION));
  logging(
        LOG_INFO, "gl shading language version: %s",
        glGetString(GL_SHADING_LANGUAGE_VERSION));

  cache_extensions();
  print_extensions();

  /* Set error callback */
#if USE_DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  if (has_extension("GL_KHR_debug"))
  {
    gl_call(
          "glDebugMessageControl", GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
          nullptr, GL_TRUE);
    gl_call("glDebugMessageCallback", GLDebugMessageCallback, nullptr);
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

  // clang-format off
  glCreateShader            = reinterpret_cast<PFNGLCREATESHADERPROC>(gl_lookup("glCreateShader", true));
  glShaderSource            = reinterpret_cast<PFNGLSHADERSOURCEPROC>(gl_lookup("glShaderSource", true));
  glCompileShader           = reinterpret_cast<PFNGLCOMPILESHADERPROC>(gl_lookup("glCompileShader", true));
  glGetShaderiv             = reinterpret_cast<PFNGLGETSHADERIVPROC>(gl_lookup("glGetShaderiv", true));
  glGetShaderInfoLog        = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(gl_lookup("glGetShaderInfoLog", true));
  glDeleteShader            = reinterpret_cast<PFNGLDELETESHADERPROC>(gl_lookup("glDeleteShader", true));
  glCreateProgram           = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(gl_lookup("glCreateProgram", true));
  glAttachShader            = reinterpret_cast<PFNGLATTACHSHADERPROC>(gl_lookup("glAttachShader", true));
  glLinkProgram             = reinterpret_cast<PFNGLLINKPROGRAMPROC>(gl_lookup("glLinkProgram", true));
  glGetProgramiv            = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(gl_lookup("glGetProgramiv", true));
  glGetProgramInfoLog       = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(gl_lookup("glGetProgramInfoLog", true));
  glUseProgram              = reinterpret_cast<PFNGLUSEPROGRAMPROC>(gl_lookup("glUseProgram", true));
  glDeleteProgram           = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(gl_lookup("glDeleteProgram", true));
  glGetProgramBinary        = reinterpret_cast<PFNGLGETPROGRAMBINARYPROC>(gl_lookup("glGetProgramBinary", true));
  glGenVertexArrays         = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(gl_lookup("glGenVertexArrays", true));
  glBufferData              = reinterpret_cast<PFNGLBUFFERDATAPROC>(gl_lookup("glBufferData", true));
  glGenBuffers              = reinterpret_cast<PFNGLGENBUFFERSPROC>(gl_lookup("glGenBuffers", true));
  glBindVertexArray         = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(gl_lookup("glBindVertexArray", true));
  glBindBuffer              = reinterpret_cast<PFNGLBINDBUFFERPROC>(gl_lookup("glBindBuffer", true));
  glVertexAttribPointer     = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(gl_lookup("glVertexAttribPointer", true));
  glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(gl_lookup("glEnableVertexAttribArray", true));
  glDrawArrays              = reinterpret_cast<PFNGLDRAWARRAYSEXTPROC>(gl_lookup("glDrawArrays", true));
  glShaderBinary            = reinterpret_cast<PFNGLSHADERBINARYPROC>(gl_lookup("glShaderBinary", true));
  glProgramBinary           = reinterpret_cast<PFNGLPROGRAMBINARYPROC>(gl_lookup("glProgramBinary", true));
  // clang-format on

  glShadeModel(GL_SMOOTH); // Enable Smooth Shading
  glEnable(GL_DEPTH_TEST); // Enables Depth Testing
  glDepthFunc(GL_LEQUAL);  // The Type Of Depth Testing To Do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  /* Setup orthographic projection */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, height, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);

  return yage::core::error::OK;
}
yage::core::error yage::platform::graphics::gl::graphics::clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::graphics::flush()
{
  glFinish();

  return yage::core::error::OK;
}

yage::core::error
yage::platform::graphics::gl::graphics::draw_line(glm::vec2 a, glm::vec2 b)
{
  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::graphics::draw_rect(
      glm::vec2 position, glm::vec2 dim)
{
  /* Draw a rectangle */
  glBegin(GL_QUADS);                                  // Start Drawing A Quad
  glColor3f(1.0f, 0.0f, 0.0f);                        // Set The Color To Red
  glVertex2f(position.x, position.y);                 // Top Left
  glColor3f(0.0f, 1.0f, 0.0f);                        // Set The Color To Green
  glVertex2f(position.x + dim.x, position.y);         // Top Right
  glColor3f(0.0f, 0.0f, 1.0f);                        // Set The Color To Blue
  glVertex2f(position.x + dim.x, position.y + dim.y); // Bottom Right
  glColor3f(1.0f, 1.0f, 0.0f);                        // Set The Color To Yellow
  glVertex2f(position.x, position.y + dim.y);         // Bottom Left
  glEnd(); // Finished Drawing The Quad
  return yage::core::error::OK;
}
