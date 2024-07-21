/** ========================================================================
 *
 * @file time.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/time.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
struct tm yc::get_time()
{
  /* Windows implementation */
  time_t t = time(nullptr);
  return *localtime(&t);
}

yc::milliseconds yc::get_ms()
{
  /* Windows implementation */
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000 + ts.tv_nsec / 1e6;
}

yc::nano_seconds yc::get_ns()
{
  /* Windows implementation */
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1e9 + ts.tv_nsec;
}
