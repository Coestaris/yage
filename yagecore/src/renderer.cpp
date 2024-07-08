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

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yc::app::threads::renderer::run()
{
  if (app->renderer->window->open() != yc::OK)
  {
    logging(LOG_ERROR, "Failed to open window");
    return;
  }

  graphics = app->renderer->window->get_graphics();
  if (!graphics)
  {
    logging(LOG_ERROR, "Unable to get graphics from window");
    return;
  }

  yc::error error;
  if ((error = app->renderer->asset_factory->read_assets(graphics))
      != yc::OK)
  {
    logging(LOG_ERROR, "Failed to read assets: %d", error);
    app->renderer->window->close();
    return;
  }

  app->renderer->before_loop();

  while (true)
  {
    /* Stage 1: Sync with updater before frame */
    profiler->start_cycle();
    profiler->passed(PROF_POINT_RENDERER_PRE_FRAME);
    app->common_data.pre_frame_sync.wait();

    /* Stage 2: Draw the frame using data from previous frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_START);
    int signal = 0;
    if (stop_signal_listener.get_next(signal))
    {
      logging(LOG_DEBUG, "Received signal: %d", signal);
      app->renderer->window->close();
    }
    if (!app->renderer->window->is_open())
    {
      logging(LOG_DEBUG, "Window is closed. Exiting renderer worker");
      break;
    }

    graphics->clear();
    app->renderer->loop();
    instances_count = app->renderer->objects.size();
    graphics->flush();

    /* Stage 3: Sync with updater after frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_END);
    app->common_data.post_frame_sync.wait();

    /* Stage 4: Copy data from updater to renderer (for next frame) */
    app->renderer->objects.clear();
    for (auto & object : app->common_data.objects)
    {
      if (object->data.visible)
      {
        app->renderer->objects.push_back(object->data.primitive);
      }
    }

    /* Stage 5: Wait for vsync */
    profiler->passed(PROF_POINT_RENDERER_UPDATE_START);
    if (app->renderer->window->update() != yc::OK)
    {
      logging(LOG_ERROR, "Failed to update window");
      break;
    }

    keeper.call();
  }
}