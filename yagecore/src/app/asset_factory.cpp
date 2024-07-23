/** ========================================================================
 *
 * @file asset_factory.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/app/asset_factory.hpp"

#include "yagecore/graphics/texture.hpp"
#include "yagecore/graphics/font.hpp"
#include "yagecore/graphics/shader.hpp"
#include "yagecore/graphics/node.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yc::app::asset_factory::poll_assets()
{
#if USE_DEBUG
  for (const auto & asset : loaded_assets)
  {
    asset->poll();
  }
#endif
}



std::shared_ptr<yc::app::asset>
yc::app::asset_factory::load_asset(const std::string & name, bool required)
{
  auto asset = get_asset(name);
  if (asset)
  {
    if (asset->load() != yc::OK)
    {
      logging(LOG_ERROR, "Failed to load asset '%s'", name.c_str());

      if (required)
      {
        throw std::runtime_error("Failed to load asset " + name);
      }
    }
    loaded_assets.push_back(asset);
  }
  return asset;
}

void yc::app::asset_factory::unload_asset(const std::string & name)
{
  for (const auto & asset : loaded_assets)
  {
    if (asset->get_name() == name)
    {
      asset->unload();
      return;
    }
  }
}

void yc::app::asset_factory::unload_assets()
{
  for (const auto & asset : loaded_assets)
  {
    asset->unload();
  }
  loaded_assets.clear();
}