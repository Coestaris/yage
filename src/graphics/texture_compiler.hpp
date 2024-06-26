/** ========================================================================
 *
 * @file texture_compiler.hpp
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
class texture_compiler
{
protected:
  yage::graphics::graphics * graphics;

  struct rgba8888
  {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  };

  /* little helper to load image,
   * so backends do not reinvent the wheel */
  bool image_to_rgba8888(
        const std::string &     path,
        std::vector<rgba8888> & out_data,
        yage::core::float2 &    out_size);

public:
  virtual ~texture_compiler() = default;
  explicit texture_compiler(yage::graphics::graphics * graphics)
  : graphics(graphics)
  {
  }

  virtual void clear() = 0;
  virtual yage::core::error
  set_parameters(const yage::graphics::texture_param & parameters)
        = 0;
  virtual yage::core::error add_image(const std::string & path)         = 0;
  virtual yage::core::error compile()                                   = 0;
  virtual yage::core::error get_binary(std::vector<uint8_t> & out_data) = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}