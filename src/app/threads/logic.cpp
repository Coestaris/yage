/** ========================================================================
 *
 * @file updater.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "logic.hpp"
#include "app/application.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yage::app::threads::logic::run()
{
  while (true)
  {
    if (should_close)
    {
      break;
    }

    /* Stage 1: Sync with updater before frame */
    profiler->passed(PROF_POINT_LOGIC_PRE_FRAME);
    app->pre_frame_sync.wait();

    /* Stage 2: Process input */
    profiler->start_cycle();
    profiler->passed(PROF_POINT_LOGIC_INPUT);
    app->input.update();

    /* Stage 3: Update objects */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_OBJECTS);
    for (auto & object : app->objects_logic)
    {
      object->update();
    }

    /* Stage 4: Update particles */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_PARTICLES);
    /* TODO: Implement particles updating */

    /* Stage 5: Update physics */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_PHYSICS);
    /* Sleep for 10 ms to simulate physics updating */
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    /* TODO: Implement physics updating */

    /* Stage 6: Update AI */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_AI);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    /* TODO: Implement AI updating */

    /* Stage 7: Update sound */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_SOUND);
    /* TODO: Implement sound updating */

    /* Stage 8: Sync with renderer after frame */
    profiler->passed(PROF_POINT_LOGIC_POST_FRAME);
    app->post_frame_sync.wait();
  }
}

void yage::app::threads::logic::close()
{
  should_close = true;
}
