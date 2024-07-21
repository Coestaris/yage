/** ========================================================================
 *
 * @file frequency_keeper.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/app/frequency_keeper.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::app::frequency_keeper::frequency_keeper(size_t max_cps)
{
  desired_interval = 1000.0f / max_cps;
}

float yc::app::frequency_keeper::call()
{
  calls++;
  yc::milliseconds ms              = yc::get_ms();
  yc::milliseconds prev_call_delta = ms - prev_call_ms;

  prev_call_ms = ms;

  if (prev_call_delta < desired_interval)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(
          static_cast<int>(desired_interval - prev_call_delta)));
  }

  yc::milliseconds delta = ms - prev_ms;

  if (delta > 1000.0f)
  {
    cps     = calls / delta * 1000.0f;
    calls   = 0;
    prev_ms = ms;
  }

  return prev_call_delta;
}

float yc::app::frequency_keeper::get_cps() const
{
  return cps;
}