/** ========================================================================
 *
 * @file renderer.hpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/named_thread.hpp"
#include "core/os_events.hpp"
#include "graphics/window.hpp"
#include "graphics/frequency_keeper.hpp"
#include "threads.hpp"
#include "core/sync_point.hpp"
#include "core/profiler.hpp"

namespace yage::graphics::threads
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class renderer : public yage::core::named_thread
{
private:
  open_window_arguments args;
  frequency_keeper      keeper;

  yage::core::profiler * profiler;
  yage::core::sync_point * pre_frame_sync;
  yage::core::sync_point * post_frame_sync;

  float draw_time = 0.0f;

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
  renderer(
        yage::core::profiler * profiler,
        yage::core::sync_point * pre_frame_sync,
        yage::core::sync_point * post_frame_sync,
        open_window_arguments &  args)
  : yage::core::named_thread("renderer")
  , args(args)
  , keeper(args.max_fps)
  , profiler(profiler)
  , pre_frame_sync(pre_frame_sync)
  , post_frame_sync(post_frame_sync)
  {
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}