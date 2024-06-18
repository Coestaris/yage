/** ========================================================================
 *
 * @file updater.hpp
 * @date 18-06-2024
 * @author Maxim Kurylko <kurylko.m@ajax.systems>
 * @copyright Ajax Systems
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/pubsub.hpp"
#include "core/named_thread.hpp"
#include "graphics/window.hpp"

namespace yage::graphics::workers
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
class logic : public yage::core::worker<void>
{
private:
  yage::graphics::window * window;
  bool                     closing = false;

public:
  explicit logic(yage::graphics::window * window)
  : yage::core::worker<void>("updater")
  , window(window)
  {
  }

  void run() override;
};

}