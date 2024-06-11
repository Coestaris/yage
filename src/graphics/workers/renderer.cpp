/** ========================================================================
 *
 * @file renderer.cpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "renderer.hpp"
#include "core/time.hpp"
#include "core/pubsub.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yage::graphics::workers::renderer::on_event(
      const yage::core::event<int> & event)
{
  log(LOG_DEBUG, "Received stop signal: %d. Closing window", event.get_data());
  window->close();
}

void yage::graphics::workers::renderer::run()
{
  if (window->open(win_title, win_geometry, false) != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to open window");
    return;
  }

  yage::core::lambda_subscriber<yage::graphics::key> key_pressed_handler(
        [&](yage::core::event<yage::graphics::key> event) {
          log(LOG_DEBUG, "Key pressed: %s at %f",
              yage::graphics::key_to_string(event.get_data()).c_str(),
              event.get_time());
        });
  window->get_key_pressed().subscribe(key_pressed_handler);

  yage::core::lambda_subscriber<
        std::pair<yage::graphics::mouse_button, yage::graphics::mouse_position>>
        mouse_pressed_handler(
              [&](yage::core::event<std::pair<
                        yage::graphics::mouse_button,
                        yage::graphics::mouse_position>> event) {
                log(LOG_DEBUG, "Mouse pressed: %s at (%d, %d) at %f",
                    yage::graphics::mouse_button_to_string(
                          event.get_data().first)
                          .c_str(),
                    event.get_data().second.x, event.get_data().second.y,
                    event.get_time());
              });
  window->get_mouse_pressed().subscribe(mouse_pressed_handler);

  yage::graphics::graphics * graphics = window->get_graphics();

  while (true)
  {
    process_events();

    float frame_interval = keeper.call();

    if (!window->is_open())
    {
      log(LOG_DEBUG, "Window is closed. Exiting renderer worker");
      break;
    }

    float draw_start = yage::core::get_ms();
    graphics->clear();
    graphics->draw_rect(0, 0, 800, 600);

    if (window->update() != yage::core::error::OK)
    {
      log(LOG_ERROR, "Failed to update window");
      break;
    }

    /* perform moving average */
    draw_time = draw_time * 0.9f + (yage::core::get_ms() - draw_start) * 0.1f;
  }
}
