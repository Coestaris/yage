/** ========================================================================
 *
 * @file input_streams.hpp
 * @date 19-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "yagecore/util/event_stream.hpp"
#include "yagecore/keyboard.hpp"
#include "yagecore/mouse.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
struct mouse_moved_data
{
  float x;
  float y;
};

struct mouse_click_data
{
  yc::mouse button;
  float     x;
  float     y;
};

struct mouse_scroll_data
{
  float amount_x;
  float amount_y;
};

using key_pressed_stream    = yc::util::event_stream<yc::key>;
using key_released_stream   = yc::util::event_stream<yc::key>;
using char_entered_stream   = yc::util::event_stream<wchar_t>;
using mouse_moved_stream    = yc::util::event_stream<mouse_moved_data>;
using mouse_pressed_stream  = yc::util::event_stream<mouse_click_data>;
using mouse_released_stream = yc::util::event_stream<mouse_click_data>;
using mouse_scrolled_stream = yc::util::event_stream<mouse_scroll_data>;

using key_pressed_listener  = yc::util::event_stream_listener<yc::key>;
using key_released_listener = yc::util::event_stream_listener<yc::key>;
using char_entered_listener = yc::util::event_stream_listener<wchar_t>;
using mouse_pressed_listener
      = yc::util::event_stream_listener<mouse_click_data>;
using mouse_moved_listener = yc::util::event_stream_listener<mouse_moved_data>;
using mouse_released_listener
      = yc::util::event_stream_listener<mouse_click_data>;
using mouse_scrolled_listener
      = yc::util::event_stream_listener<mouse_scroll_data>;

struct input_events
{
  key_pressed_stream    key_pressed;
  key_released_stream   key_released;
  char_entered_stream   char_entered;
  mouse_moved_stream    mouse_moved;
  mouse_pressed_stream  mouse_pressed;
  mouse_released_stream mouse_released;
  mouse_scrolled_stream mouse_scrolled;
};

/* Prototypes =============================================================== */
/* Variables ================================================================ */

}