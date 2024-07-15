/** ========================================================================
 *
 * @file renderer.hpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>

#include "yagecore/util/named_thread.hpp"
#include "yagecore/os_events.hpp"
#include "yagecore/util/sync_point.hpp"
#include "yagecore/util/profiler.hpp"

#include "yagecore/graphics/window.hpp"
#include "yagecore/graphics/render_pass.hpp"
#include "yagecore/app/frequency_keeper.hpp"
#include "yagecore/app/application.hpp"

namespace yc::app::threads
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class renderer final : public yc::util::named_thread
{
private:
  std::shared_ptr<yc::app::application> app;
  std::shared_ptr<yc::util::profiler>   profiler;
  yc::app::frequency_keeper             keeper;

  std::shared_ptr<yc::graphics::graphics> graphics {}; /* cache */

public:
  yc::signal_event_listener stop_signal_listener;

  void run() override;

  yc::milliseconds get_draw_time() const
  {
    return profiler->get_diff(
          yc::app::profiler_points::PROF_POINT_RENDERER_DRAW_START,
          yc::app::profiler_points::PROF_POINT_RENDERER_DRAW_END);
  }

  yc::milliseconds get_copy_time() const
  {
    return profiler->get_diff(
          yc::app::profiler_points::PROF_POINT_RENDERER_DRAW_END,
          yc::app::profiler_points::PROF_POINT_RENDERER_UPDATE_START);
  }

  float get_keeper_cps() const
  {
    return keeper.get_cps();
  }

public:
  renderer(
        std::shared_ptr<yc::app::application> app,
        std::shared_ptr<yc::util::profiler>   profiler)
  : yc::util::named_thread("renderer")
  , app(app)
  , profiler(std::move(profiler))
  {
    int max_fps = 0;
    app->render_data.window->get_max_fps(max_fps);
    keeper = yc::app::frequency_keeper(max_fps);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}