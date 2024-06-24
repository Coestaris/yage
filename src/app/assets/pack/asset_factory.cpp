/** ========================================================================
 *
 * @file asset_factory.cpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "asset_factory.hpp"
#include "core/log.hpp"
#include <zlib.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::app::assets::pack::asset_factory::~asset_factory()
{
}

yage::core::error yage::app::assets::pack::asset_factory::read_assets()
{
  /* 1. Unzip the archive */
  log(LOG_INFO, "zlib version: %s", zlibVersion());
  log(LOG_INFO, "Reading assets from %s", path.c_str());

  int err = 0;
  zip *z = zip_open("foo.zip", 0, &err);


  /* 2. Read .meta files */

  return yage::core::INVALID_STATE;
}

yage::core::error yage::app::assets::pack::asset_factory::get_shader(
      const std::string & name, yage::app::assets::shader ** shader)
{
  return yage::core::INVALID_STATE;
}
