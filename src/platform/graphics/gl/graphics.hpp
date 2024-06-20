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

  yage::graphics::shader_compiler * create_shader_compiler() override;
  yage::core::error destroy_shader_compiler(yage::graphics::shader_compiler * compiler) override;
  yage::graphics::shader * create_shader() override;
  yage::core::error destroy_shader(yage::graphics::shader * shader) override;

  yage::core::error clear() override;
  yage::core::error flush() override;

  yage::core::error draw_line(yage::core::float2 a, yage::core::float2 b) override;
  yage::core::error draw_rect(yage::core::float2 position, yage::core::float2 dim) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}

