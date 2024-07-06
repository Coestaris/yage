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

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/** Interface for caching assets.
 * Some assets may be cached to avoid preprocessing them every time they are
 * needed. Depending on the platform, assets may be cached in different ways
 * (e.g. in memory, on disk, etc.). */
class asset_cacher
{
public:
  virtual ~asset_cacher() = default;

  /** Caches asset data.
   * @param key Unique key to identify the cache entry.
   * @param data Asset data to cache. */
  virtual yage::core::error
  cache_asset(const std::string & key, const std::vector<uint8_t> & data)
        = 0;

  /** Retrieves cached asset data.
   * @param key Unique key to identify the cache entry.
   * @param data Asset data to retrieve.
   * @return Returns non-zero error code if the cache entry is not found. */
  virtual yage::core::error
  get_cached_asset(const std::string & key, std::vector<uint8_t> & data)
        = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}