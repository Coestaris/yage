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
yage::core::error
yage::platform::graphics::gl::shader::set_binary(std::vector<uint8_t> binary)
{
  /* Create program */
  id = ctx->glCreateProgram();

  /* First 4 bytes of binary are shader type */
  GLenum type = *reinterpret_cast<GLenum *>(binary.data());

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

  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::shader::destroy()
{
  ctx->glDeleteProgram(id);

  return yage::core::error::OK;
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