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

protected:
public:
  virtual void flush()             = 0;
  virtual void start_cycle()       = 0;
  virtual void passed(point point) = 0;

  virtual yc::milliseconds get_cycle_time()           = 0;
  virtual yc::milliseconds get_diff(point a, point b) = 0;
};

class simple_profiler : public profiler
{
private:
  std::mutex                    mutex;
  std::vector<yc::nano_seconds> prev_points;
  std::vector<yc::nano_seconds> points;
  size_t                        max_points;
  point                         first_point = 0;

public:
  simple_profiler(size_t max_points = 100)
  {
    prev_points.resize(max_points);
    points.resize(max_points);

    /* Fill with zeros */
    std::fill(prev_points.begin(), prev_points.end(), 0);
    std::fill(points.begin(), points.end(), 0);

    this->max_points = max_points;
  }

  void flush() override
  {
  }

  void start_cycle() override
  {
    std::lock_guard<std::mutex> lock(mutex);

    /* Copy current points to previous */
    prev_points = points;

    /* Reset current points */
    std::fill(points.begin(), points.end(), 0);
  }

  void passed(point point) override
  {
    points[point] = yc::get_ns();
  }

  yc::milliseconds get_cycle_time() override
  {
    throw std::runtime_error("Not implemented");
  }

  yc::milliseconds get_diff(point a, point b) override
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (prev_points[b] == 0 || prev_points[a] == 0)
    {
      return 0;
    }
    yc::nano_seconds diff = prev_points[b] - prev_points[a];
    if (diff < 0)
    {
      return 0;
    }

    return static_cast<float>(diff / 1000ll) / 1000.0f;
  }
};

class file_profiler : public profiler
{
private:
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

  size_t find_point(size_t start, point point)
  {
    size_t pos = start;
    for (size_t i = max_records; i > 0; --i)
    {
      if (records[pos].point == point)
      {
        return pos;
      }

      /* decrementing with overflow */
      if (pos == 0)
      {
        pos = max_records;
      }
      pos--;
    }

    return -1;
  }

public:
  explicit file_profiler(size_t max_records, const std::string & filename)
  {
    this->max_records = max_records;
    records.resize(max_records);

    file = fopen(filename.c_str(), "w");
    if (file == nullptr)
    {
      throw std::runtime_error("Cannot open file");
    }
  }

  ~file_profiler()
  {
    fclose(file);
  }

  void flush() override
  {
    for (size_t i = 0; i < current_record; ++i)
    {
      fprintf(file, "%d %llu\n", records[i].point, records[i].time);
    }
  }

  void passed(point point) override
  {
    std::lock_guard<std::mutex> lock(mutex);

    records[current_record].point = point;
    records[current_record].time  = yc::get_ns();

    ++current_record;

    if (current_record == max_records)
    {
      flush();
      current_record = 0;
    }
  }

  yc::milliseconds get_cycle_time() override
  {
    return static_cast<float>(
                 (records[current_record].time - records[cycle_start].time)
                 / 1000ll)
           / 1000.0f;
  }

  yc::milliseconds get_diff(point a, point b) override
  {
    std::lock_guard<std::mutex> lock(mutex);

    size_t a_pos = find_point(current_record, a);
    size_t b_pos = find_point(a_pos + 1, b);

    return static_cast<float>(
                 (records[b_pos].time - records[a_pos].time) / 1000ll)
           / 1000.0f;
  }
};

/* Prototypes =============================================================== */
}
