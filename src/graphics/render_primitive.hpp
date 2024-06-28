/** ========================================================================
 *
 * @file render_primitive.hpp
 * @date 28-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "glm/mat4x4.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class material
{
public:
  material()          = default;
  virtual ~material() = default;
};

class render_primitive
{
public:
  render_primitive()          = default;
  virtual ~render_primitive() = default;

  glm::mat4  transform;
  material * mat;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}