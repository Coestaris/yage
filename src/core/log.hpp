/** ========================================================================
 *
 * @file y_log.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <cstdarg>
#include <cstdio>
#include <memory>

namespace yage::core
{

/* Defines ================================================================== */
/* Macros =================================================================== */
#if __USE_SHORT_FILE__
#define log(__level, __fmt, ...)                                               \
  yage::core::get_global_logger()->_log(                                       \
        __level, __fmt, __SHORT_FILE__, __LINE__, ##__VA_ARGS__)
#else
#define log(__level, __fmt, ...)                                               \
  yage::core::get_global_logger()->_log(                                       \
        __level, __fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define LOG_DEBUG yage::core::LOG_LEVEL_DEBUG
#define LOG_INFO  yage::core::LOG_LEVEL_INFO
#define LOG_WARN  yage::core::LOG_LEVEL_WARN
#define LOG_ERROR yage::core::LOG_LEVEL_ERROR
#define LOG_FATAL yage::core::LOG_LEVEL_FATAL

/* Types ==================================================================== */
enum log_level
{
  LOG_LEVEL_DEBUG = 0,
  LOG_LEVEL_INFO  = 1,
  LOG_LEVEL_WARN  = 2,
  LOG_LEVEL_ERROR = 3,
  LOG_LEVEL_FATAL = 4,
};

class logger
{
private:
  log_level min_level = LOG_LEVEL_DEBUG;

private:
  virtual void print(const char * msg) = 0;

  virtual void format(
        char *                dst,
        std::size_t           size,
        const char *          fmt,
        yage::core::log_level level,
        const char *          file,
        int                   line,
        va_list               args);

public:
  virtual ~logger() = default;

  void      set_level(log_level level);
  log_level get_level() const;

  __attribute__((format(printf, 3, 6))) void
  _log(log_level level, const char * fmt, const char * file, int line, ...);
  virtual void flush() = 0;
};

class stdout_logger : public logger
{
private:
  void print(const char * msg) override
  {
    printf("%s\n\r", msg);
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
void                    set_global_logger(logger * logger);
std::shared_ptr<logger> get_global_logger();

}
