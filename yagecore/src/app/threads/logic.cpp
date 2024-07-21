/** ========================================================================
 *
 * @file updater.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <vector>
#include <mutex>
#include "../../../include/yagecore/app/threads/logic.hpp"
#include "../../../include/yagecore/app/application.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
enum profiler_points
{
  PROF_POINT_LOGIC_PRE_FRAME,
  PROF_POINT_LOGIC_INPUT,
  PROF_POINT_LOGIC_UPDATE_OBJECTS,
  PROF_POINT_LOGIC_UPDATE_PARTICLES,
  PROF_POINT_LOGIC_UPDATE_PHYSICS,
  PROF_POINT_LOGIC_UPDATE_AI,
  PROF_POINT_LOGIC_UPDATE_SOUND,
  PROF_POINT_LOGIC_POST_FRAME,
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::app::threads::logic::logic(
      std::shared_ptr<yc::app::application> app,
      std::shared_ptr<yc::util::profiler>   profiler)
: yc::util::named_thread("logic")
, app(std::move(app))
, profiler(std::move(profiler)) {};

void yc::app::threads::logic::run()
{
  while (true)
  {
    if (should_close)
    {
      break;
    }

    /* Stage 1: Sync with updater before frame */
    profiler->passed(PROF_POINT_LOGIC_PRE_FRAME);
    app->common_data.pre_frame_sync.wait();

    /* Stage 2: Process input */
    profiler->start_cycle();
    profiler->passed(PROF_POINT_LOGIC_INPUT);
    app->get_input()->update();

    /* Stage 3: Update objects */
    profiler->passed(PROF_POINT_LOGIC_UPDATE_OBJECTS);
    std::vector<size_t> to_remove;
    size_t              index = 0;
    for (auto & object : app->common_data.objects)
    {
      object->update();
      if (object->logic_data.should_be_deleted)
      {
        to_remove.push_back(index);
      }
      index++;
    }
    for (auto obj_index : to_remove)
    {
      auto obj = app->common_data.objects[obj_index];
      app->common_data.objects.erase(
            app->common_data.objects.begin() + obj_index);
    }
    {
      std::lock_guard<std::mutex> lock(app->logic_data.object_to_add_mutex);
      app->common_data.objects.insert(
            app->common_data.objects.end(),
            app->logic_data.object_to_add.begin(),
            app->logic_data.object_to_add.end());
      app->logic_data.object_to_add.clear();
    }
    size_t instances_count = app->common_data.objects.size();

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
    app->common_data.post_frame_sync.wait();

    /* We have nothing to do here, so process statistics */
#if COLLECT_DRAW_STATISTICS
    app->stats.log_instances_count(instances_count);
    app->stats.log_update_time(profiler->get_diff(
          PROF_POINT_LOGIC_INPUT, PROF_POINT_LOGIC_UPDATE_PARTICLES));
    app->stats.log_particles_time(profiler->get_diff(
          PROF_POINT_LOGIC_UPDATE_PARTICLES, PROF_POINT_LOGIC_UPDATE_PHYSICS));
    app->stats.log_physics_time(profiler->get_diff(
          PROF_POINT_LOGIC_UPDATE_PHYSICS, PROF_POINT_LOGIC_UPDATE_AI));
    app->stats.log_ai_time(profiler->get_diff(
          PROF_POINT_LOGIC_UPDATE_AI, PROF_POINT_LOGIC_UPDATE_SOUND));
    app->stats.log_sound_time(profiler->get_diff(
          PROF_POINT_LOGIC_UPDATE_SOUND, PROF_POINT_LOGIC_POST_FRAME));
#endif
  }
}

void yc::app::threads::logic::close()
{
  should_close = true;
}
