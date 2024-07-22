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
#include <cstddef>

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class draw_result
{
  size_t drawn_meshes {};
  size_t drawn_triangles {};

public:
  draw_result() = default;
  draw_result(size_t drawn_meshes, size_t drawn_triangles);

  draw_result operator+=(const draw_result & other);

  [[nodiscard]] size_t get_drawn_meshes() const;
  [[nodiscard]] size_t get_drawn_triangles() const;
};

/* Variables ================================================================
 */
/* Prototypes ===============================================================
 */
}