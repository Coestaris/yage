/** ========================================================================
 *
 * @file draw_result.hpp
 * @date 28-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <cstdint>

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
#if COLLECT_DRAW_STATISTICS
class draw_result
{
  size_t drawn_meshes;
  size_t drawn_triangles;

public:
  draw_result() = default;
  draw_result(size_t drawn_meshes, size_t drawn_triangles)
  : drawn_meshes(drawn_meshes)
  , drawn_triangles(drawn_triangles) {};

  draw_result operator+=(const draw_result & other)
  {
    drawn_meshes += other.drawn_meshes;
    drawn_triangles += other.drawn_triangles;
    return *this;
  }

  [[nodiscard]] size_t get_drawn_meshes() const
  {
    return drawn_meshes;
  }

  [[nodiscard]] size_t get_drawn_triangles() const
  {
    return drawn_triangles;
  }
};

#else

/* In case of release build -
 * do not store any data */
class draw_result
{
public:
  draw_result() = default;
  draw_result(size_t drawn_meshes, size_t drawn_triangles) {};

  draw_result operator+=(const draw_result & other)
  {
    return *this;
  }

  [[nodiscard]] size_t get_drawn_meshes() const
  {
    return 0;
  }

  [[nodiscard]] size_t get_drawn_triangles() const
  {
    return 0;
  }
};
#endif

/* Variables ================================================================
 */
/* Prototypes ===============================================================
 */
}