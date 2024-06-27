/** ========================================================================
 *
 * @file time.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "core/time.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
struct tm yage::core::get_time()
{
  /* Windows implementation */
  time_t t = time(nullptr);
  return *localtime(&t);
}

yage::core::milliseconds yage::core::get_ms()
{
  /* Windows implementation */
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000 + ts.tv_nsec / 1e6;
}

yage::core::nano_seconds yage::core::get_ns()
{
  /* Windows implementation */
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1e9 + ts.tv_nsec;
}
