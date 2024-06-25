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
public:
  ~texture_compiler() override = default;
  explicit texture_compiler(yage::graphics::graphics * graphics)
  : yage::graphics::texture_compiler(graphics)
  {
  }

  void clear() override;
  yage::core::error set_parameters(const yage::graphics::texture_param & parameters) override;
  yage::core::error add_image(const std::string & path)         override;
  std::string       get_hash()                                  override;
  yage::core::error compile()                                   override;
  yage::core::error get_binary(std::vector<uint8_t> & out_data) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}