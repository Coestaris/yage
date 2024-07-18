/** ========================================================================
 *
 * @file statistics.hpp
 * @date 18-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <map>
#include <string>
#include <yagecore/time.hpp>

namespace yc::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class statistics
{
public:
  struct pass_info
  {
    yc::milliseconds draw_time       = 0;
    size_t           drawn_meshes    = 0;
    size_t           drawn_triangles = 0;
  };

private:
  size_t           instances_count = 0;
  yc::milliseconds update_time     = 0;
  yc::milliseconds particles_time  = 0;
  yc::milliseconds physics_time    = 0;
  yc::milliseconds ai_time         = 0;
  yc::milliseconds sound_time      = 0;

  yc::milliseconds draw_time = 0;
  yc::milliseconds copy_time = 0;
  float            fps       = 0;

  std::vector<pass_info>        passes_time;
  std::map<std::string, size_t> passes;

public:
  /* Setters */
  /* Logic */
  void log_instances_count(size_t count);
  void log_update_time(yc::milliseconds time);
  void log_particles_time(yc::milliseconds time);
  void log_physics_time(yc::milliseconds time);
  void log_ai_time(yc::milliseconds time);
  void log_sound_time(yc::milliseconds time);
  /* Renderer */
  void   log_fps(float fps);
  void   log_draw_time(yc::milliseconds time);
  void   log_copy_time(yc::milliseconds time);
  size_t register_pass(const std::string & name);
  void   log_pass(size_t pass, pass_info time);

  /* Getters */
  /* Logic */
  [[nodiscard]] yc::milliseconds get_update_time() const;
  [[nodiscard]] yc::milliseconds get_particles_time() const;
  [[nodiscard]] yc::milliseconds get_physics_time() const;
  [[nodiscard]] yc::milliseconds get_ai_time() const;
  [[nodiscard]] yc::milliseconds get_sound_time() const;
  /* Renderer */
  [[nodiscard]] yc::milliseconds    get_draw_time() const;
  [[nodiscard]] yc::milliseconds    get_copy_time() const;
  [[nodiscard]] float               get_fps() const;
  [[nodiscard]] pass_info           get_pass_info(size_t pass) const;
  [[nodiscard]] size_t              get_passes_count() const;
  [[nodiscard]] const std::string & get_pass_name(size_t pass) const;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}