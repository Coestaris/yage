/** ========================================================================
 *
 * @file profiler.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <kurylko.m@ajax.systems>
 * @copyright Ajax Systems
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/util/profiler.hpp"

#include <mutex>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::util::simple_profiler::simple_profiler(size_t max_points)
{
  prev_points.resize(max_points);
  points.resize(max_points);

  /* Fill with zeros */
  std::fill(prev_points.begin(), prev_points.end(), 0);
  std::fill(points.begin(), points.end(), 0);

  this->max_points = max_points;
}

void yc::util::simple_profiler::flush()
{
}

void yc::util::simple_profiler::start_cycle()
{
  std::lock_guard<std::mutex> lock(mutex);

  /* Copy current points to previous */
  prev_points = points;

  /* Reset current points */
  std::fill(points.begin(), points.end(), 0);
}

void yc::util::simple_profiler::passed(point point)
{
  points[point] = yc::get_ns();
}

yc::milliseconds yc::util::simple_profiler::get_cycle_time()
{
  throw std::runtime_error("Not implemented");
}

yc::milliseconds yc::util::simple_profiler::get_diff(point a, point b)
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

size_t yc::util::file_profiler::find_point(size_t start, point point)
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

yc::util::file_profiler::file_profiler(
      size_t max_records, const std::string & filename)
{
  this->max_records = max_records;
  records.resize(max_records);

  file = fopen(filename.c_str(), "w");
  if (file == nullptr)
  {
    throw std::runtime_error("Cannot open file");
  }
}

yc::util::file_profiler::~file_profiler()
{
  flush();
  fclose(file);
}

void yc::util::file_profiler::flush()
{
  for (size_t i = 0; i < current_record; ++i)
  {
    fprintf(file, "%d %llu\n", records[i].point, records[i].time);
  }
}

void yc::util::file_profiler::passed(point point)
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

yc::milliseconds yc::util::file_profiler::get_cycle_time()
{
  return static_cast<float>(
               (records[current_record].time - records[cycle_start].time)
               / 1000ll)
         / 1000.0f;
}

yc::milliseconds yc::util::file_profiler::get_diff(point a, point b)
{
  std::lock_guard<std::mutex> lock(mutex);

  size_t a_pos = find_point(current_record, a);
  size_t b_pos = find_point(a_pos + 1, b);

  return static_cast<float>(
               (records[b_pos].time - records[a_pos].time) / 1000ll)
         / 1000.0f;
}