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
#include <utility>
#include <memory>

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

  std::shared_ptr<yc::app::asset_cacher> cacher;

  bool live {};
  bool loaded {};

public:
  virtual ~asset() = default;

  asset(std::string name, std::shared_ptr<yc::app::asset_cacher> cacher);

  /** Loads asset data into memory allocating necessary resources. */
  virtual yc::error load() = 0;

  /** Unloads asset data from memory freeing allocated resources. */
  virtual void unload() = 0;

  /** True if asset data is loaded into memory. */
  bool is_loaded() const;

  /** False is data in asset store different from loaded data into memory.
   * In that case, asset should be reloaded. */
  void poll();

  /** Returns the name of the asset. */
  [[nodiscard]] const std::string & get_name() const;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}