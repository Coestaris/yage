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
#include "core/util/named_thread.hpp"
#include "core/os_events.hpp"
#include "core/fs.hpp"

#include "platform/graphics/platform.hpp"
#include "app/object.hpp"
#include "app/application.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
class movable_obj : public yage::app::object
{
public:
  ~movable_obj() override = default;

  int x_inc = 2;
  int y_inc = 2;

  explicit movable_obj(yage::app::application * app)
  : yage::app::object(app)
  {
    yage::graphics::geometry geometry {};
    if (app->window->get_geometry(geometry) != yage::core::error::OK)
    {
      geometry.width == 800;
      geometry.height == 600;
    }

    data.position = { (float)(rand() % (geometry.width - 200)),
                      (float)(rand() % geometry.height - 200) };
    x_inc         = 2 * (rand() % 2 ? 1 : -1);
    y_inc         = 2 * (rand() % 2 ? 1 : -1);
    data.size     = { 100, 100 };
  }

  void update() override
  {
    yage::graphics::geometry geometry {};
    if (app->window->get_geometry(geometry) != yage::core::error::OK)
    {
      geometry.width == 800;
      geometry.height == 600;
    }

    data.position.x += x_inc;
    data.position.y += y_inc;

    if (data.position.x > geometry.width - data.size.x || data.position.x < 0)
    {
      x_inc *= -1;
    }

    if (data.position.y > geometry.height - data.size.y || data.position.y < 0)
    {
      y_inc *= -1;
    }
  }
};

class controller_obj : public yage::app::object
{
public:
  ~controller_obj() override = default;

  explicit controller_obj(yage::app::application * app)
  : yage::app::object(app)
  {
    data.position = { 100, 100 };
    data.size     = { 100, 100 };
  }

  void update() override
  {
    if (app->input.key_pressed(yage::core::key::ESCAPE))
    {
      // app->window->close();
      exit(0); /* TODO: Fix this */
    }

    data.position = app->input.mouse_position();
  }
};

int main(int argc, char * argv[])
{
  yage::core::os_events events;

  yage::core::stdout_logger logger;
  logger.set_level(LOG_DEBUG);
  set_global_logger(&logger);

  yage::graphics::settings settings {
    .title           = "YAGE",
    .width           = 800,
    .height          = 600,
    .max_fps         = 60,
    .win_mode        = yage::graphics::settings::window_mode::WINDOWED_FIXED,
    .vsync           = true,
    .custom_settings = {},
  };
  yage::graphics::window * window
        = yage::platform::graphics::create_window(settings);

  yage::app::application app(window, &events);

  for (int i = 0; i < 10; i++)
    app.objects_logic.push_back(new movable_obj(&app));
  app.objects_logic.push_back(new controller_obj(&app));

  app.spawn_threads();

  yage::platform::graphics::destroy_window(window);

  for (auto * game_object : app.objects_logic)
  {
    delete game_object;
  }

  return 0;
}