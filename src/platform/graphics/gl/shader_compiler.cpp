/** ========================================================================
 *
 * @file shader_compiler.cpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <GL/gl.h>
#include <GL/glext.h>

#include "platform/graphics/gl/shader_compiler.hpp"
#include "core/log.hpp"
#include "glplatform.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::platform::graphics::gl::shader_compiler::shader_compiler()
{
  // clang-format off
  glCreateShader  = (PFNGLCREATESHADERPROC)gl_lookup("glCreateShader", true);
  glShaderSource  = (PFNGLSHADERSOURCEPROC)gl_lookup("glShaderSource", true);
  glCompileShader = (PFNGLCOMPILESHADERPROC)gl_lookup("glCompileShader", true);
  glGetShaderiv   = (PFNGLGETSHADERIVPROC)gl_lookup("glGetShaderiv", true);
  glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)gl_lookup("glGetShaderInfoLog", true);
  glDeleteShader  = (PFNGLDELETESHADERPROC)gl_lookup("glDeleteShader", true);
  glCreateProgram = (PFNGLCREATEPROGRAMPROC)gl_lookup("glCreateProgram", true);
  glAttachShader  = (PFNGLATTACHSHADERPROC)gl_lookup("glAttachShader", true);
  glLinkProgram   = (PFNGLLINKPROGRAMPROC)gl_lookup("glLinkProgram", true);
  glGetProgramiv  = (PFNGLGETPROGRAMIVPROC)gl_lookup("glGetProgramiv", true);
  glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)gl_lookup("glGetProgramInfoLog", true);
  glUseProgram    = (PFNGLUSEPROGRAMPROC)gl_lookup("glUseProgram", true);
  glDeleteProgram = (PFNGLDELETEPROGRAMPROC)gl_lookup("glDeleteProgram", true);
  glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)gl_lookup("glGetProgramBinary", true);
  // clang-format on
}

void yage::platform::graphics::gl::shader_compiler::clear()
{
  sources.clear();
  binary.clear();
}

yage::core::error yage::platform::graphics::gl::shader_compiler::add_shader(
      std::string source, yage::graphics::shader_type type)
{
  log(LOG_DEBUG, "Adding shader of type %d", type);

  if (sources.find(type) != sources.end())
  {
    log(LOG_ERROR, "Shader of type %d already exists", type);
    return yage::core::error::ALREADY_EXISTS;
  }

  sources[type] = source;

  return yage::core::error::OK;
}

std::string yage::platform::graphics::gl::shader_compiler::get_hash()
{
  if (sources.empty())
  {
    log(LOG_WARN, "No shaders to hash");
    return {};
  }

  /* we need to find a better hash function */
  std::hash<std::string> hasher;
  std::string            hash;

  for (auto & [type, source] : sources)
  {
    hash += std::to_string(type) + source;
  }

  return std::to_string(hasher(hash));
}

yage::core::error yage::platform::graphics::gl::shader_compiler::compile(
      std::vector<yage::graphics::shader_compiler::compile_message> &
            out_messages)
{
  if (sources.empty())
  {
    log(LOG_ERROR, "No shaders to compile");
    return yage::core::error::EMPTY;
  }

  auto program = glCreateProgram();

  for (auto & [type, source] : sources)
  {
    log(LOG_DEBUG, "Compiling shader of type %d", type);
    GLuint gl_type;
    switch (type)
    {
      case yage::graphics::shader_type::VERTEX_SHADER:
        gl_type = GL_VERTEX_SHADER;
        break;
      case yage::graphics::shader_type::FRAGMENT_SHADER:
        gl_type = GL_FRAGMENT_SHADER;
        break;
      default:
        log(LOG_ERROR, "Unknown shader type %d", type);
        return {};
    }

    GLuint shader = glCreateShader(gl_type);
    if (!shader)
    {
      log(LOG_ERROR, "Failed to create shader");
      return yage::core::error::EXTERNAL_API_ERROR;
    }

    const char * c_source = source.c_str();
    glShaderSource(shader, 1, &c_source, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::string log;
    log.resize(length);
    glGetShaderInfoLog(shader, length, nullptr, log.data());

    /* Separate messages by new line */
    size_t start = 0;
    size_t end   = log.find('\n');
    while (end != std::string::npos)
    {
      std::string message = log.substr(start, end - start);
      if (!message.empty())
      {
        if (message.find("ERROR") != std::string::npos
            || message.find("error") != std::string::npos)
        {
          out_messages.push_back({ message, 0,
                                   yage::graphics::shader_compiler::
                                         compile_message::TYPE_ERROR });
        }
        else
        {
          out_messages.push_back({ message, 0,
                                   yage::graphics::shader_compiler::
                                         compile_message::TYPE_INFO });
        }
      }

      start = end + 1;
      end   = log.find('\n', start);
    }

    if (status == GL_FALSE)
    {
      return yage::core::error::EXTERNAL_API_ERROR;
    }

    glAttachShader(program, shader);
    glDeleteShader(shader);
  }

  glLinkProgram(program);

  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);

  GLint length;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
  std::string log;
  log.resize(length);
  glGetProgramInfoLog(program, length, nullptr, log.data());

  /* Separate messages by new line */
  size_t start = 0;
  size_t end   = log.find('\n');
  while (end != std::string::npos)
  {
    std::string message = log.substr(start, end - start);
    if (!message.empty())
    {
      if (message.find("ERROR") != std::string::npos
          || message.find("error") != std::string::npos)
      {
        out_messages.push_back(
              { message, 0,
                yage::graphics::shader_compiler::compile_message::TYPE_ERROR });
      }
      else
      {
        out_messages.push_back(
              { message, 0,
                yage::graphics::shader_compiler::compile_message::TYPE_INFO });
      }
    }

    start = end + 1;
    end   = log.find('\n', start);
  }

  if (status == GL_FALSE)
  {
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  GLint binary_length;
  glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binary_length);
  binary.resize(binary_length);
  GLenum binary_format;
  glGetProgramBinary(
        program, binary_length, nullptr, &binary_format, binary.data());
  glDeleteProgram(program);

  log(LOG_INFO, "Shader compiled successfully: %d bytes", binary_length);

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::shader_compiler::get_binary(
      std::vector<uint8_t> & out_data)
{
  if (binary.empty())
  {
    log(LOG_ERROR, "No binary data available");
    return {};
  }

  return {};
}
