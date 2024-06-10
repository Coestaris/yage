/** ========================================================================
 *
 * @file os_events.hpp
 * @date 10-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/pubsub.hpp"

namespace yage::core
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
using signal_subscriber = yage::core::sync_subscriber<int>;

class os_events
{
public:
  yage::core::publisher<int> on_stop_signal;
  yage::core::publisher<int> on_ill_signal;

  os_events();

  void update();
};

}
/* Variables ================================================================ */
/* Prototypes =============================================================== */

