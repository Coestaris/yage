/** ========================================================================
 *
 * @file shader.cpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <utility>

#include "graphics/shader.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
int yage::graphics::shader::get_uniform_location(std::string name)
{
  if (uniform_cache.find(name) != uniform_cache.end())
  {
    return uniform_cache[name];
  }

  int location        = get_uniform_location_direct(name);
  uniform_cache[name] = location;

  return location;
}

yage::core::error
yage::graphics::shader::set_uniform_1i(std::string name, int value)
{
  return set_uniform_1i(get_uniform_location(std::move(name)), value);
}

yage::core::error
yage::graphics::shader::set_uniform_1f(std::string name, float value)
{
  return set_uniform_1f(get_uniform_location(std::move(name)), value);
}

yage::core::error yage::graphics::shader::set_uniform_2f(
      std::string name, glm::vec2 value)
{
  return set_uniform_2f(get_uniform_location(std::move(name)), value);
}

yage::core::error yage::graphics::shader::set_uniform_3f(
      std::string name, glm::vec3 value)
{
  return set_uniform_3f(get_uniform_location(std::move(name)), value);
}

yage::core::error
yage::graphics::shader::set_uniform_mat4f(std::string name, float * value)
{
  return set_uniform_mat4f(get_uniform_location(std::move(name)), value);
}
yage::core::error
yage::graphics::shader::set_binary(std::vector<uint8_t> binary)
{
  return yage::core::NOT_IMPLEMENTED;
}
yage::core::error yage::graphics::shader::bind()
{
  return yage::core::NOT_IMPLEMENTED;
}
yage::core::error yage::graphics::shader::unbind()
{
  return yage::core::NOT_IMPLEMENTED;
}
yage::core::error
yage::graphics::shader::set_uniform_1i(int location, int value)
{
  return yage::core::NOT_IMPLEMENTED;
}
yage::core::error
yage::graphics::shader::set_uniform_1f(int location, float value)
{
  return yage::core::NOT_IMPLEMENTED;
}
yage::core::error
yage::graphics::shader::set_uniform_2f(int location, glm::vec2 value)
{
  return yage::core::NOT_IMPLEMENTED;
}
yage::core::error
yage::graphics::shader::set_uniform_3f(int location, glm::vec3 value)
{
  return yage::core::NOT_IMPLEMENTED;
}
yage::core::error
yage::graphics::shader::set_uniform_mat4f(int location, float * value)
{
  return yage::core::NOT_IMPLEMENTED;
}
