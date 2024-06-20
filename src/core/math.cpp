/** ========================================================================
 *
 * @file math.cpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "core/math.hpp"

/* This is a workaround for the fact that cmath does
 * not define the functions in the global namespace. */
#include <cmath>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
float yage::core::abs(float x)
{
  return std::abs(x);
}

float yage::core::sin(float x)
{
  return std::sin(x);
}

float yage::core::cos(float x)
{
  return std::cos(x);
}

float yage::core::tan(float x)
{
  return std::tan(x);
}

float yage::core::asin(float x)
{
  return std::asin(x);
}

float yage::core::acos(float x)
{
  return std::acos(x);
}

float yage::core::atan(float x)
{
  return std::atan(x);
}

float yage::core::atan2(float y, float x)
{
  return std::atan2(y, x);
}

float yage::core::sqrt(float x)
{
  return std::sqrt(x);
}

float yage::core::pow(float x, float y)
{
  return std::pow(x, y);
}

float yage::core::exp(float x)
{
  return std::exp(x);
}

float yage::core::lerp(float a, float b, float t)
{
  return a + (b - a) * t;
}