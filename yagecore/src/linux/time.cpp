/** ========================================================================
 *
 * @file time.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <sys/time.h>
#include <cstdio>
#include "yagecore/time.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
struct tm yc::get_time()
{
  time_t t = time(nullptr);
  return *localtime(&t);
}

yc::milliseconds yc::get_ms()
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return static_cast<float>(ts.tv_sec) * 1000.0f
         + static_cast<float>(ts.tv_nsec) / 1000000.0f;
}

yc::nano_seconds yc::get_ns()
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}