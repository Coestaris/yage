/** ========================================================================
 *
 * @file font.hpp
 * @date 28-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics.hpp"
#include "glm/vec4.hpp"

#include <string>
#include <memory>
#include <utility>

#include "yagecore/log.hpp"
#include "yagecore/app/asset.hpp"
#include "yagecore/graphics/font_param.hpp"
#include "glm/vec2.hpp"

#include <vector>

#include <map>

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class font : public yc::app::asset
{
public:
  class data
  {
  private:
    yc::util::uri            uri {};
    std::string              work_dir {};
    yc::graphics::font_param parameters {};

  public:
    void set_work_dir(const std::string & _work_dir)
    {
      work_dir = _work_dir;
    }

    void set_uri(yc::util::uri _uri)
    {
      uri = _uri;
    }

    void set_parameters(const yc::graphics::font_param & _parameters)
    {
      parameters = _parameters;
    }

    yc::util::uri get_uri() const
    {
      return uri;
    }

    yc::graphics::font_param get_parameters() const
    {
      return parameters;
    }

    std::string get_work_dir() const
    {
      return work_dir;
    }

    bool operator==(const data & other) const
    {
      return uri == other.uri && parameters == other.parameters;
    }

    bool empty() const
    {
      return uri.empty();
    }
  };

protected:
  struct char_cache
  {
    glm::vec2    size;
    glm::vec2    bearing;
    unsigned int advance;
  };
  std::vector<char_cache> cache;

protected:
  std::shared_ptr<yc::graphics::graphics> graphics;
  data                                    sources;

  font(std::string                             name,
       std::shared_ptr<yc::app::asset_cacher>  cacher,
       std::shared_ptr<yc::graphics::graphics> graphics)
  : asset(std::move(name), std::move(cacher))
  , graphics(std::move(graphics)) {};

  virtual void
  draw(const char_cache & c, glm::vec2 pos, glm::vec4 color, glm::vec2 scale)
        = 0;

public:
  void set_sources(const data & _sources)
  {
    if (!(sources == _sources))
    {
      logging(LOG_INFO, "Font %s marked to reload", name.c_str());
      sources = _sources;
      live    = false;
    }
  }

  virtual void
  draw(const char      c,
       const glm::vec2 pos,
       const glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f },
       const glm::vec2 scale = { 1.0f, 1.0f })
  {
    draw(cache[c], pos, color, scale);
  }

  void
  draw(std::string     text,
       glm::vec2       pos,
       const glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f },
       const glm::vec2 scale = { 1.0f, 1.0f })
  {
    for (auto c : text)
    {
      auto & cache = this->cache[c];
      draw(cache, pos, color, scale);
      pos.x += cache.advance * scale.x;
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}