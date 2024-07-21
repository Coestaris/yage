/** ========================================================================
 *
 * @file aabb.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/aabb.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::graphics::aabb::aabb(const glm::vec3 & min, const glm::vec3 & max)
: min(min)
, max(max) {};

const glm::vec3 & yc::graphics::aabb::get_min() const
{
  return min;
}

const glm::vec3 & yc::graphics::aabb::get_max() const
{
  return max;
}

void yc::graphics::aabb::extend(const aabb & other)
{
  min             = glm::min(min, other.min);
  max             = glm::max(max, other.max);
  transfrom_valid = false;
}

void yc::graphics::aabb::extend(const glm::vec3 & point)
{
  min             = glm::min(min, point);
  max             = glm::max(max, point);
  transfrom_valid = false;
}

glm::mat4 yc::graphics::aabb::get_transform()
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