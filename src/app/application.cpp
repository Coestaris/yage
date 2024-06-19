/** ========================================================================
 *
 * @file threads.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "application.hpp"
#include "app/threads/renderer.hpp"
#include "app/threads/logic.hpp"
#include "core/util/sync_point.hpp"
#include "core/util/profiler.hpp"

#include <barrier>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error yage::app::application::spawn_threads()
{
  /*
   * Threading model:
   *           Sync                                     Sync
   * Renderer:   ||  Start drawing scene   |              || Copy objects data
   * Logic:      ||  Process input | Update objects | etc ||
   */
  yage::core::util::file_profiler renderer_profiler(10000, "renderer.prof");
  yage::core::util::file_profiler logic_profiler(10000, "logic.prof");

  yage::app::threads::renderer renderer(this, &renderer_profiler);
  yage::app::threads::logic logic(this, &logic_profiler);

  os_events->on_stop_signal.add_listener(&renderer.stop_signal_listener);

  logic.start();
  renderer.start();

  renderer.join();
  log(LOG_INFO, "Renderer thread has finished");

  /* Sync points are no longer needed.
   * Shutdown them to avoid deadlocks */
  pre_frame_sync.shutdown();
  post_frame_sync.shutdown();

  /* Ask logic thread to politely close */
  logic.close();

  logic.join();
  log(LOG_INFO, "Logic thread has finished");

  renderer_profiler.flush();
  logic_profiler.flush();

  return yage::core::error::OK;
}