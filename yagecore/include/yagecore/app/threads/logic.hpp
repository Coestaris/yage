/** ========================================================================
 *
 * @file updater.hpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>

#include "yagecore/time.hpp"
#include "yagecore/sync_point.hpp"
#include "yagecore/profiler.hpp"
#include "yagecore/named_thread.hpp"

#include "yagecore/graphics/window.hpp"
#include "yagecore/app/application.hpp"

namespace yc::app::threads
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class logic final : public yc::named_thread
{
  std::shared_ptr<yc::app::application> app;
  std::shared_ptr<yc::profiler>   profiler;
  bool                                  should_close = false;

public:
  logic(std::shared_ptr<yc::app::application> app,
        std::shared_ptr<yc::profiler>   profiler);

  void run() override;
  void close();
};

}