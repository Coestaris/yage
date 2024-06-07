/** ========================================================================
 *
 * @file threads.cpp
 * @date 07-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "threads.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
static struct
{
  std::shared_ptr<yage::core::thread_pool> pool;

} ctx;

/* Shared functions ========================================================= */
void yage::core::set_global_thread_pool(yage::core::thread_pool * pool)
{
  ctx.pool = std::shared_ptr<yage::core::thread_pool>(pool);
}

std::shared_ptr<yage::core::thread_pool> yage::core::get_global_thread_pool()
{
  return ctx.pool;
}