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
#include "core/io/keyboard.hpp"

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

  yage::core::lambda_subscriber<yage::core::io::key> key_pressed_handler(
        [this](yage::core::event<yage::core::io::key> event) {
          auto key = event.get_data();
          log(LOG_DEBUG, "Key pressed: %s at %f",
              yage::core::io::key_to_string(key).c_str(), event.get_time());

          if (key == yage::core::io::key::LEFT)
          {
            x -= 10.0f;
          }
          else if (key == yage::core::io::key::RIGHT)
          {
            x += 10.0f;
          }
          else if (key == yage::core::io::key::UP)
          {
            y -= 10.0f;
          }
          else if (key == yage::core::io::key::DOWN)
          {
            y += 10.0f;
          }

          if (key == yage::core::io::key::ESCAPE)
          {
            window->close();
          }
        });
  window->get_key_pressed().subscribe(key_pressed_handler);

  yage::core::lambda_subscriber<yage::core::io::key> key_released_handler(
        [&](yage::core::event<yage::core::io::key> event) {
          auto key = event.get_data();
          log(LOG_DEBUG, "Key released: %s at %f",
              yage::core::io::key_to_string(key).c_str(), event.get_time());
        });
  window->get_key_released().subscribe(key_released_handler);

  yage::core::lambda_subscriber<wchar_t> char_entered_handler(
        [&](yage::core::event<wchar_t> event) {
          log(LOG_DEBUG, "Char entered: %c at %f", event.get_data(),
              event.get_time());
        });
  window->get_char_entered().subscribe(char_entered_handler);

  yage::core::lambda_subscriber<yage::graphics::mouse_scroll_data>
        mouse_scrolled_handler(
              [&](yage::core::event<yage::graphics::mouse_scroll_data> event) {
                auto data = event.get_data();
                log(LOG_DEBUG, "Mouse scrolled: (%f, %f) at %f", data.amount_x,
                    data.amount_y, event.get_time());
              });
  window->get_mouse_scrolled().subscribe(mouse_scrolled_handler);

  yage::core::lambda_subscriber<yage::graphics::mouse_click_data>
        mouse_pressed_handler(
              [&](yage::core::event<yage::graphics::mouse_click_data> event) {
                auto data = event.get_data();
                log(LOG_DEBUG, "Mouse pressed: %s at (%f, %f) at %f",
                    yage::core::io::mouse_to_string(data.button).c_str(),
                    data.x, data.y, event.get_time());
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
    graphics->draw_rect(x, y, 200, 200);
    graphics->draw_rect(330, 390, 300, 100);
    graphics->flush();

    /* Perform moving average.
     * Note that some GL backends may have their own vsync,
     * so this may not work as expected. */
    draw_time = draw_time * 0.9f + (yage::core::get_ms() - draw_start) * 0.1f;

    if (window->update() != yage::core::error::OK)
    {
      log(LOG_ERROR, "Failed to update window");
      break;
    }
  }
}
