/** ========================================================================
 *
 * @file updater.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "graphics/threads/logic.hpp"
#include "graphics/threads/threads.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yage::graphics::threads::logic::run()
{
  while (1)
  {
    if (should_close)
    {
      break;
    }

    /* Stage 1: Sync with updater before frame */
    profiler->passed(PROF_POINT_LOGIC_PRE_FRAME);
    pre_frame_sync->wait();

    /* Stage 2: Process input */
    profiler->passed(PROF_POINT_LOGIC_INPUT);
    /* TODO: Implement input processing */

    /* Stage 3: Update objects */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_OBJECTS);
    /* TODO: Implement objects updating */

    /* Stage 4: Update particles */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_PARTICLES);
    /* TODO: Implement particles updating */

    /* Stage 5: Update physics */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_PHYSICS);
    /* TODO: Implement physics updating */

    /* Stage 6: Update AI */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_AI);
    /* TODO: Implement AI updating */

    /* Stage 7: Update sound */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_SOUND);
    /* TODO: Implement sound updating */

    /* Stage 8: Sync with renderer after frame */
    profiler->passed(PROF_POINT_LOGIC_POST_FRAME);
    post_frame_sync->wait();
  }
}

void yage::graphics::threads::logic::close()
{
  should_close = true;
}
