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

#include "graphics/asset.hpp"
#include "graphics/texture_param.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;

class texture : public yage::graphics::asset
{
public:
  class data
  {
  private:
    std::vector<std::string> urls;
    texture_param            parameters {};

  public:
    void add_source(std::string _url)
    {
      urls.push_back(_url);
    }

    void set_parameters(const texture_param & _parameters)
    {
      parameters = _parameters;
    }

    bool operator==(const data & other) const
    {
      return urls == other.urls && parameters == other.parameters;
    }

    bool empty() const
    {
      return urls.empty();
    }

    std::vector<std::string> get_urls() const
    {
      return urls;
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
        const std::string &    path,
        std::vector<uint8_t> & out_data,
        size_t &               out_w,
        size_t &               out_h);

  yage::graphics::graphics * graphics;
  data                       sources;

  texture(
        std::string                    name,
        yage::graphics::asset_cacher * cacher,
        yage::graphics::graphics *     graphics)
  : asset(std::move(name), cacher)
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