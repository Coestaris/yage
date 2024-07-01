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
#include "graphics/texture.hpp"
#include "graphics/texture_param.hpp"
#include "graphics/graphics.hpp"

namespace yage::app::assets
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class texture : public yage::app::assets::asset
{
private:
  yage::graphics::texture *  pixmap {};
  yage::graphics::graphics * graphics {};

  bool live {};
  bool loaded {};

public:
  yage::graphics::texture_param parameters;
  std::vector<std::string>      image_paths;

  std::string get_cache_key();

  texture(
        const std::string &               name,
        yage::app::assets::asset_cacher * cacher,
        yage::graphics::graphics *        graphics)
  : asset(name, cacher)
  , graphics(graphics)
  {
  }

  ~texture() override = default;

  yage::core::error load() override;
  void              unload() override;

  bool is_loaded() const override
  {
    return loaded;
  }

#if USE_DEBUG
  void poll() override
  {
    if (!live)
    {
      unload();
      load();
    }
  }
#endif

  void update(
        const yage::graphics::texture_param & _parameters,
        const std::vector<std::string> &      _image_paths)
  {
    parameters  = _parameters;
    image_paths = _image_paths;
    live        = false;
  }
};
/* Variables ================================================================ */
/* Prototypes =============================================================== */
}