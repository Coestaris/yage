/** ========================================================================
 *
 * @file main.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
int main(int argc, char * argv[])
{
  yage::core::stdout_logger logger;
  set_global_logger(&logger);

  log(LOG_DEBUG, "Hello, World!");
  log(LOG_INFO, "Hello, World!");
  log(LOG_WARN, "Hello, World!");
  log(LOG_ERROR, "Hello, World!");
  log(LOG_FATAL, "Hello, World!");

  /* Flush the log */
  logger.flush();

  return 0;
}