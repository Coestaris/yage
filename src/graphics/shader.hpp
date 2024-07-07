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
#include <utility>

#include "graphics/asset.hpp"
#include "core/hash.hpp"
#include "asset_cacher.hpp"
#include "core/util/uri.hpp"

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
    std::string language;

    /* type -> source url */
    std::unordered_map<std::string, yage::core::util::uri> sources;

  public:
    data() = default;
    data(std::string language)
    : language(std::move(language))
    {
    }

    std::string get_hash(const std::string & name) const
    {
      yage::core::fast_hasher hasher;
      for (const auto & [type, uri] : sources)
      {
        hasher.add(type);
        hasher.add(uri.get_uri());
      }

      return "_shader" + name + "_" + hasher.as_string();
    }

    void add_source(std::string type, const yage::core::util::uri & url)
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

    std::string get_language() const
    {
      return language;
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