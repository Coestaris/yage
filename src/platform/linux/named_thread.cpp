/** ========================================================================
 *
 * @file named_thread.cpp
 * @date 01-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "core/util/named_thread.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yage::core::util::named_thread::set_port_name()
{
  /* Note that this function is not portable, and
   * should be implemented for each platform separately. */
  pthread_setname_np(thread->native_handle(), name.c_str());
}
