/** ========================================================================
 *
 * @file main.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <fstream>
#include <csignal>
#include <utility>
#include "yagecore/log.hpp"
#include "yagecore/util/named_thread.hpp"
#include "yagecore/os_events.hpp"
#include "yagecore/fs.hpp"
#include "yagecore/platform.hpp"
#include "yagecore/app/object.hpp"
#include "yagecore/app/application.hpp"

#include "yagepack/asset_factory.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
class movable_obj : public yc::app::object
{
public:
  ~movable_obj() override = default;

  explicit movable_obj(yc::app::application * app)
  : yc::app::object(app)
  {
  }

  void update(yc::milliseconds delta) override
  {
  }
};

class subrenderer : public yc::app::application::subrenderer
{
private:
  std::vector<std::shared_ptr<yc::graphics::render_pass>> passes;
  std::shared_ptr<yc::graphics::graphics>                 graphics {};

private:
  void before_loop() override
  {
    graphics = window->get_graphics();
    passes   = yc::trivial_render_chain(graphics, asset_factory);

    for (auto & pass : passes)
    {
      pass->before_loop();
    }
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
      if (pass->get_type() == yc::graphics::pass_type::OBJECT_PASS)
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
        std::shared_ptr<yc::app::asset_factory> asset_factory,
        std::shared_ptr<yc::graphics::window>   window)
  : yc::app::application::subrenderer(
          std::move(asset_factory), std::move(window)) {};
};

std::string lookup_asset_path()
{
  std::vector<std::string> variants = {
    "../../app/assets", "../app/assets", "assets",           "../assets",
    "../../assets",     "assets/assets", "../assets/assets",
  };
  std::vector<std::string> extensions = { ".pack", ".zip" };

  for (const auto & variant : variants)
  {
    for (const auto & extension : extensions)
    {
      std::string path = yc::join_path(variant, "assets" + extension);
      logging(LOG_DEBUG, "Checking for %s", path.c_str());
      if (yc::file_exists(path))
      {
        return path;
      }
    }
  }

  return {};
}

int main(int argc, char * argv[])
{
  auto events = std::make_shared<yc::os_events>();

  yc::stdout_logger logger;
  logger.set_level(LOG_DEBUG);
  set_global_logger(&logger);

  yc::graphics::settings settings {
    .title           = "YAGE",
    .width           = 800,
    .height          = 600,
    .max_fps         = 60,
    .win_mode        = yc::graphics::settings::window_mode::WINDOWED_FIXED,
    .vsync           = true,
    .custom_settings = {},
  };
  auto window = yc::create_window(settings);

  std::string asset_path = lookup_asset_path();
  if (asset_path.empty())
  {
    logging(LOG_ERROR, "Failed to find assets.pack file");
    return 1;
  }
  auto asset_factory = std::make_shared<yagepack::asset_factory>(
        asset_path, yc::get_cache_dir());

  window->set_before_close([&asset_factory]() {
    asset_factory->unload_assets();
    asset_factory->destroy();
  });

  auto renderer = std::make_shared<subrenderer>(asset_factory, window);
  yc::app::application app(renderer, window, events);

  for (int i = 0; i < 10; i++)
  {
    app.add_object(std::make_shared<movable_obj>(&app));
  }

  app.start();
  app.destroy_all_objects(true);

  return 0;
}