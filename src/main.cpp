/** ========================================================================
 *
 * @file main.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "core/log.hpp"
#include "core/pubsub.hpp"
#include "core/threads.hpp"
#include "core/os_events.hpp"

#include "platform/graphics/platform.hpp"
#include "graphics/workers/renderer.hpp"
#include "graphics/workers/statistics.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
int main(int argc, char * argv[])
{
  yage::core::os_events events;

  yage::core::stdout_logger logger;
  logger.set_level(LOG_DEBUG);
  set_global_logger(&logger);

  yage::core::thread_pool pool(3);
  set_global_thread_pool(&pool);

  yage::graphics::settings settings;
  yage::graphics::window * window
        = yage::platform::graphics::create_window(settings);

  std::string              title    = "Hello, World!";
  yage::graphics::geometry geometry = { 0, 0, 800, 600 };
  int                      max_fps  = 60;

  yage::graphics::workers::renderer renderer(window, title, geometry, max_fps);
  yage::graphics::workers::statistic_worker statistic(&renderer);

  events.on_stop_signal.subscribe(renderer);

  pool.add_task(renderer);
  pool.add_task(statistic);

  pool.wait_all();

  return 0;
}