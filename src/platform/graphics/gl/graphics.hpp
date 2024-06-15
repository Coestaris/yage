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

  yage::core::error draw_line(float x1, float y1, float x2, float y2) override;
  yage::core::error draw_rect(float x, float y, float width, float height) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}

