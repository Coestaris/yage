/** ========================================================================
 *
 * @file shader_type.cpp
 * @date 06-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "graphics/shader_type.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::string yage::graphics::to_string(yage::graphics::shader_type type)
{
  switch (type)
  {
    case yage::graphics::VERTEX_SHADER:
      return "VERTEX_SHADER";
    case yage::graphics::FRAGMENT_SHADER:
      return "FRAGMENT_SHADER";
    default:
      return "UNKNOWN_SHADER_TYPE";
  }
}