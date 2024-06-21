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
#include "core/time.hpp"

namespace yage::core::util
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

  virtual yage::core::milliseconds get_cycle_time()           = 0;
  virtual yage::core::milliseconds get_diff(point a, point b) = 0;
};

class simple_profiler : public profiler
{
private:
  std::mutex                 mutex;
  yage::core::nano_seconds * prev_points;
  yage::core::nano_seconds * points;
  size_t                     max_points;
  point                      first_point = 0;

public:
  simple_profiler(size_t max_points = 100)
  {
    prev_points      = new yage::core::nano_seconds[max_points];
    points           = new yage::core::nano_seconds[max_points];
    this->max_points = max_points;
  }

  void flush() override
  {
  }

  void start_cycle() override
  {
    std::lock_guard<std::mutex> lock(mutex);
    std::copy(points, points + max_points, prev_points);
    std::fill(points, points + max_points, 0);
  }

  void passed(point point) override
  {
    points[point] = yage::core::get_ns();
  }

  yage::core::milliseconds get_cycle_time() override
  {
    throw std::runtime_error("Not implemented");
  }

  yage::core::milliseconds get_diff(point a, point b) override
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (prev_points[b] == 0 || prev_points[a] == 0)
    {
      return 0;
    }
    yage::core::nano_seconds diff = prev_points[b] - prev_points[a];
    if (diff < 0)
    {
      return 0;
    }

    return (float)(diff / 1000) / 1000.0f;
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
    yage::core::util::profiler::point point;
    yage::core::nano_seconds          time;
  } * records; /* ring buffer */

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
  explicit file_profiler(size_t max_records, std::string filename)
  {
    this->max_records = max_records;
    records           = new record[max_records];

    file = fopen(filename.c_str(), "w");
    if (file == nullptr)
    {
      throw std::runtime_error("Cannot open file");
    }

    records = new record[max_records];
  }

  ~file_profiler()
  {
    delete[] records;
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
    records[current_record].time  = yage::core::get_ns();

    ++current_record;

    if (current_record == max_records)
    {
      flush();
      current_record = 0;
    }
  }

  yage::core::milliseconds get_cycle_time() override
  {
    return (float)((records[current_record].time - records[cycle_start].time)
                   / 1000)
           / 1000.0f;
  }

  yage::core::milliseconds get_diff(point a, point b) override
  {
    std::lock_guard<std::mutex> lock(mutex);

    size_t a_pos = find_point(current_record, a);
    size_t b_pos = find_point(a_pos + 1, b);

    return (float)((records[b_pos].time - records[a_pos].time) / 1000)
           / 1000.0f;
  }
};

/* Prototypes =============================================================== */
}
