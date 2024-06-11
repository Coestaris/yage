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

namespace yage::platform::graphics::gl
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

  yage::core::error clear() override;
  yage::core::error flush() override;

  yage::core::error draw_line(int x1, int y1, int x2, int y2) override;
  yage::core::error draw_rect(int x, int y, int width, int height) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}

