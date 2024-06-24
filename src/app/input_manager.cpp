/** ========================================================================
 *
 * @file input_manager.cpp
 * @date 20-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "app/input_manager.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
bool yage::app::input_manager::key_pressed(yage::core::key key) const
{
  return key_states[static_cast<size_t>(key)];
}

bool yage::app::input_manager::mouse_pressed(yage::core::mouse button) const
{
  return mouse_states[static_cast<size_t>(button)];
}

yage::core::float2 yage::app::input_manager::mouse_position() const
{
  return mouse_pos;
}

void yage::app::input_manager::update()
{
  yage::core::key key;
  while (key_pressed_listener.get_next(key))
  {
    key_states[static_cast<size_t>(key)] = true;
  }

  while (key_released_listener.get_next(key))
  {
    key_states[static_cast<size_t>(key)] = false;
  }

  yage::graphics::mouse_click_data mouse_data {};
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

  yage::graphics::mouse_moved_data mouse_moved_data {};
  while (mouse_moved_listener.get_next(mouse_moved_data))
  {
    mouse_pos = { mouse_moved_data.x, mouse_moved_data.y };
  }
}

yage::app::input_manager::input_manager(yage::graphics::input_events * input)
{
  this->input = input;

  input->key_pressed.add_listener(&key_pressed_listener);
  input->key_released.add_listener(&key_released_listener);
  input->mouse_pressed.add_listener(&mouse_pressed_listener);
  input->mouse_moved.add_listener(&mouse_moved_listener);
  input->mouse_released.add_listener(&mouse_released_listener);
}
