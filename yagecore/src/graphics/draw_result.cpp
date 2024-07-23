/** ========================================================================
 *
 * @file draw_result.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/draw_result.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::graphics::draw_result::draw_result(
      size_t drawn_meshes, size_t drawn_triangles)
#if YAGE_COLLECT_DRAW_STATS
: drawn_meshes(drawn_meshes)
, drawn_triangles(drawn_triangles) {};
#else
      {};
#endif

yc::graphics::draw_result
yc::graphics::draw_result::operator+=(const draw_result & other)
{
#if YAGE_COLLECT_DRAW_STATS
  drawn_meshes += other.drawn_meshes;
  drawn_triangles += other.drawn_triangles;
#endif
  return *this;
}

size_t yc::graphics::draw_result::get_drawn_meshes() const
{
#if YAGE_COLLECT_DRAW_STATS
  return drawn_meshes;
#else
  return 0;
#endif
}

size_t yc::graphics::draw_result::get_drawn_triangles() const
{
#if YAGE_COLLECT_DRAW_STATS
  return drawn_triangles;
#else
  return 0;
#endif
}