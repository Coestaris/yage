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

#if COLLECT_DRAW_STATISTICS
  /** Amount of drawables that CPU wants us to draw */
  size_t drawables_cpu = 0;
  /** Amount of drawables that actually was drawn (at previous frame) */
  size_t drawables_gpu = 0;
  /** Amount of meshes drawn (at previous frame) */
  size_t drawn_meshes = 0;
  /** Amount of triangles drawn (at previous frame) */
  size_t drawn_triangles = 0;
#endif

public:
  yc::signal_event_listener stop_signal_listener;

  void run() override;

#if COLLECT_DRAW_STATISTICS
  size_t get_drawables_cpu() const
  {
    return drawables_cpu;
  }
  size_t get_drawables_gpu() const
  {
    return drawables_gpu;
  }
  size_t get_drawn_meshes() const
  {
    return drawn_meshes;
  }
  size_t get_drawn_triangles() const
  {
    return drawn_triangles;
  }
#endif

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