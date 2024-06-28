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