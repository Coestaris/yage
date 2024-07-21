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

#include "yagecore/gl/shader.hpp"
#include "yagecore/gl/glfuncs.hpp"
#include "yagecore/log.hpp"
#include "yagecore/error.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yc::gl::shader::internal_set(GLint location, int value)
{
  yc::gl::glUniform1i(location, value);
}

void yc::gl::shader::internal_set(GLint location, float value)
{
  yc::gl::glUniform1f(location, value);
}

void yc::gl::shader::internal_set(GLint location, glm::vec2 value)
{
  yc::gl::glUniform2f(location, value.x, value.y);
}

void yc::gl::shader::internal_set(GLint location, glm::vec3 value)
{
  yc::gl::glUniform3f(location, value.x, value.y, value.z);
}

void yc::gl::shader::internal_set(GLint location, glm::vec4 value)
{
  yc::gl::glUniform4f(location, value.x, value.y, value.z, value.w);
}

void yc::gl::shader::internal_set(GLint location, glm::mat2 value)
{
  yc::gl::glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
}

void yc::gl::shader::internal_set(GLint location, glm::mat3 value)
{
  yc::gl::glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void yc::gl::shader::internal_set(GLint location, glm::mat4 value)
{
  yc::gl::glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

template <typename T> yc::gl::shader::uniform<T>::uniform(
      std::string                       name,
      std::shared_ptr<yc::gl::shader>   shader,
      std::shared_ptr<yc::gl::graphics> ctx)
: ctx(std::move(ctx))
, shader(std::move(shader))
, name(std::move(name))
{
  location = this->shader.lock()->get_uniform_location(this->name);
}

template <typename T> void yc::gl::shader::uniform<T>::reload()
{
  int old_loc = location;
  int new_loc = shader.lock()->get_uniform_location(name);

  logging(
        LOG_INFO, "Reloading uniform %s. %d->%d", name.c_str(), old_loc,
        new_loc);

  location = new_loc;

  set(prev_value);
}

template <typename T> void yc::gl::shader::uniform<T>::set(T value)
{
  prev_value = value;
  internal_set(location, value);
}

yc::error yc::gl::shader::load()
{
  if (loaded)
  {
    return OK;
  }

  live = true;

  if (sources.empty())
  {
    logging(LOG_ERROR, "No sources to load");
    return yc::error::INVALID_ARGUMENT;
  }

  std::string          hash = sources.get_hash(name);
  std::vector<uint8_t> binary;
  if (cacher->get_cached_asset(hash, binary) != yc::error::OK)
  {
    yc::error error = compile_binary(binary);
    if (error != yc::error::OK)
    {
      logging(LOG_ERROR, "Failed to compile shader");
      return error;
    }

    if (cacher->cache_asset(hash, binary) != yc::error::OK)
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

yc::error yc::gl::shader::load_from_binary(const std::vector<uint8_t> & binary)
{
  /* Create program */
  id = yc::gl::glCreateProgram();

  /* First 4 bytes of binary are shader type */
  GLenum type
        = *reinterpret_cast<GLenum *>(const_cast<uint8_t *>(binary.data()));

  /* Need to skip 4 bytes */
  yc::gl::glProgramBinary(
        id, type, binary.data() + sizeof(type), binary.size() - sizeof(type));

  /* Check if program is valid */
  GLint status;
  yc::gl::glGetProgramiv(id, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)
  {
    GLint length;
    yc::gl::glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

    std::string log(length, '\0');
    yc::gl::glGetProgramInfoLog(id, length, nullptr, log.data());

    logging(LOG_ERROR, "Failed to link program: %s", log.c_str());
    return yc::error::EXTERNAL_API_ERROR;
  }

  loaded = true;

#if USE_DEBUG
  use();
  /* Reload uniforms */
  for (auto & uniform : uniforms)
  {
    uniform->reload();
  }
#endif

  return yc::error::OK;
}

void yc::gl::shader::unload()
{
  if (!loaded)
  {
    return;
  }

  yc::gl::glDeleteProgram(id);

  loaded = false;
}

GLint yc::gl::shader::get_uniform_location(
      const std::string & name, bool required)
{
  GLint location = yc::gl::glGetUniformLocation(id, name.c_str());
  if (location == -1 && required)
  {
    throw std::runtime_error("Failed to get uniform location: " + name);
  }

  return location;
}

GLint yc::gl::shader::get_attribute_location(
      const std::string & name, bool required)
{
  GLint location = yc::gl::glGetAttribLocation(id, name.c_str());
  if (location == -1 && required)
  {
    throw std::runtime_error("Failed to get attribute location: " + name);
  }

  return location;
}

yc::error yc::gl::shader::compile_binary(std::vector<uint8_t> & binary)
{
  if (sources.empty())
  {
    logging(LOG_ERROR, "No shaders to compile");
    return yc::error::EMPTY;
  }

  logging(LOG_DEBUG, "Loading shader: %s", name.c_str());

  auto program = yc::gl::glCreateProgram();

  auto types = sources.get_types();
  for (auto & type : types)
  {
    /* Load source */
    auto source = sources.get_source(type);

    if (source.empty())
    {
      logging(LOG_ERROR, "Failed to read shader source: %s", type.c_str());
      return yc::error::EXTERNAL_API_ERROR;
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
      return yc::error::INVALID_ARGUMENT;
    }

    GLuint shader = yc::gl::glCreateShader(gl_type);
    if (!shader)
    {
      logging(LOG_ERROR, "Failed to create shader");
      return yc::error::EXTERNAL_API_ERROR;
    }

    const char * c_source = source.c_str();
    GLint        length   = source.size();
    yc::gl::glShaderSource(shader, 1, &c_source, &length);
    yc::gl::glCompileShader(shader);

    GLint status;
    yc::gl::glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    yc::gl::glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::string log;
    log.resize(length);
    yc::gl::glGetShaderInfoLog(shader, length, nullptr, log.data());

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
      return yc::error::EXTERNAL_API_ERROR;
    }

    yc::gl::glAttachShader(program, shader);
    yc::gl::glDeleteShader(shader);
  }

  yc::gl::glLinkProgram(program);

  GLint status;
  yc::gl::glGetProgramiv(program, GL_LINK_STATUS, &status);

  GLint length;
  yc::gl::glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
  std::string log;
  log.resize(length);
  yc::gl::glGetProgramInfoLog(program, length, nullptr, log.data());

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
    return yc::error::EXTERNAL_API_ERROR;
  }

  GLint binary_length;
  yc::gl::glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binary_length);
  binary.resize(binary_length);
  GLenum binary_format;
  yc::gl::glGetProgramBinary(
        program, binary_length, nullptr, &binary_format, binary.data());
  yc::gl::glDeleteProgram(program);

  /* Prepend binary format (4 bytes) */
  binary.insert(
        binary.begin(), reinterpret_cast<uint8_t *>(&binary_format),
        reinterpret_cast<uint8_t *>(&binary_format) + sizeof(binary_format));

  logging(LOG_INFO, "Shader compiled successfully: %d bytes", binary_length);

  return yc::error::OK;
}

yc::gl::shader::shader(
      std::string                                     name,
      std::shared_ptr<yc::app::asset_cacher>          cacher,
      const std::shared_ptr<yc::graphics::graphics> & graphics)
: yc::graphics::shader(std::move(name), std::move(cacher), graphics)
, ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics))
{
}

void yc::gl::shader::use() const
{
  yc::gl::glUseProgram(id);
}

template <typename T> std::shared_ptr<yc::gl::shader::uniform<T>>
yc::gl::shader::get_uniform(std::string name)
{
  auto u = std::make_shared<uniform<T>>(
        std::move(name), shared_from_this(), ctx);
  uniforms.push_back(u);
  return u;
}

/* Explicit instantiation */
template std::shared_ptr<yc::gl::shader::uniform<int>>
yc::gl::shader::get_uniform<int>(std::string name);
template std::shared_ptr<yc::gl::shader::uniform<float>>
yc::gl::shader::get_uniform<float>(std::string name);
template std::shared_ptr<yc::gl::shader::uniform<glm::vec2>>
yc::gl::shader::get_uniform<glm::vec2>(std::string name);
template std::shared_ptr<yc::gl::shader::uniform<glm::vec3>>
yc::gl::shader::get_uniform<glm::vec3>(std::string name);
template std::shared_ptr<yc::gl::shader::uniform<glm::vec4>>
yc::gl::shader::get_uniform<glm::vec4>(std::string name);
template std::shared_ptr<yc::gl::shader::uniform<glm::mat2>>
yc::gl::shader::get_uniform<glm::mat2>(std::string name);
template std::shared_ptr<yc::gl::shader::uniform<glm::mat3>>
yc::gl::shader::get_uniform<glm::mat3>(std::string name);
template std::shared_ptr<yc::gl::shader::uniform<glm::mat4>>
yc::gl::shader::get_uniform<glm::mat4>(std::string name);