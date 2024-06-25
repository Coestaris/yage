/** ========================================================================
 *
 * @file texture_compiler.cpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "texture_compiler.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yage::platform::graphics::gl::texture_compiler::clear()
{
}

yage::core::error
yage::platform::graphics::gl::texture_compiler::set_parameters(
      const yage::graphics::texture_param & parameters)
{
  return yage::core::INVALID_STATE;
}

yage::core::error yage::platform::graphics::gl::texture_compiler::add_image(
      const std::string & path)
{
  return yage::core::INVALID_STATE;
}

std::string yage::platform::graphics::gl::texture_compiler::get_hash()
{
  return {};
}

yage::core::error yage::platform::graphics::gl::texture_compiler::compile()
{
  return yage::core::INVALID_STATE;
}

yage::core::error yage::platform::graphics::gl::texture_compiler::get_binary(
      std::vector<uint8_t> & out_data)
{
  return yage::core::INVALID_STATE;
}
