/** ========================================================================
 *
 * @file main.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <thread>
#include <csignal>
#include <utility>
#include "core/log.hpp"
#include "core/pubsub.hpp"
#include "core/threads.hpp"
#include "platform/win/window.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class renderer_worker : public yage::core::worker<void>
{
private:
  std::string              win_title;
  yage::graphics::settings win_settings;
  yage::graphics::geometry win_geometry;

  yage::graphics::window * window;

public:
  void run() override
  {
    window = new yage::platform::win::window(win_settings);
    if (window->open(win_title, win_geometry, false) != yage::core::error::OK)
    {
      log(LOG_ERROR, "Failed to open window");
      return;
    }

    while (true)
    {
      if (!window->is_open())
      {
        log(LOG_DEBUG, "Window is closed. Exiting renderer worker");
        break;
      }

      if (window->update() != yage::core::error::OK)
      {
        log(LOG_ERROR, "Failed to update window");
        break;
      }
    }
  }

public:
  renderer_worker(
        yage::graphics::settings settings,
        std::string              title,
        yage::graphics::geometry geometry)
  : yage::core::worker<void>("renderer")
  , win_title(std::move(title))
  , win_settings(std::move(settings))
  , win_geometry(geometry)
  {
  }
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
int main(int argc, char * argv[])
{
  yage::core::stdout_logger logger;
  logger.set_level(LOG_DEBUG);
  set_global_logger(&logger);

  yage::core::thread_pool pool(3);
  set_global_thread_pool(&pool);

  std::string              title    = "Hello, World!";
  yage::graphics::geometry geometry = { 0, 0, 800, 600 };
  yage::graphics::settings settings;
  renderer_worker          renderer(settings, title, geometry);

  pool.add_task(renderer);

  pool.wait_all();

  return 0;
}