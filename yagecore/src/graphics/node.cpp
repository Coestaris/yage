/** ========================================================================
 *
 * @file node.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/node.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yc::graphics::node::data::set_work_dir(const std::string & _work_dir)
{
  work_dir = _work_dir;
}

void yc::graphics::node::data::set_uri(yc::util::uri _uri)
{
  uri = _uri;
}

yc::util::uri yc::graphics::node::data::get_uri() const
{
  return uri;
}

std::string yc::graphics::node::data::get_work_dir() const
{
  return work_dir;
}

bool yc::graphics::node::data::operator==(const data & other) const
{
  return uri == other.uri;
}

bool yc::graphics::node::data::empty() const
{
  return uri.empty();
}

yc::graphics::node::node(
      std::string                             name,
      std::shared_ptr<yc::app::asset_cacher>  cacher,
      std::shared_ptr<yc::graphics::graphics> graphics,
      type                                    type)
: asset(std::move(name), std::move(cacher))
, graphics(std::move(graphics))
, type_(type)
{
  if (type != type::TOPLEVEL)
  {
    live   = true;
    loaded = true;
  }
}

void yc::graphics::node::set_sources(const data & _sources)
{
  if (!(sources == _sources))
  {
    logging(LOG_INFO, "Node '%s' marked to reload", name.c_str());
    sources = _sources;
    live    = false;
  }
}

yc::graphics::node::type yc::graphics::node::get_type() const
{
  return type_;
}

yc::graphics::draw_result yc::graphics::node::draw(
      const std::shared_ptr<yc::graphics::render_pass> & pass)
{
#if USE_DEBUG
  if (!live)
  {
    return draw_result();
  }
#endif

  draw_result result {};
  if (mesh)
  {
    result += mesh->draw(pass);
  }

  for (auto & child : children)
  {
    result += child->draw(pass);
  }

  return result;
}