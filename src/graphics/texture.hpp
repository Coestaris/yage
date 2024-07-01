/** ========================================================================
 *
 * @file texture.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics/graphics.hpp"
#include "graphics/texture_param.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class texture
{
protected:
  yage::graphics::graphics * graphics;
  texture_param              parameters;

  texture(yage::graphics::graphics * graphics, const texture_param & parameters)
  : graphics(graphics)
  , parameters(parameters)
  {
  }

public:
  virtual ~texture() = default;

  /** Deletes texture data from video memory */
  virtual yage::core::error destroy() = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}