/** ========================================================================
 *
 * @file renderer.cpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <fstream>

#include "yagecore/app/threads/renderer.hpp"
#include "yagecore/app/application.hpp"
#include "yagecore/time.hpp"
#include "yagecore/log.hpp"
#include "yagecore/error.hpp"

#include <yagecore/platform.hpp>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
enum
{
  PROF_POINT_RENDERER_PRE_FRAME = 0,
  PROF_POINT_RENDERER_DRAW_START,
  PROF_POINT_RENDERER_DRAW_END,
  PROF_POINT_RENDERER_UPDATE_START,
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::app::threads::renderer::renderer(
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

void yc::app::threads::renderer::run()
{
  if (app->render_data.window->open() != yc::OK)
  {
    logging(LOG_ERROR, "Failed to open window");
    return;
  }

  graphics = app->render_data.window->get_graphics();
  if (!graphics)
  {
    logging(LOG_ERROR, "Unable to get graphics from window");
    return;
  }

  yc::error error;
  if ((error = app->render_data.asset_factory->read_assets(graphics)) != yc::OK)
  {
    logging(LOG_ERROR, "Failed to read assets: %d", error);
    app->render_data.window->close();
    return;
  }

  auto passes
        = app->create_render_chain(graphics, app->render_data.asset_factory);

#if YAGE_COLLECT_DRAW_STATS
  std::vector<size_t> statistcs_passes;
  statistcs_passes.reserve(passes.size());
#endif

  for (const auto & pass : passes)
  {
    pass->before_loop();
#if YAGE_COLLECT_DRAW_STATS
    statistcs_passes.push_back(app->stats.register_pass(pass->get_name()));
#endif
  }

  while (true)
  {
    /* Stage 0.5: Currently we're between two sync points,
     * so it's perfect time to check for signals. */
    if (int signal = 0; stop_signal_listener.get_next(signal))
    {
      logging(LOG_DEBUG, "Received signal: %d", signal);
      app->render_data.window->close();
    }
    if (!app->render_data.window->is_open())
    {
      logging(LOG_DEBUG, "Window is closed. Exiting renderer worker");
      break;
    }

    /* Stage 1: Sync with updater before frame */
    profiler->start_cycle();
    profiler->passed(PROF_POINT_RENDERER_PRE_FRAME);
    app->common_data.pre_frame_sync.wait();

    /* Stage 2: Draw the frame using data from previous frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_START);
    graphics->clear();

    for (auto & pass : passes)
    {
      if (pass->get_skip())
      {
        continue;
      }

      pass->begin();
      if (pass->get_type() == yc::graphics::render_pass::type::OBJECT_PASS)
      {
        for (const auto & object : app->render_data.objects)
        {
          /* Note, that visibility, already accounted,
           * when adding object to the renderer. */
          if (!pass->on_node(object))
          {
            continue;
          }

          pass->record_result(object.node->draw(pass));
        }
      }
      pass->end();
    }
    // graphics->flush();

    /* Stage 3: Sync with updater after frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_END);
    app->common_data.post_frame_sync.wait();

    /* Stage 4: Copy data from updater to renderer (for next frame).
     * At this point, updater doing nothing. So we can safely copy data. */
    app->render_data.objects.clear();
    for (const auto & object : app->common_data.objects)
    {
      /* Stage 4.1: Call on_ready for objects, that are not ready yet.
       * It's important to call on_ready from rendeing thread,
       * sice this thread holds graphics context (that may be critical
       * for some API's. Blinking to Windows). */
      if (!object->renderer_data.renderable.ready)
      {
        object->on_ready(app->render_data.asset_factory);
        object->renderer_data.renderable.ready = true;
      }

      /* Stage 4.2: Add object to the render list */
      if (object->renderer_data.renderable.visible)
      {
        app->render_data.objects.push_back(object->renderer_data.renderable);
      }
    }

    /* Stage 4.3: Poll assets. This will check, if there are newer
     * versions of assets, and if so - reload them. This is
     * important to call from rendering thread too. */
    app->render_data.asset_factory->poll_assets();

#if YAGE_COLLECT_DRAW_STATS
    app->stats.log_draw_time(profiler->get_diff(
          PROF_POINT_RENDERER_DRAW_START, PROF_POINT_RENDERER_DRAW_END));
    app->stats.log_draw_time(profiler->get_diff(
          PROF_POINT_RENDERER_DRAW_END, PROF_POINT_RENDERER_UPDATE_START));
    app->stats.log_fps(keeper.get_cps());
    for (size_t i = 0; i < passes.size(); i++)
    {
      yc::app::statistics::pass_info pass_info
            = { .draw_time       = passes[i]->get_draw_time(),
                .drawn_meshes    = passes[i]->get_drawn_meshes(),
                .drawn_triangles = passes[i]->get_drawn_triangles() };
      app->stats.log_pass(statistcs_passes[i], pass_info);
    }
#endif

    /* Stage 5: Wait for vsync */
    profiler->passed(PROF_POINT_RENDERER_UPDATE_START);
    if (app->render_data.window->update() != yc::OK)
    {
      logging(LOG_ERROR, "Failed to update window");
      break;
    }

    keeper.call();
  }
}