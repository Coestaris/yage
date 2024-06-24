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

#include "core/error.hpp"
#include "app/assets/asset.hpp"
#include "app/assets/shader.hpp"

namespace yage::app::assets
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class asset_cacher
{
public:
  virtual ~asset_cacher() = default;

  virtual yage::core::error cache_asset(
        asset *                      asset,
        const std::string &          name,
        const std::vector<uint8_t> & data)
        = 0;
};

class asset_factory
{
public:
  virtual ~asset_factory() = default;

  virtual yage::core::error read_assets() = 0;

  virtual yage::core::error
  get_shader(const std::string & name, yage::app::assets::shader ** shader)
        = 0;
};

}
