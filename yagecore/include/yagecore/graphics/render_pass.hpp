/** ========================================================================
 *
 * @file render_pass.hpp
 * @date 28-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <utility>
#include "yagecore/graphics/node.hpp"

#include <yagecore/app/object.hpp>

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
enum class pass_type
{
  OBJECT_PASS,
  VIRTUAL_PASS,
};

class render_pass
{
private:
  std::string name;
  pass_type   type;
  bool        skip = false;

#if COLLECT_DRAW_STATISTICS
  yc::milliseconds draw_time {}, start_time {};
  size_t           drawn_meshes {}, drawn_meshes_imm {};
  size_t           drawn_triangles {}, drawn_triangles_imm {};
#endif

public:
  virtual ~render_pass() = default;
           render_pass(std::string name, pass_type type)
  : name(std::move(name))
  , type(type)
  {
  }

protected:
  /** Called before rendering all objects */
  virtual void on_end() = 0;

  /** Called after rendering all objects */
  virtual void on_begin() = 0;

public:
  /** Called once before rendering loop */
  virtual void before_loop() = 0;

  /** Called before rendering single object.
   * If returns false, object will be skipped */
  virtual bool on_node(const yc::app::object::renderable & object) = 0;

  /** Called before rendering single mesh.
   * If returns false, mesh will be skipped */
  virtual bool on_mesh(
        const mesh_buffer &                             mesh,
        const std::shared_ptr<yc::graphics::material> & material)
        = 0;

  virtual void set_name(std::string _name)
  {
    this->name = std::move(_name);
  }
  virtual std::string get_name()
  {
    return this->name;
  }
  virtual void set_skip(bool _skip)
  {
    this->skip = _skip;
  }
  virtual bool get_skip()
  {
    return this->skip;
  }
  virtual pass_type get_type()
  {
    return this->type;
  }

  /* Wrappers for on_begin and on_end */
  void begin()
  {
#if COLLECT_DRAW_STATISTICS
    drawn_meshes_imm    = 0;
    drawn_triangles_imm = 0;
    start_time          = yc::get_ms();
#endif

    on_begin();
  }

  void end()
  {
    on_end();

#if COLLECT_DRAW_STATISTICS
    draw_time       = yc::get_ms() - start_time;
    drawn_meshes    = drawn_meshes_imm;
    drawn_triangles = drawn_triangles_imm;
#endif
  }

#if COLLECT_DRAW_STATISTICS
  yc::milliseconds get_draw_time() const
  {
    return draw_time;
  }
  size_t get_drawn_meshes() const
  {
    return drawn_meshes;
  }
  size_t get_drawn_triangles() const
  {
    return drawn_triangles;
  }

  void record_result(const yc::graphics::draw_result & result)
  {
    drawn_meshes_imm += result.get_drawn_meshes();
    drawn_triangles_imm += result.get_drawn_triangles();
  }

#else
  void record_result(const yc::graphics::draw_result & result) {};
#endif
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}