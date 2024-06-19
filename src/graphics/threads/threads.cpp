/** ========================================================================
 *
 * @file threads.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "threads.hpp"
#include "renderer.hpp"
#include "logic.hpp"
#include "core/sync_point.hpp"
#include "core/profiler.hpp"

#include <barrier>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
std::vector<yage::graphics::game_object* > yage::graphics::threads::game_objects_1{}; /* copy for logic */
std::vector<yage::graphics::game_object::public_data> yage::graphics::threads::game_objects_2{}; /* copy for renderer */

/* Shared functions ========================================================= */
/*
 * Threading model:
 *           Sync                                     Sync
 * Renderer:   ||  Start drawing scene   |              || Copy objects data
 * Logic:      ||  Process input | Update objects | etc ||
 */
void yage::graphics::threads::spawn_threads(
      yage::graphics::threads::open_window_arguments & args)
{
  yage::core::sync_point pre_frame_sync(2);
  yage::core::sync_point post_frame_sync(2);

  yage::core::file_profiler renderer_profiler(10000, "renderer.prof");
  yage::core::file_profiler logic_profiler(10000, "logic.prof");

  yage::graphics::threads::renderer renderer(
        &renderer_profiler, &pre_frame_sync, &post_frame_sync, args);

  args.os_events->on_stop_signal.add_listener(&renderer.stop_signal_listener);

  yage::graphics::threads::logic logic(
        &logic_profiler, &pre_frame_sync, &post_frame_sync, args.window);

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
}