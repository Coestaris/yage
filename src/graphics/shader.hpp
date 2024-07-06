/** ========================================================================
 *
 * @file shader.hpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <unordered_map>
#include <cstdint>

#include "graphics/asset.hpp"
#include "core/hash.hpp"
#include "shader_type.hpp"
#include "asset_cacher.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;

class shader : public yage::graphics::asset
{
public:
  class data
  {
  private:
    /* type -> source url */
    std::unordered_map<yage::graphics::shader_type, std::string> sources;

  public:
    std::string get_hash(const std::string& name) const
    {
      yage::core::fast_hasher hasher;
      for (const auto & [type, url] : sources)
      {
        hasher.add(static_cast<int>(type));
        hasher.add(url);
      }

      return "_shader" + name + "_" + hasher.as_string();
    }

    void add_source(yage::graphics::shader_type type, std::string url)
    {
      sources[type] = url;
    }

    bool operator==(const data & other) const
    {
      return sources == other.sources;
    }

    /* Implement iterator */
    auto begin()
    {
      return sources.begin();
    }
    auto end()
    {
      return sources.end();
    }

    bool empty() const
    {
      return sources.empty();
    }
  };

protected:
  yage::graphics::graphics * graphics;
  data                       sources;

  shader(
        std::string                    name,
        yage::graphics::asset_cacher * cacher,
        yage::graphics::graphics *     graphics)
  : yage::graphics::asset(std::move(name), cacher)
  , graphics(graphics)
  {
  }

public:
  void set_sources(const data & _sources)
  {
    if (!(sources == _sources))
    {
      sources = _sources;
      live    = false;
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}