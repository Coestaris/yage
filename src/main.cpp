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
#include "app/assets/pack/asset_factory.hpp"

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

  float x_inc = 2;
  float y_inc = 2;

  explicit movable_obj(
        yage::app::application * app, yage::core::float2 start_pos)
  : yage::app::object(app)
  {
    data.position = start_pos;
    x_inc         = 0.1 * (rand() % 2 ? 1 : -1);
    y_inc         = 0.1 * (rand() % 2 ? 1 : -1);
    data.size     = { 100, 100 };
  }

  void update(yage::core::milliseconds delta) override
  {
    yage::graphics::geometry geometry {};
    if (app->window->get_geometry(geometry) != yage::core::error::OK)
    {
      geometry.width == 800;
      geometry.height == 600;
    }

    auto mouse_pos = app->input.mouse_position();
    if (app->input.mouse_pressed(yage::core::mouse::LEFT)
        && mouse_pos.x > data.position.x && mouse_pos.x < data.position.x + 100
        && mouse_pos.y > data.position.y && mouse_pos.y < data.position.y + 100)
    {
      destroy();
      return;
    }

    data.position.x += x_inc * delta;
    data.position.y += y_inc * delta;

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
  yage::graphics::mouse_pressed_listener mouse_pressed_listener;

  explicit controller_obj(yage::app::application * app)
  : yage::app::object(app)
  {
    data.position = { 100, 100 };
    data.size     = { 100, 100 };

    app->window->input.mouse_pressed.add_listener(&mouse_pressed_listener);
  }

  ~controller_obj() override
  {
    app->window->input.mouse_pressed.remove_listener(&mouse_pressed_listener);
  }

  void update(yage::core::milliseconds delta) override
  {
    if (app->input.key_pressed(yage::core::key::ESCAPE))
    {
      app->window->close();
    }

    yage::graphics::mouse_click_data click_data {};
    if (mouse_pressed_listener.get_next(click_data))
    {
      if (click_data.button == yage::core::mouse::RIGHT)
      {
        app->objects_logic.push_back(new movable_obj(app, data.position));
      }
    }

    data.position = app->input.mouse_position();
  }
};

std::string lookup_asset_path()
{
  std::vector<std::string> variants = {
    "assets", "../assets", "../../assets", "assets/assets", "../assets/assets",
  };
  std::vector<std::string> extensions = { ".pack", ".zip" };

  for (const auto & variant : variants)
  {
    for (const auto & extension : extensions)
    {
      std::string path = variant + extension;
      log(LOG_DEBUG, "Checking for %s", path.c_str());
      if (yage::core::file_exists(path))
      {
        return path;
      }
    }
  }

  return {};
}

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

  std::string asset_path = lookup_asset_path();
  if (asset_path.empty())
  {
    log(LOG_ERROR, "Failed to find assets.pack file");
    return 1;
  }
  yage::app::assets::pack::asset_factory asset_factory(
        asset_path, yage::core::get_temp_dir("yagecache"));

  yage::app::application app(&asset_factory, window, &events);

  for (int i = 0; i < 10; i++)
  {
    yage::core::float2 position
          = { static_cast<float>(rand() % (settings.width - 200)),
              static_cast<float>(rand() % settings.height - 200) };

    app.objects_logic.push_back(new movable_obj(&app, position));
  }
  app.objects_logic.push_back(new controller_obj(&app));

  app.spawn_threads();

  yage::platform::graphics::destroy_window(window);

  for (auto * game_object : app.objects_logic)
  {
    delete game_object;
  }

  return 0;
}