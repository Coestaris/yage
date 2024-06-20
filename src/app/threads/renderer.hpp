/** ========================================================================
 *
 * @file renderer.hpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/util/named_thread.hpp"
#include "core/os_events.hpp"
#include "graphics/window.hpp"
#include "app/frequency_keeper.hpp"
#include "app/application.hpp"
#include "core/util/sync_point.hpp"
#include "core/util/profiler.hpp"

namespace yage::app::threads
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class renderer : public yage::core::util::named_thread
{
private:
  yage::app::application *     app;
  yage::core::util::profiler * profiler;
  yage::app::frequency_keeper  keeper;

public:
  yage::core::signal_event_listener stop_signal_listener;

  void run() override;

  yage::core::util::profiler::time get_draw_time() const
  {
    return profiler->get_diff(
          yage::app::profiler_points::PROF_POINT_RENDERER_DRAW_START,
          yage::app::profiler_points::PROF_POINT_RENDERER_DRAW_END);
  }

  yage::core::util::profiler::time get_copy_time() const
  {
    return profiler->get_diff(
          yage::app::profiler_points::PROF_POINT_RENDERER_DRAW_END,
          yage::app::profiler_points::PROF_POINT_RENDERER_UPDATE_START);
  }

  float get_keeper_cps() const
  {
    return keeper.get_cps();
  }

public:
  renderer(yage::app::application * app, yage::core::util::profiler * profiler)
  : yage::core::util::named_thread("renderer")
  , app(app)
  , profiler(profiler)
  {
    int max_fps = 0;
    app->window->get_max_fps(max_fps);
    keeper = yage::app::frequency_keeper(max_fps);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}