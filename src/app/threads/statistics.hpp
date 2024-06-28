/** ========================================================================
 *
 * @file updater.hpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/util/named_thread.hpp"
#include "app/threads/logic.hpp"
#include "app/threads/renderer.hpp"
#include "glm/common.hpp"

namespace yage::app::threads
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class statistics : public yage::core::util::named_thread
{
private:
  constexpr static const float lerp_alpha     = 0.4;
  constexpr static const int   log_period_ms  = 1000;
  constexpr static const int   poll_period_ms = 200;

  yage::app::threads::logic *    logic;
  yage::app::threads::renderer * renderer;

  size_t counter      = 0;
  bool   should_close = false;

  float draw_time_ms = 0;
  float copy_time_ms = 0;
  float update_time_ms = 0;
  float particles_time_ms = 0;
  float physics_time_ms = 0;
  float ai_time_ms = 0;
  float sound_time_ms = 0;

public:
  statistics(
        yage::app::threads::logic *    logic,
        yage::app::threads::renderer * renderer)
  : yage::core::util::named_thread("statistics")
  , logic(logic)
  , renderer(renderer)
  {
  }

  void run()
  {
    while (true)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(poll_period_ms));

      if (should_close)
      {
        break;
      }

      // clang-format off
      draw_time_ms = glm::mix(draw_time_ms, renderer->get_draw_time(), lerp_alpha);
      copy_time_ms = glm::mix(copy_time_ms, renderer->get_copy_time(), lerp_alpha);
      update_time_ms = glm::mix(update_time_ms, logic->get_update_time(), lerp_alpha);
      particles_time_ms = glm::mix(particles_time_ms, logic->get_particles_time(), lerp_alpha);
      physics_time_ms = glm::mix(physics_time_ms, logic->get_physics_time(), lerp_alpha);
      ai_time_ms = glm::mix(ai_time_ms, logic->get_ai_time(), lerp_alpha);
      sound_time_ms = glm::mix(sound_time_ms, logic->get_sound_time(), lerp_alpha);
      // clang-format on

      /* Prescaler, to check data multiple
       * times per one log period */
      if (counter++ % (log_period_ms / poll_period_ms) != 0)
      {
        continue;
      }

      logging(LOG_INFO,
          "Obj: %zu (%zu). FPS: %2.2f (%.1f, %.1f). Logic: "
          "%.1f, %.1f, %.1f, %.1f, %.1f",
          renderer->get_instances_count(), logic->get_instances_count(),
          renderer->get_keeper_cps(), draw_time_ms, copy_time_ms,
          update_time_ms, particles_time_ms, physics_time_ms, ai_time_ms,
          sound_time_ms);
    }
  }

  void close()
  {
    should_close = true;
  }
};

}