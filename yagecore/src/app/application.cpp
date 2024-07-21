/** ========================================================================
 *
 * @file threads.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "../../include/yagecore/app/application.hpp"
#include "../../include/yagecore/app/threads/renderer.hpp"
#include "../../include/yagecore/app/threads/logic.hpp"
#include "../../include/yagecore/util/sync_point.hpp"
#include "../../include/yagecore/util/profiler.hpp"
#include "../../include/yagecore/error.hpp"

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
  auto  renderer_profiler = std::make_shared<yc::util::simple_profiler>();
  auto renderer_thread = std::make_shared<yc::app::threads::renderer>(shared_from_this(), renderer_profiler);

  auto logic_profiler = std::make_shared<yc::util::simple_profiler>();
  auto logic_thread = std::make_shared<yc::app::threads::logic>(shared_from_this(), logic_profiler);

  os_events->on_stop_signal.add_listener(&renderer_thread->stop_signal_listener);

  logic_thread->start();
  renderer_thread->start();

  renderer_thread->join();
  logging(LOG_INFO, "Renderer thread has finished");

  /* Sync points are no longer needed.
   * Shutdown them to avoid deadlocks */
  common_data.pre_frame_sync.shutdown();
  common_data.post_frame_sync.shutdown();

  /* Ask logic/statistics thread to politely close */
  logic_thread->close();
  logic_thread->join();

  logging(LOG_INFO, "Logic thread has finished");

  return yc::error::OK;
}