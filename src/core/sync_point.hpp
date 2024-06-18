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

#include "log.hpp"

namespace yage::core
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class sync_point
{
private:
  std::mutex              m;
  std::condition_variable cv;
  int                     counter;
  int                     waiting;
  int                     thread_count;

  bool closed = false;

public:
  sync_point(int count)
  : counter(0)
  , waiting(0)
  , thread_count(count)
  {
  }

  void wait()
  {
    if (closed)
    {
      return;
    }

    // fence mechanism
    std::unique_lock<std::mutex> lk(m);
    ++counter;
    ++waiting;
    cv.wait(lk, [&] { return counter >= thread_count; });
    cv.notify_one();
    --waiting;
    if (waiting == 0)
    {
      // reset barrier
      counter = 0;
    }
    lk.unlock();
  }

  void shutdown()
  {
    cv.notify_all();
    counter = thread_count;
    waiting = 0;
    closed = true;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}