/** ========================================================================
 *
 * @file texture.cpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 * @copyright Ajax Systems
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "texture.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error yage::platform::graphics::gl::texture::bind(size_t slot)
{
  return yage::core::error::OK;
}

yage::core::error yage::platform::graphics::gl::texture::destroy()
{
  return yage::core::error::OK;
}

yage::core::error
yage::platform::graphics::gl::texture::set_binary(std::vector<uint8_t> binary)
{
  return yage::core::error::OK;
}
