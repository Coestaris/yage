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
class render_pass
{
public:
  enum class type
  {
    OBJECT_PASS,
    VIRTUAL_PASS,
  };

private:
  std::string name;
  type        type_;
  bool        skip = false;

  yc::milliseconds draw_time {}, start_time {};
  size_t           drawn_meshes {}, drawn_meshes_imm {};
  size_t           drawn_triangles {}, drawn_triangles_imm {};

public:
  virtual ~render_pass() = default;
           render_pass(std::string name, type type);

  /** Called before rendering all objects */
  virtual void on_end() = 0;

  /** Called after rendering all objects */
  virtual void on_begin() = 0;

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

  void set_name(std::string name);
  void set_skip(bool skip);

  [[nodiscard]] std::string get_name();
  [[nodiscard]] bool        get_skip();
  [[nodiscard]] type        get_type();

  /* Wrappers for on_begin and on_end */
  void begin();
  void end();

  void record_result(const yc::graphics::draw_result & result);

  [[nodiscard]] yc::milliseconds get_draw_time() const;
  [[nodiscard]] size_t           get_drawn_meshes() const;
  [[nodiscard]] size_t           get_drawn_triangles() const;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}