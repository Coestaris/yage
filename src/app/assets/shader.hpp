/** ========================================================================
 *
 * @file shader.hpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>

#include "app/assets/asset.hpp"
#include "app/assets/asset_cacher.hpp"
#include "graphics/shader.hpp"

namespace yage::app::assets
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader : public yage::app::assets::asset
{
public:
  using sources
        = std::vector<std::pair<yage::graphics::shader_type, std::string>>;

private:
  yage::graphics::shader *   program  = nullptr;
  yage::graphics::graphics * graphics = nullptr;

  std::string get_cache_key();

public:
  sources src;

  shader(
        const std::string &               name,
        yage::app::assets::asset_cacher * cacher,
        yage::graphics::graphics *        graphics)
  : asset(name, cacher)
  , graphics(graphics)
  {
  }

  ~shader() override = default;

  yage::core::error load() override;
  void              unload() override;

  yage::graphics::shader * get_shader()
  {
    if (!loaded)
    {
      return nullptr;
    }

    updated = false;
    return program;
  }

  void update(sources new_src)
  {
    src  = std::move(new_src);
    live = false;
  }
};
/* Variables ================================================================ */
/* Prototypes =============================================================== */
}