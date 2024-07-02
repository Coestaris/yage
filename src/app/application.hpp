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

#include "core/os_events.hpp"
#include "core/util/sync_point.hpp"
#include "core/util/profiler.hpp"
#include "graphics/window.hpp"
#include "app/object.hpp"
#include "app/input_manager.hpp"
#include "app/asset_factory.hpp"

namespace yage::app
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
  yage::core::os_events *  os_events;
  yage::app::input_manager input;

public:
  class common_data
  {
  private:
    std::vector<yage::app::object *> objects;

    yage::core::util::sync_point pre_frame_sync { 2 };
    yage::core::util::sync_point post_frame_sync { 2 };

    friend class yage::app::threads::logic;
    friend class yage::app::threads::renderer;
    friend class application;

  } common_data;

  class logic_data
  {
  private:
    std::vector<yage::app::object *> object_to_add;
    std::mutex                       object_to_add_mutex;

    friend class yage::app::threads::logic;
    friend class application;

  } logic_data;

  /* acts like renderer data,
   * and additionally used to perform custom rendering */
  class subrenderer_
  {
  protected:
    application *                                   app;
    yage::app::asset_factory *                      asset_factory;
    yage::graphics::window *                        window;
    std::vector<yage::graphics::render_primitive *> objects;

    friend class yage::app::threads::renderer;
    friend class application;

    virtual ~subrenderer_() = default;

    subrenderer_(
          yage::app::asset_factory * asset_factory,
          yage::graphics::window *   window)
    : asset_factory(asset_factory)
    , window(window) {};

    virtual void before_loop() = 0;
    virtual void loop()        = 0;

  } * subrenderer;

  application(
        yage::app::application::subrenderer_ * subrenderer,
        yage::graphics::input_events *         input_events,
        yage::core::os_events *                os_events)
  : os_events(os_events)
  , input(input_events)
  , subrenderer(subrenderer)
  {
    subrenderer->app = this;
  }

  yage::core::error start();

  void stop()
  {
    subrenderer->window->close();
  }

  void add_object(yage::app::object * object)
  {
    std::lock_guard<std::mutex> lock(logic_data.object_to_add_mutex);
    logic_data.object_to_add.push_back(object);
  }

  void destroy_object(yage::app::object * object, bool hard = false)
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

  yage::graphics::geometry get_geometry() const
  {
    yage::graphics::geometry geometry {};
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