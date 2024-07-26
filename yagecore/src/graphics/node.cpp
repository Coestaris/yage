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

void yc::graphics::node::data::set_uri(yc::data_source _src)
{
  src = _src;
}

yc::data_source yc::graphics::node::data::get_src() const
{
  return src;
}

std::string yc::graphics::node::data::get_work_dir() const
{
  return work_dir;
}

bool yc::graphics::node::data::operator==(const data & other) const
{
  return src == other.src;
}

bool yc::graphics::node::data::empty() const
{
  return src.empty();
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