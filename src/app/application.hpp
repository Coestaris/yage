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
#include "graphics/window.hpp"
#include "app/game_object.hpp"
#include "core/os_events.hpp"
#include "core/util/sync_point.hpp"
#include "core/util/profiler.hpp"

namespace yage::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
enum profiler_points
{
  PROF_POINT_RENDERER_PRE_FRAME,
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
  /* copy for logic */
  std::vector<yage::app::game_object *> game_objects_1;

  /* copy for renderer */
  std::vector<yage::app::game_object::public_data> game_objects_2;

  yage::core::util::sync_point pre_frame_sync { 2 };
  yage::core::util::sync_point post_frame_sync { 2 };

  yage::core::os_events *  os_events;
  yage::graphics::window * window;

  application(
        yage::graphics::window * window, yage::core::os_events * os_events)
  : os_events(os_events)
  , window(window)
  {
  }

  yage::core::error spawn_threads();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}