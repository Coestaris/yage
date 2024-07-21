/** ========================================================================
 *
 * @file sync_point.hpp
 * @date 07-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <mutex>
#include <condition_variable>

#include "yagecore/log.hpp"

namespace yc::util
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class sync_point
{
private:
  std::mutex              m;
  std::condition_variable cv;
  int                     counter = 0;
  int                     waiting = 0;
  int                     thread_count;

  bool closed = false;

public:
  sync_point(int count);

  void wait();
  void shutdown();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}