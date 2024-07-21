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

#include "yagecore/error.hpp"
#include "yagecore/log.hpp"
#include "yagecore/graphics/graphics.hpp"
#include "yagecore/app/asset.hpp"

namespace yc::app
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
  std::vector<std::shared_ptr<yc::app::asset>> loaded_assets;

protected:
  /** Tries to get an asset by its name.
   * @param name Name of the asset to get.
   * @return Returns a pointer to the asset if it is found, otherwise nullptr.
   */
  virtual std::shared_ptr<yc::app::asset> get_asset(const std::string & name)
        = 0;

public:
  virtual ~asset_factory() = default;

  /** Reads assets from the asset storage.
   * None of assets are loaded into memory at this point.
   * @param graphics Graphics context to load assets into.
   * @return Returns non-zero error code if the assets could not be read. */
  virtual yc::error
  read_assets(std::shared_ptr<yc::graphics::graphics> graphics)
        = 0;

  /** Destroys the asset factory and all its assets
   * also unloads all loaded assets and frees all found assets. */
  virtual yc::error destroy() = 0;

  /*
   * Handy methods for loading/unloading assets
   */

  /** Checks if there newer versions of assets in the asset storage.
   * If so - reloads them. May be useful for debugging purposes. */
  void poll_assets();

  /** Finds and loads an asset by its name.
   * @param name Name of the asset to load.
   * @return Returns a pointer to the asset if it is found and loaded,
   *         otherwise nullptr. */
  std::shared_ptr<yc::app::asset>
  load_asset(const std::string & name, bool required = true);

  /** Same as get_asset, but casts the result to the specified type.
   * @note This will return nullptr if the asset is not of the specified type.
   */
  template <typename T>
  std::shared_ptr<T> load_asset(const std::string & name, bool required = true)
  {
    auto ptr = std::dynamic_pointer_cast<T>(load_asset(name, required));
    if (!ptr && required)
    {
      throw std::runtime_error("Failed to load asset " + name);
    }

    return ptr;
  }

  /** Unloads an asset by its name.
   * @param name Name of the asset to unload. */
  void unload_asset(const std::string & name);

  /** Unloads all loaded assets. */
  void unload_assets();
};

}
