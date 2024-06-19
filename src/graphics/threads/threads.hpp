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
#include "graphics/game_object.hpp"
#include "core/os_events.hpp"

namespace yage::graphics::threads
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

struct open_window_arguments
{
  yage::core::os_events *        os_events;
  yage::graphics::window *       window;
  const std::string              title;
  const yage::graphics::geometry geometry;
  const int                      max_fps;
};

/* Variables ================================================================ */
extern std::vector<yage::graphics::game_object* > game_objects_1; /* copy for logic */
extern std::vector<yage::graphics::game_object::public_data> game_objects_2; /* copy for renderer */

/* Prototypes =============================================================== */
void spawn_threads(open_window_arguments & args);

}