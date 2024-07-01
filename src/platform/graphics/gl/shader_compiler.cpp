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
#include "platform/graphics/gl/graphics.hpp"
#include "core/log.hpp"
#include "core/hash.hpp"
#include "glplatform.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yage::platform::graphics::gl::shader_compiler::clear()
{
  sources.clear();
  binary.clear();
}

yage::core::error yage::platform::graphics::gl::shader_compiler::add_shader(
      std::string source, yage::graphics::shader_type type)
{
  //  logging(LOG_DEBUG, "Adding shader of type %d", type);

  if (sources.find(type) != sources.end())
  {
    logging(LOG_ERROR, "Shader of type %d already exists", type);
    return yage::core::error::ALREADY_EXISTS;
  }

  sources[type] = source;

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::shader_compiler::compile(
      std::vector<yage::graphics::shader_compiler::compile_message> &
            out_messages)
{
  if (sources.empty())
  {
    logging(LOG_ERROR, "No shaders to compile");
    return yage::core::error::EMPTY;
  }

  auto program = ctx->glCreateProgram();

  for (auto & [type, source] : sources)
  {
    //    logging(LOG_DEBUG, "Compiling shader of type %d", type);
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
        logging(LOG_ERROR, "Unknown shader type %d", type);
        return {};
    }

    GLuint shader = ctx->glCreateShader(gl_type);
    if (!shader)
    {
      logging(LOG_ERROR, "Failed to create shader");
      return yage::core::error::EXTERNAL_API_ERROR;
    }

    const char * c_source = source.c_str();
    ctx->glShaderSource(shader, 1, &c_source, nullptr);
    ctx->glCompileShader(shader);

    GLint status;
    ctx->glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    GLint length;
    ctx->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::string log;
    log.resize(length);
    ctx->glGetShaderInfoLog(shader, length, nullptr, log.data());

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

    ctx->glAttachShader(program, shader);
    ctx->glDeleteShader(shader);
  }

  ctx->glLinkProgram(program);

  GLint status;
  ctx->glGetProgramiv(program, GL_LINK_STATUS, &status);

  GLint length;
  ctx->glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
  std::string log;
  log.resize(length);
  ctx->glGetProgramInfoLog(program, length, nullptr, log.data());

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
  ctx->glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binary_length);
  binary.resize(binary_length);
  GLenum binary_format;
  ctx->glGetProgramBinary(
        program, binary_length, nullptr, &binary_format, binary.data());
  ctx->glDeleteProgram(program);

  /* Prepend binary format (4 bytes) */
  binary.insert(
        binary.begin(), reinterpret_cast<uint8_t *>(&binary_format),
        reinterpret_cast<uint8_t *>(&binary_format) + sizeof(binary_format));

  logging(LOG_INFO, "Shader compiled successfully: %d bytes", binary_length);

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::shader_compiler::get_binary(
      std::vector<uint8_t> & out_data)
{
  if (binary.empty())
  {
    logging(LOG_ERROR, "No binary data available");
    return {};
  }

  out_data = binary;

  return {};
}
