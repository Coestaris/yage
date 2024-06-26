/** ========================================================================
 *
 * @file renderer.cpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <fstream>

#include "renderer.hpp"
#include "core/time.hpp"
#include "graphics/shader_compiler.hpp"
#include "platform/graphics/platform.hpp"
#include "app/application.hpp"
#include "app/assets/shader.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yage::app::threads::renderer::run()
{
  if (app->subrenderer->window->open() != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to open window");
    return;
  }

  graphics = app->subrenderer->window->get_graphics();
  if (!graphics)
  {
    log(LOG_ERROR, "Unable to get graphics from window");
    return;
  }

  yage::core::error error;
  if ((error = app->subrenderer->asset_factory->read_assets(graphics))
      != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to read assets: %d", error);
    app->subrenderer->window->close();
    return;
  }

  app->subrenderer->before_loop();

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
      log(LOG_DEBUG, "Received signal: %d", signal);
      app->subrenderer->window->close();
    }
    if (!app->subrenderer->window->is_open())
    {
      log(LOG_DEBUG, "Window is closed. Exiting renderer worker");
      break;
    }

    graphics->clear();
    app->subrenderer->loop();
    instances_count = app->subrenderer->objects.size();
    graphics->flush();

    /* Stage 3: Sync with updater after frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_END);
    app->common_data.post_frame_sync.wait();

    /* Stage 4: Copy data from updater to renderer (for next frame) */
    app->subrenderer->objects.clear();
    for (auto & object : app->common_data.objects)
    {
      app->subrenderer->objects.push_back(object->data);
    }

    /* Stage 5: Wait for vsync */
    profiler->passed(PROF_POINT_RENDERER_UPDATE_START);
    if (app->subrenderer->window->update() != yage::core::error::OK)
    {
      log(LOG_ERROR, "Failed to update window");
      break;
    }

    keeper.call();
  }
}