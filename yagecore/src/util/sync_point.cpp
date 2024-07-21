/** ========================================================================
 *
 * @file sync_point.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/util/sync_point.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::util::sync_point::sync_point(int count)
: thread_count(count)
{
}

void yc::util::sync_point::wait()
{
  if (closed)
  {
    return;
  }

  /* fence mechanism */
  std::unique_lock<std::mutex> lk(m);
  ++counter;
  ++waiting;
  cv.wait(lk, [&] { return counter >= thread_count; });
  cv.notify_one();
  --waiting;
  if (waiting == 0)
  {
    /* reset barrier */
    counter = 0;
  }
  lk.unlock();
}

void yc::util::sync_point::shutdown()
{
  cv.notify_all();
  counter = thread_count;
  waiting = 0;
  closed  = true;
}