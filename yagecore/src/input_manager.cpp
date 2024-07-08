/** ========================================================================
 *
 * @file input_manager.cpp
 * @date 20-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/app/input_manager.hpp"
#include "yagecore/keyboard.hpp"
#include "yagecore/mouse.hpp"
#include "yagecore/graphics/input_events.hpp"
#include "yagecore/graphics/window.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
bool yc::app::input_manager::key_pressed(yc::key key) const
{
  return key_states[static_cast<size_t>(key)];
}

bool yc::app::input_manager::mouse_pressed(yc::mouse button) const
{
  return mouse_states[static_cast<size_t>(button)];
}

glm::vec2 yc::app::input_manager::mouse_position() const
{
  return mouse_pos;
}

void yc::app::input_manager::update()
{
  yc::key key;
  while (key_pressed_listener.get_next(key))
  {
    key_states[static_cast<size_t>(key)] = true;
  }

  while (key_released_listener.get_next(key))
  {
    key_states[static_cast<size_t>(key)] = false;
  }

  yc::graphics::mouse_click_data mouse_data {};
  while (mouse_pressed_listener.get_next(mouse_data))
  {
    mouse_states[static_cast<size_t>(mouse_data.button)] = true;
    mouse_pos = { mouse_data.x, mouse_data.y };
  }

  while (mouse_released_listener.get_next(mouse_data))
  {
    mouse_states[static_cast<size_t>(mouse_data.button)] = false;
    mouse_pos = { mouse_data.x, mouse_data.y };
  }

  yc::graphics::mouse_moved_data mouse_moved_data {};
  while (mouse_moved_listener.get_next(mouse_moved_data))
  {
    mouse_pos = { mouse_moved_data.x, mouse_moved_data.y };
  }
}

yc::app::input_manager::input_manager(
      const std::shared_ptr<yc::graphics::window>& window)
{
  this->window = window;

  window->input.key_pressed.add_listener(&key_pressed_listener);
  window->input.key_released.add_listener(&key_released_listener);
  window->input.mouse_pressed.add_listener(&mouse_pressed_listener);
  window->input.mouse_moved.add_listener(&mouse_moved_listener);
  window->input.mouse_released.add_listener(&mouse_released_listener);
}
