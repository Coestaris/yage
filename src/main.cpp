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
class movable_game_object : public yage::graphics::game_object
{
public:
  int x_inc = 2;
  int y_inc = 2;

  movable_game_object()
  {
    data.position = { (float)(rand() % 500), (float)(rand() % 500) };
    x_inc         = 2 * (rand() % 2 ? 1 : -1);
    y_inc         = 2 * (rand() % 2 ? 1 : -1);
    data.size     = { 100, 100 };
  }

  void update() override
  {
    data.position.x += x_inc;
    data.position.y += y_inc;

    if (data.position.x > 800 - data.size.x || data.position.x < 0)
    {
      x_inc *= -1;
    }

    if (data.position.y > 600 - data.size.y || data.position.y < 0)
    {
      y_inc *= -1;
    }
  }
};

class constant_game_object : public yage::graphics::game_object
{
public:
  constant_game_object()
  {
    data.position = { 100, 100 };
    data.size     = { 100, 100 };
  }

  void update() override
  {
  }
};

int main(int argc, char * argv[])
{
  yage::core::os_events events;

  yage::core::stdout_logger logger;
  logger.set_level(LOG_DEBUG);
  set_global_logger(&logger);

  yage::graphics::settings settings;
  yage::graphics::window * window
        = yage::platform::graphics::create_window(settings);

  /* WARNING: There's a huge memory leak */
  for (int i = 0; i < 10; i++)
    yage::graphics::threads::game_objects_1.push_back(new movable_game_object);

  yage::graphics::threads::open_window_arguments args
        = { &events, window, "Hello world", { 0, 0, 800, 600 }, 60 };
  yage::graphics::threads::spawn_threads(args);

  yage::platform::graphics::destroy_window(window);

  return 0;
}