/** ========================================================================
 *
 * @file renderer.hpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/util/named_thread.hpp"
#include "core/os_events.hpp"
#include "graphics/window.hpp"
#include "app/frequency_keeper.hpp"
#include "app/application.hpp"
#include "core/util/sync_point.hpp"
#include "core/util/profiler.hpp"

namespace yage::app::threads
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class renderer : public yage::core::util::named_thread
{
private:
  yage::app::application *     app;
  yage::core::util::profiler * profiler;
  yage::app::frequency_keeper  keeper;
  float                        draw_time = 0.0f;

public:
  yage::core::signal_event_listener stop_signal_listener;

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
  renderer(yage::app::application * app, yage::core::util::profiler * profiler)
  : yage::core::util::named_thread("renderer")
  , app(app)
  , profiler(profiler)
  , keeper(app->window->get_max_fps())
  {
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}