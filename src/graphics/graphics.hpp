/** ========================================================================
 * @file graphics.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include "core/error.hpp"

#include "glm/vec2.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader;
class texture;
class asset_cacher;

class graphics
{
public:
  virtual yage::core::error clear() = 0;
  virtual yage::core::error flush() = 0;

  virtual yage::graphics::shader * create_shader(
        std::string                    name,
        yage::graphics::asset_cacher * cacher,
        yage::graphics::graphics *     graphics)
        = 0;
  virtual yage::core::error destroy_shader(yage::graphics::shader * shader) = 0;

  virtual yage::graphics::texture * create_texture(
        std::string                    name,
        yage::graphics::asset_cacher * cacher,
        yage::graphics::graphics *     graphics)
        = 0;
  virtual yage::core::error destroy_texture(yage::graphics::texture * texture)
        = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}
