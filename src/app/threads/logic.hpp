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
#include "graphics/window.hpp"
#include "core/util/sync_point.hpp"
#include "core/util/profiler.hpp"
#include "app/application.hpp"

namespace yage::app::threads
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class logic : public yage::core::util::named_thread
{
private:
  yage::app::application *     app;
  yage::core::util::profiler * profiler;
  bool                         should_close = false;

  void profile(int point)
  {
    if (profiler)
    {
      profiler->passed(point);
    }
  }

public:
  logic(yage::app::application * app, yage::core::util::profiler * profiler)
  : yage::core::util::named_thread("logic")
  , app(app)
  , profiler(profiler)
  {
  }

  void run() override;

  void close();
};

}