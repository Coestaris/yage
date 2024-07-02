/** ========================================================================
 *
 * @file texture.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>
#include <string>
#include <vector>

#include "graphics/asset.hpp"
#include "graphics/texture_param.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;

class texture : public yage::graphics::asset
{
protected:
  yage::graphics::graphics * graphics;
  texture_param              parameters;

public:
  texture(
        std::string                name,
        yage::graphics::graphics * graphics,
        const texture_param &      parameters)
  : asset(std::move(name))
  , graphics(graphics)
  , parameters(parameters)
  {
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}