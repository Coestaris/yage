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
yc::graphics::shader * yc::gl::graphics::create_shader(
      std::string              name,
      yc::app::asset_cacher *  cacher,
      yc::graphics::graphics * graphics)
{
  return new yc::gl::shader(std::move(name), cacher, graphics);
}

yc::graphics::texture * yc::gl::graphics::create_texture(
      std::string              name,
      yc::app::asset_cacher *  cacher,
      yc::graphics::graphics * graphics)
{
  return new yc::gl::texture(name, cacher, graphics);
}

yc::graphics::node * yc::gl::graphics::create_node(
      std::string              name,
      yc::app::asset_cacher *  cacher,
      yc::graphics::graphics * graphics)
{
  return new yc::gl::node(std::move(name), cacher, graphics);
}

yc::error yc::gl::graphics::destroy_shader(yc::graphics::shader * shader)
{
  delete shader;
  return yc::error::OK;
}

yc::error yc::gl::graphics::destroy_texture(yc::graphics::texture * texture)
{
  delete texture;
  return yc::error::OK;
}

yc::error yc::gl::graphics::destroy_node(yc::graphics::node * node)
{
  delete node;
  return yc::error::OK;
}

void yc::gl::graphics::cache_extensions()
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

bool yc::gl::graphics::has_extension(const char * extension)
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
  glActiveTexture           = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(gl_lookup("glActiveTexture", true));
  glBindTexture             = reinterpret_cast<PFNGLBINDTEXTUREEXTPROC>(gl_lookup("glBindTexture", true));
  glGenerateMipmap          = reinterpret_cast<PFNGLGENERATEMIPMAPPROC>(gl_lookup("glGenerateMipmap", true));
  glGetUniformLocation      = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(gl_lookup("glGetUniformLocation", true));
  glGetAttribLocation       = reinterpret_cast<PFNGLGETATTRIBLOCATIONPROC>(gl_lookup("glGetAttribLocation", true));
  glUniform1i               = reinterpret_cast<PFNGLUNIFORM1IPROC>(gl_lookup("glUniform1i", true));
  glUniform1f               = reinterpret_cast<PFNGLUNIFORM1FPROC>(gl_lookup("glUniform1f", true));
  glUniform2f               = reinterpret_cast<PFNGLUNIFORM2FPROC>(gl_lookup("glUniform2f", true));
  glUniform3f               = reinterpret_cast<PFNGLUNIFORM3FPROC>(gl_lookup("glUniform3f", true));
  glUniform4f               = reinterpret_cast<PFNGLUNIFORM4FPROC>(gl_lookup("glUniform4f", true));
  glUniformMatrix2fv        = reinterpret_cast<PFNGLUNIFORMMATRIX2FVPROC>(gl_lookup("glUniformMatrix2fv", true));
  glUniformMatrix3fv        = reinterpret_cast<PFNGLUNIFORMMATRIX3FVPROC>(gl_lookup("glUniformMatrix3fv", true));
  glUniformMatrix4fv        = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(gl_lookup("glUniformMatrix4fv", true));
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
