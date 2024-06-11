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

  virtual yage::core::error draw_line(int x1, int y1, int x2, int y2) = 0;
  virtual yage::core::error draw_rect(int x, int y, int width, int height) = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}


