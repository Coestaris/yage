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
private:
  bool      transfrom_valid = false;
  glm::mat4 transform {};

  glm::vec3 min {};
  glm::vec3 max {};

public:
  aabb() = default;
  aabb(const glm::vec3 & min, const glm::vec3 & max)
  : min(min)
  , max(max) {};

  [[nodiscard]] const glm::vec3 & get_min() const
  {
    return min;
  }

  [[nodiscard]] const glm::vec3 & get_max() const
  {
    return max;
  }

  void extend(const aabb & other)
  {
    min             = glm::min(min, other.min);
    max             = glm::max(max, other.max);
    transfrom_valid = false;
  }

  void extend(const glm::vec3 & point)
  {
    min             = glm::min(min, point);
    max             = glm::max(max, point);
    transfrom_valid = false;
  }

  glm::mat4 get_transform()
  {
    if (!transfrom_valid)
    {
      glm::vec3 size   = glm::vec3(max.x - min.x, max.y - min.y, max.z - min.z);
      glm::vec3 center = glm::vec3(
            (max.x + min.x) / 2.0f, (max.y + min.y) / 2.0f,
            (max.z + min.z) / 2.0f);

      transform = glm::translate(glm::mat4(1), center)
                  * glm::scale(glm::mat4(1), size);
      transfrom_valid = true;
    }

    return transform;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}