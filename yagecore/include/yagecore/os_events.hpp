/** ========================================================================
 *
 * @file os_events.hpp
 * @date 10-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "yagecore/util/event_stream.hpp"

namespace yc
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
using signal_event_listener = yc::util::event_stream_listener<int>;
using signal_event_stream   = yc::util::event_stream<int>;

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
