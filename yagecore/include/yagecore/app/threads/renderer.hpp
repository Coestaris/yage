/** ========================================================================
 *
 * @file renderer.hpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>

#include "yagecore/util/named_thread.hpp"
#include "yagecore/os_events.hpp"
#include "yagecore/util/sync_point.hpp"
#include "yagecore/util/profiler.hpp"

#include "yagecore/graphics/window.hpp"
#include "yagecore/graphics/render_pass.hpp"
#include "yagecore/app/frequency_keeper.hpp"
#include "yagecore/app/application.hpp"

namespace yc::app::threads
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class renderer final : public yc::util::named_thread
{
  std::shared_ptr<yc::app::application> app;
  std::shared_ptr<yc::util::profiler>   profiler;
  yc::app::frequency_keeper             keeper;

  std::shared_ptr<yc::graphics::graphics> graphics {}; /* cache */

public:
  yc::signal_event_listener stop_signal_listener;

  renderer(
        std::shared_ptr<yc::app::application> app,
        std::shared_ptr<yc::util::profiler>   profiler);

  void run() override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}