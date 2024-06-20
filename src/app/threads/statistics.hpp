/** ========================================================================
 *
 * @file updater.hpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/util/named_thread.hpp"
#include "graphics/window.hpp"
#include "core/util/sync_point.hpp"
#include "core/util/profiler.hpp"
#include "app/application.hpp"

namespace yage::app::threads
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class logic : public yage::core::util::named_thread
{
private:
  yage::app::application *     app;
  yage::core::util::profiler * profiler;
  bool                         should_close = false;

  void profile(int point)
  {
    if (profiler)
    {
      profiler->passed(point);
    }
  }

public:
  logic(yage::app::application * app, yage::core::util::profiler * profiler)
  : yage::core::util::named_thread("logic")
  , app(app)
  , profiler(profiler)
  {
  }

  yage::core::util::profiler::time get_update_time() const
  {
    return profiler->get_diff(
          yage::app::profiler_points::PROF_POINT_LOGIC_INPUT,
          yage::app::profiler_points::PROF_POINT_LOGIC_UPDATE_PARTICLES);
  }

  yage::core::util::profiler::time get_particles_time() const
  {
    return profiler->get_diff(
          yage::app::profiler_points::PROF_POINT_LOGIC_UPDATE_PARTICLES,
          yage::app::profiler_points::PROF_POINT_LOGIC_UPDATE_PHYSICS);
  }

  yage::core::util::profiler::time get_physics_time() const
  {
    return profiler->get_diff(
          yage::app::profiler_points::PROF_POINT_LOGIC_UPDATE_PHYSICS,
          yage::app::profiler_points::PROF_POINT_LOGIC_UPDATE_AI);
  }

  yage::core::util::profiler::time get_ai_time() const
  {
    return profiler->get_diff(
          yage::app::profiler_points::PROF_POINT_LOGIC_UPDATE_AI,
          yage::app::profiler_points::PROF_POINT_LOGIC_UPDATE_SOUND);
  }

  yage::core::util::profiler::time get_sound_time() const
  {
    return profiler->get_diff(
          yage::app::profiler_points::PROF_POINT_LOGIC_UPDATE_SOUND,
          yage::app::profiler_points::PROF_POINT_LOGIC_POST_FRAME);
  }

  void run() override;

  void close();
};

}