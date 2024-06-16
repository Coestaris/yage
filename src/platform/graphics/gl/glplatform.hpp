/** ========================================================================
 *
 * @file glplatform.hpp
 * @date 15-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */

namespace yage::platform::graphics::gl
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
void * gl_lookup(const char * func, bool required = false);

template <typename RetType = void, typename... Args>
RetType gl_call(const char * func, Args... args)
{
  auto proc = (RetType(*)(Args...))gl_lookup(func, true);
  return proc(args...);
}

}
