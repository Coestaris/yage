/** ========================================================================
 *
 * @file asset_factory.hpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <string>

#include "core/error.hpp"

namespace yage::app::assets
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class shader;
class texture;

class asset_factory
{
public:
  virtual ~asset_factory() = default;

  virtual yage::core::error read_assets(yage::graphics::graphics * graphics) = 0;
  virtual yage::core::error destroy() = 0;

  virtual yage::app::assets::shader *  get_shader(const std::string & name) = 0;
  virtual yage::app::assets::texture * get_texture(const std::string & name) = 0;
};

}
