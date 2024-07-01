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

  /* little helper to load image,
   * so backends do not reinvent the wheel */
  bool load_image(
        const std::string &      path,
        std::vector<glm::vec4> & out_data,
        size_t &                 out_w,
        size_t &                 out_h);

public:
  virtual ~texture_compiler() = default;
  explicit texture_compiler(yage::graphics::graphics * graphics)
  : graphics(graphics)
  {
  }

  virtual void clear() = 0;
  virtual yage::core::error set_parameters(const yage::graphics::texture_param & parameters) = 0;
  virtual yage::core::error add_image(const std::string & path)         = 0;
  virtual yage::core::error compile(yage::graphics::texture * & out_texture) = 0;

  virtual yage::core::error add_latent(size_t w, size_t h, const std::vector<glm::vec4> & initial_values) = 0;
  virtual yage::core::error add_latent(size_t w, size_t h, glm::vec4 initial_value) = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}