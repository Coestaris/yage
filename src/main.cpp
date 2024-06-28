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

  explicit movable_obj(yage::app::application * app)
  : yage::app::object(app)
  {
  }

  void update(yage::core::milliseconds delta) override
  {
  }
};

class subrenderer : public yage::app::application::subrenderer_
{
private:
  std::vector<yage::graphics::render_pass *> passes;
  yage::graphics::graphics *                 graphics {};

private:
  void before_loop() override
  {
    graphics = window->get_graphics();
    passes   = yage::platform::graphics::trivial_render_chain(graphics);
  }

  void loop() override
  {
    asset_factory->poll_assets();

    for (auto & pass : passes)
    {
      if (pass->get_skip())
      {
        continue;
      }

      pass->begin();
      if (pass->get_type() == yage::graphics::pass_type::OBJECT_PASS)
      {
        for (auto & object : objects)
        {
          pass->on_object(object);
        }
      }
      pass->end();
    }
  }

public:
  subrenderer(
        yage::app::assets::asset_factory * asset_factory,
        yage::graphics::window *           window)
  : subrenderer_(asset_factory, window) {};
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
      logging(LOG_DEBUG, "Checking for %s", path.c_str());
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
    logging(LOG_ERROR, "Failed to find assets.pack file");
    return 1;
  }
  yage::app::assets::pack::asset_factory asset_factory(
        asset_path, yage::core::get_cache_dir());

  subrenderer            subrenderer(&asset_factory, window);
  yage::app::application app(&subrenderer, &window->input, &events);

  for (int i = 0; i < 10; i++)
  {
    app.add_object(new movable_obj(&app));
  }

  app.start();

  app.destroy_all_objects(true);

  asset_factory.destroy();
  yage::platform::graphics::destroy_window(window);

  return 0;
}