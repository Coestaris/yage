/** ========================================================================
 *
 * @file main.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <fstream>
#include "core/log.hpp"
#include "core/pubsub.hpp"
#include "core/named_thread.hpp"
#include "core/os_events.hpp"
#include "core/fs.hpp"

#include "platform/graphics/platform.hpp"
#include "graphics/threads/threads.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::string resources_directory = "../resources";

int main(int argc, char * argv[])
{
  yage::core::os_events events;

  yage::core::stdout_logger logger;
  logger.set_level(LOG_DEBUG);
  set_global_logger(&logger);

  yage::graphics::settings settings;
  yage::graphics::window * window = yage::platform::graphics::create_window(settings);

  yage::graphics::threads::open_window_arguments args
        = { window, "Hello world", { 0, 0, 800, 600 }, 60 };
  yage::graphics::threads::spawn_threads(args);

  yage::platform::graphics::destroy_window(window);

  return 0;
}