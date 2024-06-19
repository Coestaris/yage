/** ========================================================================
 *
 * @file float2.hpp
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
class float2
{
public:
  float x;
  float y;

  float2()
  : x(0.0f)
  , y(0.0f)
  {
  }
  float2(float x, float y)
  : x(x)
  , y(y)
  {
  }

  float2 operator+(const float2 & other) const
  {
    return float2(x + other.x, y + other.y);
  }

  float2 operator-(const float2 & other) const
  {
    return float2(x - other.x, y - other.y);
  }

  float2 operator*(float scalar) const
  {
    return float2(x * scalar, y * scalar);
  }

  float2 operator/(float scalar) const
  {
    return float2(x / scalar, y / scalar);
  }

  float2 & operator+=(const float2 & other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  float2 & operator-=(const float2 & other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  float2 & operator*=(float scalar)
  {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  float2 & operator/=(float scalar)
  {
    x /= scalar;
    y /= scalar;
    return *this;
  }

  bool operator==(const float2 & other) const
  {
    return x == other.x && y == other.y;
  }

  bool operator!=(const float2 & other) const
  {
    return x != other.x || y != other.y;
  }

  float length() const
  {
    return yage::core::sqrt(x * x + y * y);
  }

  float2 normalized() const
  {
    return *this / length();
  }

  float dot(const float2 & other) const
  {
    return x * other.x + y * other.y;
  }

  float2 lerp(const float2 & other, float t) const
  {
    return *this + (other - *this) * t;
  }

  float2 reflect(const float2 & normal) const
  {
    return *this - normal * 2.0f * dot(normal);
  }

  float2 rotate(float angle) const
  {
    float s = yage::core::sin(angle);
    float c = yage::core::cos(angle);
    return float2(x * c - y * s, x * s + y * c);
  }

  float2 perpendicular() const
  {
    return float2(-y, x);
  }

  float2 project(const float2 & other) const
  {
    return other * dot(other) / other.dot(other);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
