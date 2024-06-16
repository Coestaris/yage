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
#include "core/float2.hpp"

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

  virtual yage::core::error draw_line(yage::core::float2 a, yage::core::float2 b) = 0;
  virtual yage::core::error draw_rect(yage::core::float2 position, yage::core::float2 dim) = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}


