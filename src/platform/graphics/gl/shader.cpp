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

  live = true;
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

  for (auto & [type, url] : sources)
  {
    /* Load source */
    std::string source;
    std::ifstream file(url);
        if (!file.is_open())
        {
          logging(LOG_ERROR, "Failed to open file: %s", url.c_str());
          return yage::core::error::FILE_NOT_FOUND;
        }

    file.seekg(0, std::ios::end);
    source.resize(file.tellg());
    file.seekg(0, std::ios::beg);

    file.read(reinterpret_cast<char *>(source.data()), source.size());
    file.close();

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
      logging(
            LOG_ERROR, "Failed to compile shader of type %s",
            yage::graphics::to_string(type).c_str());
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