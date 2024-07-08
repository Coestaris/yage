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
#include <fstream>

#include "yagecore/error.hpp"
#include "yagecore/app/asset_cacher.hpp"

namespace yc::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class asset
{
protected:
  /** Programmer-readable name of the asset.
   * It is used to identify the asset in the asset storage. */
  std::string name;

  yc::app::asset_cacher * cacher;

  bool live {};
  bool loaded {};

public:
  asset(std::string name, yc::app::asset_cacher * cacher)
  : name(std::move(name))
  , cacher(cacher)
  {
  }

  virtual ~asset() = default;

  /** Loads asset data into memory allocating necessary resources. */
  virtual yc::error load() = 0;
  /** Unloads asset data from memory freeing allocated resources. */
  virtual void unload() = 0;

  /** True if asset data is loaded into memory. */
  bool is_loaded() const
  {
    return loaded;
  }

#if USE_DEBUG
  /** False is data in asset store different from loaded data into memory.
   * In that case, asset should be reloaded. */
  void poll()
  {
    if (!live)
    {
      unload();
      load();
    }
  }
#endif

  /** Returns the name of the asset. */
  [[nodiscard]] const std::string & get_name() const
  {
    return name;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}