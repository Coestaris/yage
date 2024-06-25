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
#include "core/os_events.hpp"
#include "core/util/sync_point.hpp"
#include "core/util/profiler.hpp"
#include "graphics/window.hpp"
#include "app/object.hpp"
#include "app/input_manager.hpp"
#include "app/assets/asset_factory.hpp"

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

class application
{
public:
  ~application()
  {
    delete render_func;
  }

  yage::app::assets::asset_factory * asset_factory;

  /* copy for logic */
  std::vector<yage::app::object *> objects_logic;
  /* copy for renderer */
  std::vector<yage::app::object::public_data> objects_renderer;

  yage::core::util::sync_point pre_frame_sync { 2 };
  yage::core::util::sync_point post_frame_sync { 2 };

  yage::core::os_events *  os_events;
  yage::graphics::window * window;

  yage::app::input_manager input;

  class custom_render_func
  {
  protected:
    application * app;

  public:
    explicit custom_render_func(application * app)
    : app(app)
    {
    }
    virtual ~custom_render_func() = default;
    virtual void before_loop()    = 0;
    virtual void loop()           = 0;
  } * render_func;

  application(
        yage::app::assets::asset_factory * asset_factory,
        yage::graphics::window *           window,
        yage::core::os_events *            os_events)
  : asset_factory(asset_factory)
  , os_events(os_events)
  , window(window)
  , input(&window->input)
  {
  }

  yage::core::error spawn_threads();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}