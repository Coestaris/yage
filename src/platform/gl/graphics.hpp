/** ========================================================================
*
* @file graphics.hpp
* @date 06-06-2024
* @author Maxim Kurylko <vk_vm@ukr.net>
*
*  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <string>

#include "graphics/graphics.hpp"

namespace yage::platform::gl
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics : public yage::graphics::graphics
{
private:
  std::vector<std::string> extensions;

  void cache_extensions();
  void print_extensions();
  bool has_extension(const char * extension);

public:
  yage::core::error init(int bits_per_pixel, int width, int height);
  yage::core::error update();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}

