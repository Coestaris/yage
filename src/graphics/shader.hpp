/** ========================================================================
 *
 * @file shader.hpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>
#include <string>
#include <vector>
#include <cstdint>

#include "graphics/asset.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;

class shader : public yage::graphics::asset
{
protected:
  yage::graphics::graphics * graphics;
  std::vector<uint8_t>       binary;

public:
  shader(
        std::string                name,
        yage::graphics::graphics * graphics,
        std::vector<uint8_t>       compiled)
  : asset(std::move(name))
  , graphics(graphics)
  , binary(std::move(compiled))
  {
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}