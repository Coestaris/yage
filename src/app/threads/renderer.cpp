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
  if (app->window->open() != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to open window");
    return;
  }

  graphics = app->window->get_graphics();
  if (!graphics)
  {
    log(LOG_ERROR, "Unable to get graphics from window");
    return;
  }

  yage::core::error error;
  if ((error = app->asset_factory->read_assets(graphics))
      != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to read assets: %d", error);
    app->window->close();
    return;
  }
  yage::app::assets::shader * shader
        = app->asset_factory->get_shader("triangle");
  if (shader->load() != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to load shader");
    app->window->close();
    return;
  }
  auto * program = shader->get_program();

  while (true)
  {
    /* Stage 1: Sync with updater before frame */
    profiler->start_cycle();
    profiler->passed(PROF_POINT_RENDERER_PRE_FRAME);
    app->pre_frame_sync.wait();

    /* Stage 2: Draw the frame using data from previous frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_START);
    int signal = 0;
    if (stop_signal_listener.get_next(signal))
    {
      log(LOG_DEBUG, "Received signal: %d", signal);
      app->window->close();
    }
    if (!app->window->is_open())
    {
      log(LOG_DEBUG, "Window is closed. Exiting renderer worker");
      break;
    }
    if (!shader->is_live())
    {
      shader->unload();
      shader->load();
      program = shader->get_program();
    }

    graphics->clear();
    for (auto & public_data : app->objects_renderer)
    {
      graphics->draw_rect(public_data.position, public_data.size);
    }
    instances_count = app->objects_renderer.size();
    graphics->flush();

    /* Stage 3: Sync with updater after frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_END);
    app->post_frame_sync.wait();

    /* Stage 4: Copy data from updater to renderer (for next frame) */
    app->objects_renderer.clear();
    for (auto & object : app->objects_logic)
    {
      app->objects_renderer.push_back(object->data);
    }

    /* Stage 5: Wait for vsync */
    profiler->passed(PROF_POINT_RENDERER_UPDATE_START);
    if (app->window->update() != yage::core::error::OK)
    {
      log(LOG_ERROR, "Failed to update window");
      break;
    }

    keeper.call();
  }
}