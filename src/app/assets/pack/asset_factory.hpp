/** ========================================================================
 *
 * @file asset_factory.hpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <unordered_map>
#include <utility>
#include "app/assets/shader.hpp"
#include "app/assets/asset_factory.hpp"
#include "app/assets/asset_cacher.hpp"
#include "json/json.h"

namespace yage::app::assets::pack
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class asset_factory : public yage::app::assets::asset_factory
{
private:
  std::string extract_path {};
  std::string path;

  std::unordered_map<std::string, yage::app::assets::shader *> shaders;

  class asset_cacher : public yage::app::assets::asset_cacher
  {
  private:
    std::string cache_path {};

  public:
    explicit asset_cacher(std::string cache_path)
    : cache_path(std::move(cache_path))
    {
    }

    yage::core::error cache_asset(
          const std::string & key, const std::vector<uint8_t> & data) override;
    yage::core::error get_cached_asset(
          const std::string & key, std::vector<uint8_t> & data) override;
  } cacher;

  yage::app::assets::shader * parse_shader(
        yage::graphics::graphics * graphics,
        const std::string & basename,
        const Json::Value & root);

public:
  asset_factory(std::string path, std::string cache_path)
  : path(std::move(path))
  , cacher(std::move(cache_path)) {};

  ~asset_factory()
  {
    for (auto & shader : shaders)
    {
      delete shader.second;
    }
  }

  yage::core::error read_assets(yage::graphics::graphics * graphics) override;
  yage::core::error get_shader(
        const std::string & name, yage::app::assets::shader ** shader) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}