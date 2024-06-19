/** ========================================================================
 *
 * @file updater.hpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/named_thread.hpp"
#include "graphics/window.hpp"
#include "core/sync_point.hpp"
#include "core/profiler.hpp"

namespace yage::graphics::threads
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class logic : public yage::core::named_thread
{
private:
  yage::graphics::window * window;
  bool should_close = false;

  yage::core::profiler *   profiler;
  yage::core::sync_point * pre_frame_sync;
  yage::core::sync_point * post_frame_sync;

public:
  explicit logic(
        yage::core::profiler *   profiler,
        yage::core::sync_point * pre_frame_sync,
        yage::core::sync_point * post_frame_sync,
        yage::graphics::window * window)
  : yage::core::named_thread("logic")
  , window(window)
  , profiler(profiler)
  , pre_frame_sync(pre_frame_sync)
  , post_frame_sync(post_frame_sync)
  {
  }

  void run() override;

  void close();
};

}