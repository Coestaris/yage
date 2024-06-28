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
#include "app/threads/statistics.hpp"
#include "core/util/sync_point.hpp"
#include "core/util/profiler.hpp"

#include <barrier>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error yage::app::application::start()
{
  /*
   * Threading model:
   *           Sync                                     Sync
   * Renderer:   ||  Start drawing scene   |              || Copy objects data
   * Logic:      ||  Process input | Update objects | etc ||
   */
  yage::core::util::simple_profiler renderer_profiler;
  yage::app::threads::renderer      renderer(this, &renderer_profiler);

  yage::core::util::simple_profiler logic_profiler;
  yage::app::threads::logic         logic(this, &logic_profiler);

  yage::app::threads::statistics statistics(&logic, &renderer);

  os_events->on_stop_signal.add_listener(&renderer.stop_signal_listener);

  logic.start();
  renderer.start();
  statistics.start();

  renderer.join();
  logging(LOG_INFO, "Renderer thread has finished");

  /* Sync points are no longer needed.
   * Shutdown them to avoid deadlocks */
  common_data.pre_frame_sync.shutdown();
  common_data.post_frame_sync.shutdown();

  /* Ask logic/statistics thread to politely close */
  logic.close();
  statistics.close();

  logic.join();
  statistics.join();

  logging(LOG_INFO, "Logic thread has finished");

  renderer_profiler.flush();
  logic_profiler.flush();

  return yage::core::error::OK;
}