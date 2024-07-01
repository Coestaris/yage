/** ========================================================================
 *
 * @file input_manager.hpp
 * @date 20-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics/input_events.hpp"
#include "core/keyboard.hpp"
#include "core/mouse.hpp"
#include "glm/vec2.hpp"

namespace yage::app
{
/* Defines ================================================================== */

/* Macros =================================================================== */
/* Types ==================================================================== */
class input_manager
{
private:
  yage::graphics::input_events * input;

  bool key_states[static_cast<size_t>(yage::core::key::MAX_KEYS)] = { false };
  bool mouse_states[static_cast<size_t>(yage::core::mouse::MAX_BUTTONS)]
        = { false };
  glm::vec2 mouse_pos;

  yage::graphics::key_pressed_listener    key_pressed_listener;
  yage::graphics::key_released_listener   key_released_listener;
  yage::graphics::mouse_pressed_listener  mouse_pressed_listener;
  yage::graphics::mouse_moved_listener    mouse_moved_listener;
  yage::graphics::mouse_released_listener mouse_released_listener;

public:
  input_manager(yage::graphics::input_events * input);

  bool      key_pressed(yage::core::key key) const;
  bool      mouse_pressed(yage::core::mouse button) const;
  glm::vec2 mouse_position() const;

  void update();

  yage::graphics::input_events * raw_input()
  {
    return input;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}