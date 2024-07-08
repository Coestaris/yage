/** ========================================================================
 *
 * @file threads.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/app/application.hpp"
#include "yagecore/app/threads/renderer.hpp"
#include "yagecore/app/threads/logic.hpp"
#include "yagecore/app/threads/statistics.hpp"
#include "yagecore/util/sync_point.hpp"
#include "yagecore/util/profiler.hpp"
#include "yagecore/error.hpp"

#include <barrier>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::error yc::app::application::start()
{
  /*
   * Threading model:
   *           Sync                                     Sync
   * Renderer:   ||  Start drawing scene   |              || Copy objects data
   * Logic:      ||  Process input | Update objects | etc ||
   */
  yc::util::simple_profiler  renderer_profiler;
  yc::app::threads::renderer renderer(this, &renderer_profiler);

  yc::util::simple_profiler logic_profiler;
  yc::app::threads::logic   logic(this, &logic_profiler);

  yc::app::threads::statistics statistics(&logic, &renderer);

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

  return yc::error::OK;
}