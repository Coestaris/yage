/** ========================================================================
 *
 * @file updater.hpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "yagecore/time.hpp"
#include "yagecore/util/sync_point.hpp"
#include "yagecore/util/profiler.hpp"
#include "yagecore/util/named_thread.hpp"

#include "yagecore/graphics/window.hpp"
#include "yagecore/app/application.hpp"

namespace yc::app::threads
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class logic : public yc::util::named_thread
{
private:
  yc::app::application * app;
  yc::util::profiler *   profiler;
  size_t                 instances_count = 0;
  bool                   should_close    = false;

public:
  logic(yc::app::application * app, yc::util::profiler * profiler)
  : yc::util::named_thread("logic")
  , app(app)
  , profiler(profiler)
  {
  }

  size_t get_instances_count() const
  {
    return instances_count;
  }

  yc::milliseconds get_update_time() const
  {
    return profiler->get_diff(
          yc::app::profiler_points::PROF_POINT_LOGIC_INPUT,
          yc::app::profiler_points::PROF_POINT_LOGIC_UPDATE_PARTICLES);
  }

  yc::milliseconds get_particles_time() const
  {
    return profiler->get_diff(
          yc::app::profiler_points::PROF_POINT_LOGIC_UPDATE_PARTICLES,
          yc::app::profiler_points::PROF_POINT_LOGIC_UPDATE_PHYSICS);
  }

  yc::milliseconds get_physics_time() const
  {
    return profiler->get_diff(
          yc::app::profiler_points::PROF_POINT_LOGIC_UPDATE_PHYSICS,
          yc::app::profiler_points::PROF_POINT_LOGIC_UPDATE_AI);
  }

  yc::milliseconds get_ai_time() const
  {
    return profiler->get_diff(
          yc::app::profiler_points::PROF_POINT_LOGIC_UPDATE_AI,
          yc::app::profiler_points::PROF_POINT_LOGIC_UPDATE_SOUND);
  }

  yc::milliseconds get_sound_time() const
  {
    return profiler->get_diff(
          yc::app::profiler_points::PROF_POINT_LOGIC_UPDATE_SOUND,
          yc::app::profiler_points::PROF_POINT_LOGIC_POST_FRAME);
  }

  void run() override;

  void close();
};

}