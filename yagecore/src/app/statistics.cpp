/** ========================================================================
 *
 * @file statistics.cpp
 * @date 18-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/app/statistics.hpp"
#include "yagecore/log.hpp"

#include <cinttypes>
#include <stdexcept>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
static yc::milliseconds wma(yc::milliseconds a, yc::milliseconds b)
{
#define WEIGHT 0.5f
  return static_cast<yc::milliseconds>(a * WEIGHT + b * (1.0f - WEIGHT));
}

/* Shared functions ========================================================= */
void yc::app::statistics::log_instances_count(size_t count)
{
  instances_count = count;
}

void yc::app::statistics::log_update_time(yc::milliseconds time)
{
  update_time = wma(update_time, time);
}

void yc::app::statistics::log_particles_time(yc::milliseconds time)
{
  particles_time = wma(particles_time, time);
}

void yc::app::statistics::log_physics_time(yc::milliseconds time)
{
  physics_time = wma(physics_time, time);
}

void yc::app::statistics::log_ai_time(yc::milliseconds time)
{
  ai_time = wma(ai_time, time);
}

void yc::app::statistics::log_sound_time(yc::milliseconds time)
{
  sound_time = wma(sound_time, time);
}

void yc::app::statistics::log_fps(yc::milliseconds _fps)
{
  fps = wma(fps, _fps);
}

void yc::app::statistics::log_draw_time(yc::milliseconds time)
{
  draw_time = wma(draw_time, time);
}

void yc::app::statistics::log_copy_time(yc::milliseconds time)
{
  copy_time = wma(copy_time, time);
}

size_t yc::app::statistics::register_pass(const std::string & name)
{
  passes[name] = passes_time.size();
  passes_time.push_back({});
  return passes_time.size() - 1;
}

void yc::app::statistics::log_pass(
      size_t pass, yc::app::statistics::pass_info time)
{
#if USE_DEBUG
  if (pass >= passes_time.size())
  {
    logging(LOG_ERROR, "Invalid pass number: %" PRIu64, pass);
    return;
  }
#endif
  auto & t = passes_time[pass];

  t.draw_time       = wma(t.draw_time, time.draw_time);
  t.drawn_meshes    = time.drawn_meshes;
  t.drawn_triangles = time.drawn_triangles;
}

size_t yc::app::statistics::get_instances_count() const
{
  return instances_count;
}

yc::milliseconds yc::app::statistics::get_update_time() const
{
  return update_time;
}

yc::milliseconds yc::app::statistics::get_particles_time() const
{
  return particles_time;
}

yc::milliseconds yc::app::statistics::get_physics_time() const
{
  return physics_time;
}

yc::milliseconds yc::app::statistics::get_ai_time() const
{
  return ai_time;
}

yc::milliseconds yc::app::statistics::get_sound_time() const
{
  return sound_time;
}

yc::milliseconds yc::app::statistics::get_draw_time() const
{
  return draw_time;
}

yc::milliseconds yc::app::statistics::get_copy_time() const
{
  return copy_time;
}

yc::milliseconds yc::app::statistics::get_fps() const
{
  return fps;
}

yc::app::statistics::pass_info
yc::app::statistics::get_pass_info(size_t pass) const
{
  return passes_time[pass];
}

size_t yc::app::statistics::get_passes_count() const
{
  return passes_time.size();
}

const std::string & yc::app::statistics::get_pass_name(size_t pass) const
{
  for (const auto & [name, index] : passes)
  {
    if (index == pass)
    {
      return name;
    }
  }

  throw std::out_of_range("Invalid pass number");
}
