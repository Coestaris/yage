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

#include "platform/graphics/gl/shader.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error
yage::platform::graphics::gl::shader::set_binary(std::vector<uint8_t> binary)
{
  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::shader::destroy()
{
  return yage::core::error::NOT_IMPLEMENTED;
}

std::vector<std::string> yage::platform::graphics::gl::shader::get_uniforms()
{
  return {};
}

std::vector<std::string> yage::platform::graphics::gl::shader::get_attributes()
{
  return {};
}

yage::core::error yage::platform::graphics::gl::shader::bind()
{
  return yage::core::error::NOT_IMPLEMENTED;
}

yage::core::error yage::platform::graphics::gl::shader::unbind()
{
  return yage::core::error::NOT_IMPLEMENTED;
}

int yage::platform::graphics::gl::shader::get_uniform_location_direct(
      std::string name)
{
  return yage::core::error::NOT_IMPLEMENTED;
}

yage::core::error
yage::platform::graphics::gl::shader::set_uniform_1i(int location, int value)
{
  return yage::core::error::NOT_IMPLEMENTED;
}

yage::core::error
yage::platform::graphics::gl::shader::set_uniform_1f(int location, float value)
{
  return yage::core::error::NOT_IMPLEMENTED;
}

yage::core::error yage::platform::graphics::gl::shader::set_uniform_2f(
      int location, yage::core::float2 value)
{
  return yage::core::error::NOT_IMPLEMENTED;
}

yage::core::error yage::platform::graphics::gl::shader::set_uniform_3f(
      int location, yage::core::float3 value)
{
  return yage::core::error::NOT_IMPLEMENTED;
}

yage::core::error yage::platform::graphics::gl::shader::set_uniform_mat4f(
      int location, float * value)
{
  return yage::core::error::NOT_IMPLEMENTED;
}
