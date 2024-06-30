/** ========================================================================
 *
 * @file trivial_render_chain.cpp
 * @date 28-06-2024
 * @author Maxim Kurylko <kurylko.m@ajax.systems>
 * @copyright Ajax Systems
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <vector>
#include "graphics/render_pass.hpp"
#include "graphics/graphics.hpp"
#include "platform/graphics/platform.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::vector<yage::graphics::render_pass *>
yage::platform::graphics::trivial_render_chain(
      yage::graphics::graphics * graphics)
{
  return {};
}