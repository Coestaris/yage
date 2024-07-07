/** ========================================================================
 *
 * @file shader.cpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdexcept>

#include "platform/graphics/gl/shader.hpp"
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error yage::platform::graphics::gl::shader::load()
{
  if (sources.empty())
  {
    logging(LOG_ERROR, "No sources to load");
    return yage::core::error::INVALID_ARGUMENT;
  }

  std::string          hash = sources.get_hash(name);
  std::vector<uint8_t> binary;
  if (cacher->get_cached_asset(hash, binary) != yage::core::error::OK)
  {
    yage::core::error error = compile_binary(binary);
    if (error != yage::core::error::OK)
    {
      logging(LOG_ERROR, "Failed to compile shader");
      return error;
    }

    if (cacher->cache_asset(hash, binary) != yage::core::error::OK)
    {
      logging(LOG_WARN, "Failed to cache shader");
    }
  }
  else
  {
    logging(LOG_DEBUG, "Loaded shader from cache (key: %s)", hash.c_str());
  }

  return load_from_binary(binary);
}

yage::core::error yage::platform::graphics::gl::shader::load_from_binary(
      const std::vector<uint8_t> & binary)
{
  /* Create program */
  id = ctx->glCreateProgram();

  /* First 4 bytes of binary are shader type */
  GLenum type
        = *reinterpret_cast<GLenum *>(const_cast<uint8_t *>(binary.data()));

  /* Need to skip 4 bytes */
  ctx->glProgramBinary(
        id, type, binary.data() + sizeof(type), binary.size() - sizeof(type));

  /* Check if program is valid */
  GLint status;
  ctx->glGetProgramiv(id, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)
  {
    GLint length;
    ctx->glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

    std::string log(length, '\0');
    ctx->glGetProgramInfoLog(id, length, nullptr, log.data());

    logging(LOG_ERROR, "Failed to link program: %s", log.c_str());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  live   = true;
  loaded = true;

  return yage::core::error::OK;
}

void yage::platform::graphics::gl::shader::unload()
{
  ctx->glDeleteProgram(id);
}

GLint yage::platform::graphics::gl::shader::get_uniform_location(
      const std::string & name, bool required)
{
  GLint location = ctx->glGetUniformLocation(id, name.c_str());
  if (location == -1 && required)
  {
    throw std::runtime_error("Failed to get uniform location: " + name);
  }

  return location;
}

GLint yage::platform::graphics::gl::shader::get_attribute_location(
      const std::string & name, bool required)
{
  GLint location = ctx->glGetAttribLocation(id, name.c_str());
  if (location == -1 && required)
  {
    throw std::runtime_error("Failed to get attribute location: " + name);
  }

  return location;
}

yage::core::error yage::platform::graphics::gl::shader::compile_binary(
      std::vector<uint8_t> & binary)
{
  if (sources.empty())
  {
    logging(LOG_ERROR, "No shaders to compile");
    return yage::core::error::EMPTY;
  }

  logging(LOG_DEBUG, "Loading shader: %s", name.c_str());

  auto program = ctx->glCreateProgram();

  for (auto & [type, uri] : sources)
  {
    /* Load source */
    std::vector<uint8_t> source;
    uri.read(source);
    if (source.empty())
    {
      logging(
            LOG_ERROR, "Failed to read shader source: %s",
            uri.get_uri().c_str());
      return yage::core::error::EXTERNAL_API_ERROR;
    }

    GLuint gl_type;
    if (type == "VERTEX")
    {
      gl_type = GL_VERTEX_SHADER;
    }
    else if (type == "FRAGMENT")
    {
      gl_type = GL_FRAGMENT_SHADER;
    }
    else
    {
      logging(LOG_ERROR, "Unknown shader type %s", type.c_str());
      return yage::core::error::INVALID_ARGUMENT;
    }

    GLuint shader = ctx->glCreateShader(gl_type);
    if (!shader)
    {
      logging(LOG_ERROR, "Failed to create shader");
      return yage::core::error::EXTERNAL_API_ERROR;
    }

    const char * c_source = reinterpret_cast<const char *>(source.data());
    GLint        length   = source.size();
    ctx->glShaderSource(shader, 1, &c_source, &length);
    ctx->glCompileShader(shader);

    GLint status;
    ctx->glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
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
          logging(LOG_ERROR, "Shader compilation error: %s", message.c_str());
        }
        else
        {
          logging(LOG_INFO, "Shader compilation info: %s", message.c_str());
        }
      }

      start = end + 1;
      end   = log.find('\n', start);
    }

    if (status == GL_FALSE)
    {
      logging(LOG_ERROR, "Failed to compile shader of type %s", type.c_str());
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
        logging(LOG_ERROR, "Shader linking error: %s", message.c_str());
      }
      else
      {
        logging(LOG_INFO, "Shader linking info: %s", message.c_str());
      }
    }

    start = end + 1;
    end   = log.find('\n', start);
  }

  if (status == GL_FALSE)
  {
    logging(LOG_ERROR, "Failed to link program");
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