/** ========================================================================
 *
 * @file texture.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>
#include <string>
#include <vector>

#include "yagecore/graphics/texture_param.hpp"
#include "yagecore/log.hpp"
#include "yagecore/app/asset.hpp"
#include "yagecore/util/uri.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;

class texture : public yc::app::asset
{
public:
  class data
  {
  private:
    std::vector<yc::util::uri> uris {};
    texture_param              parameters {};

  public:
    void add_uri(yc::util::uri uri)
    {
      uris.push_back(uri);
    }

    void set_parameters(const texture_param & _parameters)
    {
      parameters = _parameters;
    }

    bool operator==(const data & other) const
    {
      return uris == other.uris && parameters == other.parameters;
    }

    bool empty() const
    {
      return uris.empty();
    }

    std::vector<yc::util::uri> get_uris() const
    {
      return uris;
    }

    texture_param get_parameters() const
    {
      return parameters;
    }
  };

protected:
  /* little helper to load image,
   * so backends do not reinvent the wheel */
  static bool load_image(
        const yc::util::uri &  uri,
        std::vector<uint8_t> & out_data,
        size_t &               out_w,
        size_t &               out_h);

  yc::graphics::graphics * graphics;
  data                     sources;

  texture(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics)
  : asset(std::move(name), cacher)
  , graphics(graphics)
  {
  }

public:
  void set_sources(const data & _sources)
  {
    /* TODO: check if sources are different */
    if (sources.empty())
    //    if (!(sources == _sources))
    {
      logging(LOG_INFO, "Texture %s marked to reload", name.c_str());
      sources = _sources;
      live    = false;
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}