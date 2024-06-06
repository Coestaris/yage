/** ========================================================================
 *
 * @file log.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "log.hpp"

#include <iostream>
#include <cstring>
#include <thread>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
static struct
{
  yage::core::logger * logger;
} ctx = { .logger = nullptr };

/* Shared functions ========================================================= */
/* Default formatter */
void yage::core::logger::format(
      char *       dst,
      size_t       size,
      const char * fmt,
      const char * file,
      int          line,
      va_list      args)
{
  /* Get time in format MM-DD-YYYY HH:MM:SS.MS */
  time_t      now = time(nullptr);
  struct tm * tm  = localtime(&now);
  char        time_str[32];
  strftime(time_str, sizeof(time_str), "%m-%d-%Y %H:%M:%S", tm);

  /* Get thread id */
  std::thread::id id = std::this_thread::get_id();
  snprintf(dst, size, "[%%s] [%%d] %s [%%s:%%d] ", fmt);
  snprintf(dst, size, dst, time_str, id, file, line);
}

void yage::core::logger::_log(
      int level, const char * fmt, const char * file, int line, ...)
{
  if (level < this->min_level)
    return;

  char buff[1024];

  va_list args;
  va_start(args, line);
  this->format(buff, sizeof(buff), fmt, file, line, args);
  va_end(args);

  this->print(buff);
}

void yage::core::logger::set_level(int level)
{
  this->min_level = level;
}

int yage::core::logger::get_level() const
{
  return this->min_level;
}

void yage::core::set_global_logger(yage::core::logger * logger)
{
  ctx.logger = logger;
}

yage::core::logger * yage::core::get_global_logger()
{
  if (!ctx.logger)
    throw std::runtime_error("No logger set");

  return ctx.logger;
}