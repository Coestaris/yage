/** ========================================================================
 *
 * @file os_events.hpp
 * @date 10-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "event_stream.hpp"

namespace yage::core
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
using signal_event_listener = yage::core::event_stream_listener<int>;
using signal_event_stream   = yage::core::event_stream<int>;

class os_events
{
public:
  signal_event_stream on_stop_signal;
  signal_event_stream on_ill_signal;

  os_events();

  void update();
};

}
/* Variables ================================================================ */
/* Prototypes =============================================================== */
