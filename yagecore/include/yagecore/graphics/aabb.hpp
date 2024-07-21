/** ========================================================================
 *
 * @file aabb.hpp
 * @date 14-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class aabb
{
  bool      transfrom_valid = false;
  glm::mat4 transform {};

  glm::vec3 min {};
  glm::vec3 max {};

public:
  aabb() = default;
  aabb(const glm::vec3 & min, const glm::vec3 & max);

  void extend(const aabb & other);
  void extend(const glm::vec3 & point);

  [[nodiscard]] const glm::vec3 & get_min() const;
  [[nodiscard]] const glm::vec3 & get_max() const;
  [[nodiscard]] glm::mat4         get_transform();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}