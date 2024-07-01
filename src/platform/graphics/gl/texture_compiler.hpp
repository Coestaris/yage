/** ========================================================================
 *
 * @file texture_compiler.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics/texture_compiler.hpp"

namespace yage::platform::graphics::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class texture_compiler : public yage::graphics::texture_compiler
{
private:
  std::vector<std::string> image_paths;
  yage::graphics::texture_param parameters;

public:
  ~texture_compiler() override = default;
  explicit texture_compiler(yage::graphics::graphics * graphics)
  : yage::graphics::texture_compiler(graphics)
  {
  }

  void clear() override;
  yage::core::error
  set_parameters(const yage::graphics::texture_param & parameters) override;
  yage::core::error add_image(const std::string & path) override;
  yage::core::error compile(yage::graphics::texture * & out_texture) override;

  yage::core::error add_latent(size_t w, size_t h, const std::vector<glm::vec4> & initial_values) override;
  yage::core::error add_latent(size_t w, size_t h, glm::vec4 initial_value) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}