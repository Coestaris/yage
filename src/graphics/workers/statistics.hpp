/** ========================================================================
 *
 * @file statistics.hpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/threads.hpp"
#include "core/pubsub.hpp"

#include "graphics/workers/renderer.hpp"

namespace yage::graphics::workers
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class statistic_worker : public yage::core::worker<void>
{
private:
  yage::graphics::workers::renderer * renderer;

public:
  explicit statistic_worker(yage::graphics::workers::renderer * renderer)
  : yage::core::worker<void>("statistic")
  , renderer(renderer)
  {
  }

  void run() override
  {
    while (true)
    {
      if (!renderer->is_running())
      {
        log(LOG_DEBUG,
            "Renderer worker is not running. Exiting statistic worker");
        break;
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));
      log(LOG_INFO, "Draw time: %.2f ms, FPS: %.2f", renderer->get_draw_time(),
          renderer->get_keeper_cps());
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}