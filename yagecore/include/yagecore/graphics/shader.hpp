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
    std::unordered_map<std::string, yc::util::uri> uris;
    /* Cache for loaded sources */
    std::unordered_map<std::string, std::string> sources;

  public:
    data() = default;
    data(std::string language)
    : language(std::move(language))
    {
    }

    std::string get_hash(const std::string & name)
    {
      yc::fast_hasher hasher;
      for (const auto & [type, uri] : uris)
      {
        hasher.add(type);
        hasher.add(get_source(type));
      }

      return "_shader" + name + "_" + hasher.as_string();
    }

    void add_source(const std::string & type, const yc::util::uri & url)
    {
      uris[type] = url;
    }

    bool operator==(data & other)
    {
      /* Compare URI by content, not as a string */
      if (uris.size() != other.uris.size())
      {
        return false;
      }

      for (const auto & [type, uri] : uris)
      {
        if (other.uris.find(type) == other.uris.end())
        {
          return false;
        }

        if (uri != other.uris.at(type))
        {
          return false;
        }

        std::string old_source = get_source(type);
        std::string new_source = other.get_source(type);

        if (old_source != new_source)
        {
          return false;
        }
      }

      return true;
    }

    std::vector<std::string> get_types() const
    {
      std::vector<std::string> types;
      for (const auto & [type, uri] : uris)
      {
        types.push_back(type);
      }

      return types;
    }

    std::string get_source(const std::string & type)
    {
      if (sources.find(type) == sources.end())
      {
        std::vector<uint8_t> data;
        uris.at(type).read(data);
        sources[type] = std::string(data.begin(), data.end());
      }

      return sources.at(type);
    }

    bool empty() const
    {
      return uris.empty();
    }

    std::string get_language() const
    {
      return language;
    }
  };

protected:
  std::shared_ptr<yc::graphics::graphics> graphics;
  data                                    sources;

  shader(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : yc::app::asset(std::move(name), std::move(cacher))
  , graphics(std::move(graphics))
  {
  }

public:
  void set_sources(data & _sources)
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