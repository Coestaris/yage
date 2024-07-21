/** ========================================================================
 *
 * @file profiler.hpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include "yagecore/time.hpp"

#include <mutex>
#include <vector>

namespace yc::util
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
class profiler
{
public:
  /* Time in nanoseconds */
  using point = int;

public:
  virtual ~    profiler()          = default;
  virtual void flush()             = 0;
  virtual void start_cycle()       = 0;
  virtual void passed(point point) = 0;

  virtual yc::milliseconds get_cycle_time()           = 0;
  virtual yc::milliseconds get_diff(point a, point b) = 0;
};

class simple_profiler final : public profiler
{
  std::mutex                    mutex;
  std::vector<yc::nano_seconds> prev_points;
  std::vector<yc::nano_seconds> points;
  size_t                        max_points;
  point                         first_point = 0;

public:
  explicit simple_profiler(size_t max_points = 100);

  void flush() override;
  void start_cycle() override;
  void passed(point point) override;

  yc::milliseconds get_cycle_time() override;
  yc::milliseconds get_diff(point a, point b) override;
};

class file_profiler : public profiler
{
  FILE * file {};

  std::mutex mutex;
  size_t     max_records;
  size_t     current_record = 0;
  size_t     cycle_start    = 0;

  struct record
  {
    yc::util::profiler::point point;
    yc::nano_seconds          time;
  };

  std::vector<record> records;

  size_t find_point(size_t start, point point);

public:
  explicit file_profiler(size_t max_records, const std::string & filename);

  ~file_profiler() override;

  void flush() override;
  void passed(point point) override;

  yc::milliseconds get_cycle_time() override;
  yc::milliseconds get_diff(point a, point b) override;
};

/* Prototypes =============================================================== */
}
