/** ========================================================================
 *
 * @file window.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>

#include "graphics/image.hpp"
#include "graphics/graphics.hpp"
#include "graphics/keyboard.hpp"

#include "core/error.hpp"
#include "core/pubsub.hpp"
#include "core/io/mouse.hpp"
#include "core/io/keyboard.hpp"

namespace yage::graphics
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class settings
{
};

struct geometry
{
  int x;
  int y;
  int width;
  int height;
};

struct mouse_moved_data
{
  float x;
  float y;
};

struct mouse_click_data
{
  yage::core::io::mouse button;
  float                 x;
  float                 y;
};

struct mouse_scroll_data
{
  float amount_x;
  float amount_y;
};

using key_pressed_publisher    = yage::core::publisher<yage::core::io::key>;
using key_released_publisher   = yage::core::publisher<yage::core::io::key>;
using char_entered_publisher   = yage::core::publisher<wchar_t>;
using mouse_moved_publisher    = yage::core::publisher<mouse_moved_data>;
using mouse_pressed_publisher  = yage::core::publisher<mouse_click_data>;
using mouse_released_publisher = yage::core::publisher<mouse_click_data>;
using mouse_scrolled_publisher = yage::core::publisher<mouse_scroll_data>;
using mouse_double_clicked_publisher = yage::core::publisher<mouse_click_data>;
using window_closed_publisher        = yage::core::publisher<void>;
using window_resized_publisher
      = yage::core::publisher<yage::graphics::geometry>;

class window
{
protected:
  key_pressed_publisher          key_pressed;
  key_released_publisher         key_released;
  char_entered_publisher         char_entered;
  mouse_moved_publisher          mouse_moved;
  mouse_pressed_publisher        mouse_pressed;
  mouse_released_publisher       mouse_released;
  mouse_scrolled_publisher       mouse_scrolled;
  mouse_double_clicked_publisher mouse_double_clicked;
  window_closed_publisher        window_closed;
  window_resized_publisher       window_resized;

public:
  window(settings & settings)
  {
  }
  virtual yage::core::error
  open(std::string & title, geometry & geometry, bool fullscreen)
        = 0;
  virtual yage::core::error          update()       = 0;
  virtual yage::core::error          close()        = 0;
  virtual bool                       is_open()      = 0;
  virtual yage::graphics::graphics * get_graphics() = 0;

#define __DECLARE_SEMI_VIRTUAL_METHODS(name, ...)                              \
  virtual yage::core::error name(__VA_ARGS__)                                  \
  {                                                                            \
    return yage::core::error::NOT_IMPLEMENTED;                                 \
  }

  __DECLARE_SEMI_VIRTUAL_METHODS(set_fullscreen, bool fullscreen)
  __DECLARE_SEMI_VIRTUAL_METHODS(set_vsync, bool vsync)
  __DECLARE_SEMI_VIRTUAL_METHODS(set_icon, image & icon)
  __DECLARE_SEMI_VIRTUAL_METHODS(set_cursor, image & cursor)
  __DECLARE_SEMI_VIRTUAL_METHODS(set_geometry, geometry & geometry)
  __DECLARE_SEMI_VIRTUAL_METHODS(set_title, char * title)

#define __DECLARE_EXPORT_EVENT_METHODS(event)                                  \
  event##_publisher & get_##event()                                            \
  {                                                                            \
    return event;                                                              \
  }

  __DECLARE_EXPORT_EVENT_METHODS(key_pressed)
  __DECLARE_EXPORT_EVENT_METHODS(key_released)
  __DECLARE_EXPORT_EVENT_METHODS(char_entered)
  __DECLARE_EXPORT_EVENT_METHODS(mouse_moved)
  __DECLARE_EXPORT_EVENT_METHODS(mouse_pressed)
  __DECLARE_EXPORT_EVENT_METHODS(mouse_released)
  __DECLARE_EXPORT_EVENT_METHODS(mouse_scrolled)
  __DECLARE_EXPORT_EVENT_METHODS(mouse_double_clicked)
  __DECLARE_EXPORT_EVENT_METHODS(window_closed)
  __DECLARE_EXPORT_EVENT_METHODS(window_resized)

#undef __DECLARE_EXPORT_EVENT_METHODS
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}
