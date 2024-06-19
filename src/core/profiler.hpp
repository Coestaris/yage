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

namespace yage::core
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
class profiler
{
private:
protected:
  int max_points;
  int current_point = 0;
  struct point
  {
    int   point;
    long long time; /* ns */
  } * points;

public:
  virtual void flush() = 0;

  ~profiler()
  {
    delete[] points;
  }

  profiler(size_t max_points)
  {
    this->max_points = max_points;
    points           = new point[max_points];
  }

  void passed(int point)
  {
    points[current_point].point = point;
    points[current_point].time
          = std::chrono::time_point_cast<std::chrono::nanoseconds>(
                  std::chrono::system_clock::now())
                  .time_since_epoch()
                  .count();

    ++current_point;

    if (current_point == max_points)
    {
      flush();
      current_point = 0;
    }
  }
};

class file_profiler : public profiler
{
private:
  FILE * file;

public:
  void flush() override
  {
    for (int i = 0; i < current_point; ++i)
    {
      fprintf(file, "%d %llu\n", points[i].point, points[i].time);
    }
  }

  file_profiler(size_t max_points, const char * filename)
  : profiler(max_points)
  {
    file = fopen(filename, "w");
  }

  ~file_profiler()
  {
    fclose(file);
  }
};

/* Prototypes =============================================================== */
}
