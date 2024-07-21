/** ========================================================================
 *
 * @file asset.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/app/asset.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::app::asset::asset(
      std::string name, std::shared_ptr<yc::app::asset_cacher> cacher)
: name(std::move(name))
, cacher(std::move(cacher)) {};

bool yc::app::asset::is_loaded() const
{
  return loaded;
}

void yc::app::asset::poll()
{
#if USE_DEBUG
  if (!live)
  {
    unload();
    load();
  }
#else
/* Do nothing */
#endif
}

[[nodiscard]] const std::string & yc::app::asset::get_name() const
{
  return name;
}