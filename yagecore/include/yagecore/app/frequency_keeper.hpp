/** ========================================================================
 *
 * @file frequency_keeper.hpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <thread>
#include "yagecore/time.hpp"

namespace yc::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class frequency_keeper
{
private:
  size_t           calls        = 0;
  yc::milliseconds prev_ms      = 0.0f;
  yc::milliseconds prev_call_ms = 0.0f;
  float            cps          = 0.0f;

  yc::milliseconds desired_interval;

public:
  explicit frequency_keeper(size_t max_cps = 60)
  {
    desired_interval = 1000.0f / max_cps;
  }

  float call()
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

  float get_cps() const
  {
    return cps;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}