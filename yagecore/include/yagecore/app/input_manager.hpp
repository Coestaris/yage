/** ========================================================================
 *
 * @file input_manager.hpp
 * @date 20-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "glm/vec2.hpp"

#include "yagecore/graphics/input_events.hpp"
#include "yagecore/keyboard.hpp"
#include "yagecore/mouse.hpp"
#include "yagecore/graphics/window.hpp"

namespace yc::app
{
/* Defines ================================================================== */

/* Macros =================================================================== */
/* Types ==================================================================== */
class input_manager
{
private:
  std::shared_ptr<yc::graphics::window> window;

  bool key_states[static_cast<size_t>(yc::key::MAX_KEYS)]        = { false };
  bool mouse_states[static_cast<size_t>(yc::mouse::MAX_BUTTONS)] = { false };
  glm::vec2 mouse_pos {};

  yc::graphics::key_pressed_listener    key_pressed_listener;
  yc::graphics::key_released_listener   key_released_listener;
  yc::graphics::mouse_pressed_listener  mouse_pressed_listener;
  yc::graphics::mouse_moved_listener    mouse_moved_listener;
  yc::graphics::mouse_released_listener mouse_released_listener;

public:
  explicit input_manager(const std::shared_ptr<yc::graphics::window> & window);

  [[nodiscard]] bool      key_pressed(yc::key key) const;
  [[nodiscard]] bool      mouse_pressed(yc::mouse button) const;
  [[nodiscard]] glm::vec2 mouse_position() const;

  void update();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}