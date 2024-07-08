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

#include "yagecore/os_events.hpp"
#include "yagecore/util/sync_point.hpp"
#include "yagecore/util/profiler.hpp"
#include "yagecore/graphics/window.hpp"
#include "yagecore/app/object.hpp"
#include "yagecore/app/input_manager.hpp"
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

class application
{
private:
  yc::os_events *        os_events;
  yc::app::input_manager input;

public:
  class common_data
  {
  private:
    std::vector<yc::app::object *> objects;

    yc::util::sync_point pre_frame_sync { 2 };
    yc::util::sync_point post_frame_sync { 2 };

    friend class yc::app::threads::logic;
    friend class yc::app::threads::renderer;
    friend class application;

  } common_data;

  class logic_data
  {
  private:
    std::vector<yc::app::object *> object_to_add;
    std::mutex                     object_to_add_mutex;

    friend class yc::app::threads::logic;
    friend class application;

  } logic_data;

  /* acts like renderer data,
   * and additionally used to perform custom rendering */
  class subrenderer_
  {
  protected:
    application *                     app;
    yc::app::asset_factory *          asset_factory;
    yc::graphics::window *            window;
    std::vector<yc::graphics::node *> objects;

    friend class yc::app::threads::renderer;
    friend class application;

    virtual ~subrenderer_() = default;

    subrenderer_(
          yc::app::asset_factory * asset_factory, yc::graphics::window * window)
    : asset_factory(asset_factory)
    , window(window) {};

    virtual void before_loop() = 0;
    virtual void loop()        = 0;

  } * subrenderer;

  application(
        yc::app::application::subrenderer_ * subrenderer,
        yc::graphics::input_events *         input_events,
        yc::os_events *                      os_events)
  : os_events(os_events)
  , input(input_events)
  , subrenderer(subrenderer)
  {
    subrenderer->app = this;
  }

  yc::error start();

  void stop()
  {
    subrenderer->window->close();
  }

  void add_object(yc::app::object * object)
  {
    std::lock_guard<std::mutex> lock(logic_data.object_to_add_mutex);
    logic_data.object_to_add.push_back(object);
  }

  void destroy_object(yc::app::object * object, bool hard = false)
  {
    if (hard)
    {
      auto it = std::find(
            common_data.objects.begin(), common_data.objects.end(), object);
      if (it != common_data.objects.end())
      {
        common_data.objects.erase(it);
      }

      delete object;
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
      if (hard)
      {
        delete object;
      }
      else
      {
        object->logic_data.should_be_deleted = true;
      }
    }

    if (hard)
    {
      common_data.objects.clear();
    }
  }

  yc::graphics::geometry get_geometry() const
  {
    yc::graphics::geometry geometry {};
    subrenderer->window->get_geometry(geometry);
    return geometry;
  }

  input_manager & get_input()
  {
    return input;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}