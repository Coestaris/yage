/** ========================================================================
 *
 * @file shader_compiler.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <string>

#include "graphics/shader.hpp"

namespace yage::platform::graphics::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader : public yage::graphics::shader
{
public:
  explicit shader(yage::graphics::graphics * graphics)
  : yage::graphics::shader(graphics)
  {
  }
  ~shader() override = default;

  yage::core::error        set_binary(std::vector<uint8_t> binary) override;
  yage::core::error        destroy() override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
