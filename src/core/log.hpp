/** ========================================================================
 *
 * @file y_log.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

#include <cstdarg>
#include <cstdio>
#include <memory>
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
namespace yage::core
{

/* Defines ================================================================== */
#define LOG_DEBUG 0
#define LOG_INFO  1
#define LOG_WARN  2
#define LOG_ERROR 3
#define LOG_FATAL 4

/* Macros =================================================================== */
#define log(__level, __fmt, ...)                                               \
  yage::core::get_global_logger()->_log(                                       \
        __level, __fmt, __FILE__, __LINE__, ##__VA_ARGS__)

/* Types ==================================================================== */
class logger
{
private:
  int min_level = 0;

private:
  virtual void print(const char * msg) = 0;

  virtual void format(
        char *       dst,
        std::size_t  size,
        const char * fmt,
        const char * file,
        int          line,
        va_list      args);

public:
  void set_level(int level);
  int  get_level() const;

  void _log(int level, const char * fmt, const char * file, int line, ...);
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
void     set_global_logger(logger * logger);
logger * get_global_logger();

}

#ifdef __cplusplus
}
#endif
