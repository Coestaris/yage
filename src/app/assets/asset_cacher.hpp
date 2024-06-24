/** ========================================================================
 *
 * @file asset_cacher.hpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <cstdint>
#include <string>

#include "core/error.hpp"

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

  virtual yage::core::error
  cache_asset(const std::string & key, const std::vector<uint8_t> & data)
        = 0;
  virtual yage::core::error
  get_cached_asset(const std::string & key, std::vector<uint8_t> & data)
        = 0;
};

}