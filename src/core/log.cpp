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
#include <sys/time.h>
#include <csignal>

/* Defines ================================================================== */
#define COLOR_BLUE     "\033[34m"
#define COLOR_GREEN    "\033[32m"
#define COLOR_BOLD     "\033[1m"
#define COLOR_MAGENTA  "\033[35m"
#define COLOR_CYAN     "\033[36m"
#define COLOR_RED      "\033[31m"
#define COLOR_YELLOW   "\033[33m"
#define COLOR_BLACK_FG "\033[30m"
#define COLOR_RED_BG   "\033[41m"
#define COLOR_RESET    "\033[0m"

/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
static const char * level_specifiers[]
      = { COLOR_BOLD COLOR_MAGENTA "debug" COLOR_RESET,
          COLOR_BOLD COLOR_CYAN " info" COLOR_RESET,
          COLOR_BOLD COLOR_YELLOW " warn" COLOR_RESET,
          COLOR_BOLD COLOR_RED "error" COLOR_RESET,
          COLOR_BOLD COLOR_RED_BG COLOR_BLACK_FG "fatal" COLOR_RESET };

/* Static functions ========================================================= */
static struct
{
  yage::core::logger * logger;
} ctx = { .logger = nullptr };

/* Shared functions ========================================================= */
/* Default formatter */
void yage::core::logger::format(
      char *                dst,
      size_t                size,
      const char *          fmt,
      yage::core::log_level level,
      const char *          file,
      int                   line,
      va_list               args)
{
  /* Get time in format MM-DD-YYYY HH:MM:SS.MS */
  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm * tm = localtime(&tv.tv_sec);
  char        time_str[32];
  strftime(time_str, sizeof(time_str), "%m-%d-%Y %H:%M:%S", tm);
  int ms = tv.tv_usec / 1000;
  if (ms >= 1000)
  {
    ms -= 1000;
    tv.tv_sec++;
  }

  /* Get thread id */
  int id = 0;
  /* Get PID */
  int pid = getpid();

  if (level >= (int)(sizeof(level_specifiers) / sizeof(level_specifiers[0])))
    throw std::runtime_error("Invalid log level");

  /* Format the format string */
  char format[1024];
  snprintf(
        format, sizeof(format),
        "[" COLOR_GREEN "%s.%d" COLOR_RESET "][" COLOR_YELLOW
        "%d:%u" COLOR_RESET "][%s] %s [" COLOR_CYAN "%s:%d" COLOR_RESET "] ",
        time_str, ms, pid, id, level_specifiers[level], fmt, file, line);

  /* Format the message */
  vsnprintf(dst, size, format, args);
}

void yage::core::logger::_log(
      yage::core::log_level level,
      const char *          fmt,
      const char *          file,
      int                   line,
      ...)
{
  if (level < this->min_level)
    return;

  char buff[1024];

  va_list args;
  va_start(args, line);
  this->format(buff, sizeof(buff), fmt, level, file, line, args);
  va_end(args);

  this->print(buff);
}

void yage::core::logger::set_level(yage::core::log_level level)
{
  this->min_level = level;
}

yage::core::log_level yage::core::logger::get_level() const
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