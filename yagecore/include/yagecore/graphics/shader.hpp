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

#include "yagecore/app/asset.hpp"
#include "yagecore/log.hpp"
#include "yagecore/app/asset_cacher.hpp"
#include "yagecore/hash.hpp"
#include "yagecore/util/uri.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;

class shader : public yc::app::asset
{
public:
  class data
  {
  private:
    std::string language;

    /* type -> source url */
    std::unordered_map<std::string, yc::util::uri> sources;

  public:
    data() = default;
    data(std::string language)
    : language(std::move(language))
    {
    }

    std::string get_hash(const std::string & name) const
    {
      yc::fast_hasher hasher;
      for (const auto & [type, uri] : sources)
      {
        hasher.add(type);
        hasher.add(uri.get_uri());
      }

      return "_shader" + name + "_" + hasher.as_string();
    }

    void add_source(std::string type, const yc::util::uri & url)
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
  yc::graphics::graphics * graphics;
  data                     sources;

  shader(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics)
  : yc::app::asset(std::move(name), cacher)
  , graphics(graphics)
  {
  }

public:
  void set_sources(const data & _sources)
  {
    if (!(sources == _sources))
    {
      logging(LOG_INFO, "Shader %s marked to reload", name.c_str());
      sources = _sources;
      live    = false;
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}