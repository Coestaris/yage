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
    std::vector<size_t> to_remove;
    size_t              index = 0;
    for (auto & object : app->objects_logic)
    {
      object->update();
      if (object->should_be_deleted)
      {
        to_remove.push_back(index);
      }
      index++;
    }
    for (auto obj_index : to_remove)
    {
      auto * obj = app->objects_logic[obj_index];
      app->objects_logic.erase(app->objects_logic.begin() + obj_index);
      delete obj;
    }
    instances_count = app->objects_logic.size();

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
    app->post_frame_sync.wait();
  }
}

void yage::app::threads::logic::close()
{
  should_close = true;
}
