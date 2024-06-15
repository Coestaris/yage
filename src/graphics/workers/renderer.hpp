/** ========================================================================
 *
 * @file renderer.hpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/threads.hpp"
#include "core/pubsub.hpp"
#include "core/os_events.hpp"
#include "graphics/window.hpp"
#include "graphics/frequency_keeper.hpp"

namespace yage::graphics::workers
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class renderer : public yage::core::worker<void>,
                        public yage::core::signal_subscriber
{
private:
  std::string              win_title;
  yage::graphics::geometry win_geometry;
  yage::graphics::window * window;
  float                    draw_time = 0.0f;
  frequency_keeper         keeper;

  // TODO: temporary
  float x = 0.0f, y = 0.0f;

public:
  void on_event(const yage::core::event<int> & event) override;
  void run() override;

  float get_draw_time() const
  {
    return draw_time;
  }

  float get_keeper_cps() const
  {
    return keeper.get_cps();
  }

public:
  renderer(
        yage::graphics::window * window,
        std::string              title,
        yage::graphics::geometry geometry,
        int                      max_fps)
  : yage::core::worker<void>("renderer")
  , win_title(std::move(title))
  , win_geometry(geometry)
  , window(window)
  , keeper(max_fps)
  {
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}