/** ========================================================================
 *
 * @file font.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/font.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yc::graphics::font::data::set_work_dir(const std::string & _work_dir)
{
  work_dir = _work_dir;
}

void yc::graphics::font::data::set_uri(yc::util::uri _uri)
{
  uri = _uri;
}

void yc::graphics::font::data::set_parameters(
      const yc::graphics::font_param & _parameters)
{
  parameters = _parameters;
}

yc::util::uri yc::graphics::font::data::get_uri() const
{
  return uri;
}

yc::graphics::font_param yc::graphics::font::data::get_parameters() const
{
  return parameters;
}

std::string yc::graphics::font::data::get_work_dir() const
{
  return work_dir;
}

bool yc::graphics::font::data::operator==(const data & other) const
{
  return uri == other.uri && parameters == other.parameters;
}

bool yc::graphics::font::data::empty() const
{
  return uri.empty();
}

yc::graphics::font::font(
      std::string                             name,
      std::shared_ptr<yc::app::asset_cacher>  cacher,
      std::shared_ptr<yc::graphics::graphics> graphics)
: asset(std::move(name), std::move(cacher))
, graphics(std::move(graphics)) {};

void yc::graphics::font::set_sources(const data & _sources)
{
  if (!(sources == _sources))
  {
    logging(LOG_INFO, "Font %s marked to reload", name.c_str());
    sources = _sources;
    live    = false;
  }
}

void yc::graphics::font::draww(
      const std::string & text, glm::vec2 pos, const glm::vec4 color)
{
  if (!loaded)
  {
    return;
  }

  draw_begin(color);
  for (const auto c : text)
  {
    if (c > sources.get_parameters().get_end_char()
        || c < sources.get_parameters().get_start_char())
    {
      logging(LOG_ERROR, "Character %c is out of font range", c);
      goto exit;
    }

    size_t index = c - sources.get_parameters().get_start_char();
    if (index >= cache.size())
    {
      logging(LOG_ERROR, "Character %c is out of font range", c);
      goto exit;
    }

    auto & cache = this->cache[index];
    draw(cache, pos);
    pos.x += cache.x_advance;
  }

exit:
  draw_end();
}