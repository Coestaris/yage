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
#include <stdexcept>

#include "graphics/graphics.hpp"
#include "core/error.hpp"
#include "core/log.hpp"
#include "graphics/asset.hpp"

namespace yage::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class shader;

/** Interface for asset factories.
 * Asset factories are used to load assets from the asset storage,
 * like archives, databases, etc. */
class asset_factory
{
private:
  std::vector<yage::graphics::asset *> loaded_assets;

protected:
  /** Tries to get an asset by its name.
   * @param name Name of the asset to get.
   * @return Returns a pointer to the asset if it is found, otherwise nullptr.
   */
  virtual yage::graphics::asset * get_asset(const std::string & name) = 0;

public:
  virtual ~asset_factory() = default;

  /** Reads assets from the asset storage.
   * None of assets are loaded into memory at this point.
   * @param graphics Graphics context to load assets into.
   * @return Returns non-zero error code if the assets could not be read. */
  virtual yage::core::error read_assets(yage::graphics::graphics * graphics)
        = 0;

  /** Destroys the asset factory and all its assets
   * also unloads all loaded assets and frees all found assets. */
  virtual yage::core::error destroy() = 0;

  /*
   * Handy methods for loading/unloading assets
   */

  /** Checks if there newer versions of assets in the asset storage.
   * If so - reloads them. May be useful for debugging purposes. */
  void poll_assets()
  {
#if USE_DEBUG
    for (auto * asset : loaded_assets)
    {
      asset->poll();
    }
#endif
  }

  /** Finds and loads an asset by its name.
   * @param name Name of the asset to load.
   * @return Returns a pointer to the asset if it is found and loaded,
   *         otherwise nullptr. */
  yage::graphics::asset *
  load_asset(const std::string & name, bool required = true)
  {
    auto * asset = get_asset(name);
    if (asset)
    {
      if (asset->load() != yage::core::error::OK)
      {
        logging(LOG_ERROR, "Failed to load asset %s", name.c_str());

        if (required)
        {
          throw std::runtime_error("Failed to load asset " + name);
        }
      }
      loaded_assets.push_back(asset);
    }
    return asset;
  }

  /** Same as get_asset, but casts the result to the specified type.
   * @note This will return nullptr if the asset is not of the specified type.
   */
  template <typename T>
  T * load_asset(const std::string & name, bool required = true)
  {
    auto * ptr = dynamic_cast<T *>(load_asset(name));
    if (!ptr && required)
    {
      throw std::runtime_error("Failed to load asset " + name);
    }

    return ptr;
  }

  /** Unloads an asset by its name.
   * @param name Name of the asset to unload. */
  void unload_asset(const std::string & name)
  {
    for (auto * asset : loaded_assets)
    {
      if (asset->get_name() == name)
      {
        asset->unload();
        return;
      }
    }
  }

  /** Unloads all loaded assets. */
  void unload_assets()
  {
    for (auto * asset : loaded_assets)
    {
      asset->unload();
    }
    loaded_assets.clear();
  }
};

}
