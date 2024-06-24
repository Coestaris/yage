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
#include "app/assets/asset_factory.hpp"

namespace yage::app::assets::pack
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class asset_factory : public yage::app::assets::asset_factory
{
private:
  std::string path;
  std::string cache_path;

public:
  asset_factory(const std::string & path, const std::string & cache_path)
  : path(path)
  , cache_path(cache_path) {};
  ~asset_factory();

  yage::core::error read_assets() override;
  yage::core::error get_shader(
        const std::string & name, yage::app::assets::shader ** shader) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}