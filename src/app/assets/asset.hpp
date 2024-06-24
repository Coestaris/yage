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

#include "core/error.hpp"

namespace yage::app::assets
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class asset_cacher;

class asset
{
protected:
  asset_cacher * cacher = nullptr;
  std::string    name;

public:
  asset(const char * name, asset_cacher * cacher)
  : asset(std::string(name), cacher)
  {
  }
  asset(const std::string & name, asset_cacher * cacher)
  : cacher(cacher)
  , name(name)
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
  virtual bool is_live() const;
#endif
};

}