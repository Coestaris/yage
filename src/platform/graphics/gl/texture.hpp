/** ========================================================================
 *
 * @file image.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics/texture.hpp"

namespace yage::platform::graphics::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class texture : public yage::graphics::texture
{
public:
  ~texture() override = default;
  explicit texture(
        yage::graphics::graphics *            graphics,
        const yage::graphics::texture_param & parameters)
  : yage::graphics::texture(graphics, parameters)
  {
  }

  yage::core::error set_binary(std::vector<uint8_t> binary) override;
  yage::core::error destroy() override;
  yage::core::error bind(size_t slot) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}