/** ========================================================================
*
* @file graphics.hpp
* @date 06-06-2024
* @author Maxim Kurylko <vk_vm@ukr.net>
*
*  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include "core/error.hpp"

namespace yage::graphics
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics
{
public:
  virtual yage::core::error clear() = 0;
  virtual yage::core::error flush() = 0;

  virtual yage::core::error draw_line(float x1, float y1, float x2, float y2) = 0;
  virtual yage::core::error draw_rect(float x, float y, float width, float height) = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}


