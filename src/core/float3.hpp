/** ========================================================================
 *
 * @file float3.hpp
 * @date 07-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/math.hpp"

namespace yage::core
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class float3
{
public:
  float x;
  float y;
  float z;

  float3()
  : x(0.0f)
  , y(0.0f)
  , z(0.0f)
  {
  }
  float3(float x, float y, float z)
  : x(x)
  , y(y)
  , z(z)
  {
  }

  float3 operator+(const float3 & other) const
  {
    return float3(x + other.x, y + other.y, z + other.z);
  }

  float3 operator-(const float3 & other) const
  {
    return float3(x - other.x, y - other.y, z - other.z);
  }

  float3 operator*(float scalar) const
  {
    return float3(x * scalar, y * scalar, z * scalar);
  }

  float3 operator/(float scalar) const
  {
    return float3(x / scalar, y / scalar, z / scalar);
  }

  float3 & operator+=(const float3 & other)
  {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  float3 & operator-=(const float3 & other)
  {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  float3 & operator*=(float scalar)
  {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }

  float3 & operator/=(float scalar)
  {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
  }

  bool operator==(const float3 & other) const
  {
    return x == other.x && y == other.y && z == other.z;
  }

  bool operator!=(const float3 & other) const
  {
    return x != other.x || y != other.y || z != other.z;
  }

  float length() const
  {
    return yage::core::sqrt(x * x + y * y + z * z);
  }

  float3 normalized() const
  {
    return *this / length();
  }

  float dot(const float3 & other) const
  {
    return x * other.x + y * other.y + z * other.z;
  }

  float3 lerp(const float3 & other, float t) const
  {
    return *this + (other - *this) * t;
  }

  float3 reflect(const float3 & normal) const
  {
    return *this - normal * 2.0f * dot(normal);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
