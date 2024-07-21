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
  size_t           calls        = 0;
  yc::milliseconds prev_ms      = 0.0f;
  yc::milliseconds prev_call_ms = 0.0f;
  float            cps          = 0.0f;

  yc::milliseconds desired_interval;

public:
  explicit frequency_keeper(size_t max_cps = 60);

  float call();

  [[nodiscard]] float get_cps() const;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}