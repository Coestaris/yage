/** ========================================================================
 *
 * @file asset.hpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <utility>

#include "core/error.hpp"
#include "app/assets/asset_cacher.hpp"

namespace yage::app::assets
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
/** Base class for all assets.
 * Assets are resources that are loaded into memory and used by
 * the application (e.g. textures, sounds, etc.). */
class asset
{
protected:
  /** Interface for performing caching operations. */
  yage::app::assets::asset_cacher * cacher = nullptr;

  /** Programmer-readable name of the asset.
   * It is used to identify the asset in the asset storage. */
  std::string                       name;

public:
  asset(std::string name, yage::app::assets::asset_cacher * cacher)
  : cacher(cacher)
  , name(std::move(name))
  {
  }

  virtual ~asset() = default;

  /** Loads asset data into memory allocating necessary resources. */
  virtual yage::core::error load() = 0;
  /** Unloads asset data from memory freeing allocated resources. */
  virtual void unload() = 0;
  /** True if asset data is loaded into memory. */
  virtual bool is_loaded() const = 0;

#if USE_DEBUG
  /** False is data in asset store different from loaded data into memory.
   * In that case, asset should be reloaded. */
  virtual void poll() = 0;
#endif

  /** Returns the name of the asset. */
  [[nodiscard]] const std::string & get_name() const
  {
    return name;
  }
};

}