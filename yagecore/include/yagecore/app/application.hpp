/** ========================================================================
 *
 * @file threads.hpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <algorithm>
#include <utility>

#include "yagecore/os_events.hpp"
#include "yagecore/util/sync_point.hpp"
#include "yagecore/util/profiler.hpp"
#include "yagecore/graphics/window.hpp"
#include "yagecore/app/object.hpp"
#include "yagecore/app/input_manager.hpp"
#include "yagecore/app/statistics.hpp"
#include "yagecore/app/asset_factory.hpp"

namespace yc::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
enum profiler_points
{
  PROF_POINT_RENDERER_PRE_FRAME = 0,
  PROF_POINT_RENDERER_DRAW_START,
  PROF_POINT_RENDERER_DRAW_END,
  PROF_POINT_RENDERER_UPDATE_START,
  PROF_POINT_LOGIC_PRE_FRAME,
  PROF_POINT_LOGIC_INPUT,
  PROF_POINT_LOGIC_UPDATE_OBJECTS,
  PROF_POINT_LOGIC_UPDATE_PARTICLES,
  PROF_POINT_LOGIC_UPDATE_PHYSICS,
  PROF_POINT_LOGIC_UPDATE_AI,
  PROF_POINT_LOGIC_UPDATE_SOUND,
  PROF_POINT_LOGIC_POST_FRAME,
};

namespace threads
{
class logic;
class renderer;
}

class application : public std::enable_shared_from_this<application>
{
private:
  std::shared_ptr<yc::os_events>        os_events;
  std::shared_ptr<yc::graphics::window> window;
  std::shared_ptr<input_manager>        input_mgr;

public:
  class common_data
  {
  private:
    std::vector<std::shared_ptr<yc::app::object>> objects;

    yc::util::sync_point pre_frame_sync { 2 };
    yc::util::sync_point post_frame_sync { 2 };

    friend class yc::app::threads::logic;
    friend class yc::app::threads::renderer;
    friend class application;

  } common_data;

  class logic_data
  {
  private:
    std::vector<std::shared_ptr<yc::app::object>> object_to_add;
    std::mutex                                    object_to_add_mutex;

    friend class yc::app::threads::logic;
    friend class application;

  } logic_data;

  /* acts like renderer data,
   * and additionally used to perform custom rendering */
  class render_data
  {
  protected:
    std::shared_ptr<yc::app::asset_factory>  asset_factory;
    std::shared_ptr<yc::graphics::window>    window;
    std::vector<yc::app::object::renderable> objects;

    friend class yc::app::threads::renderer;
    friend class application;

    render_data(
          std::shared_ptr<yc::app::asset_factory> asset_factory,
          std::shared_ptr<yc::graphics::window>   window)
    : asset_factory(std::move(asset_factory))
    , window(std::move(window)) {};
  } render_data;

  yc::app::statistics stats;

  application(
        std::shared_ptr<yc::app::asset_factory>       asset_factory,
        const std::shared_ptr<yc::graphics::window> & window,
        std::shared_ptr<yc::os_events>                os_events)
  : os_events(std::move(os_events))
  , window(window)
  , render_data(std::move(asset_factory), window)
  {
    input_mgr = std::make_shared<input_manager>(this->window);
  }

  yc::error start();

  void stop()
  {
    render_data.window->close();
  }

  void add_object(std::shared_ptr<yc::app::object> object)
  {
    std::lock_guard<std::mutex> lock(logic_data.object_to_add_mutex);
    logic_data.object_to_add.push_back(std::move(object));
  }

  void destroy_object(
        const std::shared_ptr<yc::app::object> & object, bool hard = false)
  {
    if (hard)
    {
      auto it = std::find(
            common_data.objects.begin(), common_data.objects.end(), object);
      if (it != common_data.objects.end())
      {
        common_data.objects.erase(it);
      }
    }
    else
    {
      object->logic_data.should_be_deleted = true;
    }
  }

  void destroy_all_objects(bool hard = false)
  {
    for (auto & object : common_data.objects)
    {
      object->logic_data.should_be_deleted = true;
    }

    if (hard)
    {
      common_data.objects.clear();
    }
  }

  [[nodiscard]] yc::graphics::geometry get_geometry() const
  {
    yc::graphics::geometry geometry {};
    render_data.window->get_geometry(geometry);
    return geometry;
  }

  std::shared_ptr<input_manager> get_input()
  {
    return input_mgr;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}