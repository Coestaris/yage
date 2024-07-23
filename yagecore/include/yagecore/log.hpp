/** ========================================================================
 *
 * @file log.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <cstdarg>
#include <cstdio>
#include <memory>

namespace yc
{
/* Defines ================================================================== */
/* Macros =================================================================== */
#if 1
#if __USE_SHORT_FILE__
#define logging(__level, __fmt, ...)                                           \
  yc::get_global_logger()->_log(                                               \
        __level, __fmt, __SHORT_FILE__, __LINE__, ##__VA_ARGS__)
#else
#define logging(__level, __fmt, ...)                                           \
  yc::get_global_logger()->_log(                                               \
        __level, __fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#endif
#else
#define logging(__level, __fmt, ...)                                           \
  do                                                                           \
  {                                                                            \
  } while (0)
#endif

#define LOG_TRACE yc::LOG_LEVEL_TRACE
#define LOG_DEBUG yc::LOG_LEVEL_DEBUG
#define LOG_INFO  yc::LOG_LEVEL_INFO
#define LOG_WARN  yc::LOG_LEVEL_WARN
#define LOG_ERROR yc::LOG_LEVEL_ERROR
#define LOG_FATAL yc::LOG_LEVEL_FATAL

/* Types ==================================================================== */
enum log_level
{
  LOG_LEVEL_TRACE = 0,
  LOG_LEVEL_DEBUG = 1,
  LOG_LEVEL_INFO  = 2,
  LOG_LEVEL_WARN  = 3,
  LOG_LEVEL_ERROR = 4,
  LOG_LEVEL_FATAL = 5,
};

class logger
{
private:
  log_level min_level = LOG_LEVEL_DEBUG;

private:
  virtual void print(const char * msg) = 0;

  virtual void format(
        char *        dst,
        std::size_t   size,
        const char *  fmt,
        yc::log_level level,
        const char *  file,
        int           line,
        va_list       args);

public:
  virtual ~logger() = default;

  void set_level(log_level level);

#if YAGE_STATIC_LOG_FILTERING
  [[nodiscard]] constexpr static
#endif
        log_level
        get_level();

  __attribute__((format(printf, 3, 6))) void
  _log(log_level level, const char * fmt, const char * file, int line, ...);

  virtual void flush() = 0;
};

class stdout_logger : public logger
{
  void print(const char * msg) override
  {
    puts(msg);
    fflush(stdout);
  }

public:
  stdout_logger() = default;

  void flush() override
  {
    fflush(stdout);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
void     set_global_logger(logger * logger);
logger * get_global_logger();

}
