/** ========================================================================
 *
 * @file log.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/log.hpp"
#include "yagecore/named_thread.hpp"
#include "yagecore/time.hpp"

#include <iostream>
#include <cstring>
#include <thread>
#include <csignal>

/* Defines ================================================================== */
#if USE_DEBUG
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
#else
#define COLOR_BLUE     ""
#define COLOR_GREEN    ""
#define COLOR_BOLD     ""
#define COLOR_MAGENTA  ""
#define COLOR_CYAN     ""
#define COLOR_RED      ""
#define COLOR_YELLOW   ""
#define COLOR_BLACK_FG ""
#define COLOR_RED_BG   ""
#define COLOR_RESET    ""
#endif
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
static const char * level_specifiers[]
      = { COLOR_BOLD              COLOR_BLUE "trace" COLOR_RESET,
          COLOR_BOLD              COLOR_MAGENTA "debug" COLOR_RESET,
          COLOR_BOLD              COLOR_CYAN " info" COLOR_RESET,
          COLOR_BOLD              COLOR_YELLOW " warn" COLOR_RESET,
          COLOR_BOLD              COLOR_RED "error" COLOR_RESET,
          COLOR_BOLD COLOR_RED_BG COLOR_BLACK_FG "fatal" COLOR_RESET };

/* Static functions ========================================================= */
static struct
{
  yc::logger * logger;

} ctx = { .logger = nullptr };

/* Shared functions ========================================================= */
/* Default formatter */
void yc::logger::format(
      char *        dst,
      size_t        size,
      const char *  fmt,
      yc::log_level level,
      const char *  file,
      int           line,
      va_list       args)
{
  /* Get time in format MM-DD-YYYY HH:MM:SS.MS */
  char      time_str[32];
  struct tm tm = yc::get_time();
  strftime(time_str, sizeof(time_str), "%m-%d-%Y %H:%M:%S", &tm);

  int pid = getpid();

  std::string task_name = yc::named_thread::get_thread_name();
  if (task_name.empty())
  {
    task_name = "???";
  }

  if (static_cast<size_t>(level)
      >= sizeof(level_specifiers) / sizeof(level_specifiers[0]))
  {
    throw std::runtime_error("Invalid log level");
  }

  /* Format the format string */
  char format[1024];
  snprintf(
        format, sizeof(format),
        "[" COLOR_GREEN "%s.%3d" COLOR_RESET "][" COLOR_YELLOW
        "%d:%10s" COLOR_RESET "][%s] %s [" COLOR_CYAN "%s:%d" COLOR_RESET "] ",
        time_str, static_cast<int>(yc::get_ms()) % 1000, pid, task_name.c_str(),
        level_specifiers[level], fmt, file, line);

  /* Format the message */
  vsnprintf(dst, size, format, args);
}

void yc::logger::_log(
      yc::log_level level, const char * fmt, const char * file, int line, ...)
{
#if YAGE_STATIC_LOG_FILTERING
  /* If level is compile-time constant, we can filter it out */
  if (level < YAGE_LOG_LEVEL)
    return;
#else
  if (level < this->min_level)
    return;
#endif

  char buff[1024];

  va_list args;
  va_start(args, line);
  this->format(buff, sizeof(buff), fmt, level, file, line, args);
  va_end(args);

  this->print(buff);
}

void yc::logger::set_level(yc::log_level level)
{
#if YAGE_STATIC_LOG_FILTERING
  throw std::runtime_error("Cannot change log level in static build");
#else
  this->min_level = level;
#endif
}

#if YAGE_STATIC_LOG_FILTERING
constexpr
#endif
      yc::log_level
      yc::logger::get_level()
{
#if YAGE_STATIC_LOG_FILTERING
  return YAGE_LOG_LEVEL;
#else
  return this->min_level;
#endif
}

void yc::set_global_logger(yc::logger * logger)
{
  ctx.logger = logger;
}

yc::logger * yc::get_global_logger()
{
  if (!ctx.logger)
    throw std::runtime_error("No logger set");

  return ctx.logger;
}