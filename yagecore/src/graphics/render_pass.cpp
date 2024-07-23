/** ========================================================================
 *
 * @file render_pass.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/render_pass.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::graphics::render_pass::render_pass(std::string name, type type)
: name(std::move(name))
, type_(type)
{
}

void yc::graphics::render_pass::begin()
{
#if YAGE_COLLECT_DRAW_STATS
  drawn_meshes_imm    = 0;
  drawn_triangles_imm = 0;
  start_time          = yc::get_ms();
#endif

  on_begin();
}

void yc::graphics::render_pass::end()
{
  on_end();

#if YAGE_COLLECT_DRAW_STATS
  draw_time       = yc::get_ms() - start_time;
  drawn_meshes    = drawn_meshes_imm;
  drawn_triangles = drawn_triangles_imm;
#endif
}

yc::milliseconds yc::graphics::render_pass::get_draw_time() const
{
  return draw_time;
}

size_t yc::graphics::render_pass::get_drawn_meshes() const
{
  return drawn_meshes;
}

size_t yc::graphics::render_pass::get_drawn_triangles() const
{
  return drawn_triangles;
}

void yc::graphics::render_pass::record_result(
      const yc::graphics::draw_result & result)
{
  drawn_meshes_imm += result.get_drawn_meshes();
  drawn_triangles_imm += result.get_drawn_triangles();
}

void yc::graphics::render_pass::set_name(std::string _name)
{
  this->name = std::move(_name);
}

std::string yc::graphics::render_pass::get_name()
{
  return this->name;
}

void yc::graphics::render_pass::set_skip(bool _skip)
{
  this->skip = _skip;
}

bool yc::graphics::render_pass::get_skip()
{
  return this->skip;
}

yc::graphics::render_pass::type yc::graphics::render_pass::get_type()
{
  return this->type_;
}
