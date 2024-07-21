/** ========================================================================
 *
 * @file draw_buffer.hpp
 * @date 14-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <memory>
#include "GL/gl.h"
#include "yagecore/graphics/mesh_buffer.hpp"
#include "yagecore/graphics/material.hpp"
#include "yagecore/gl/graphics.hpp"
#include "yagecore/gl/util.hpp"
#include "yagecore/gl/glfuncs.hpp"
#include "yagecore/graphics/render_pass.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class node;

class mesh_buffer final : public yc::graphics::mesh_buffer
{
public:
  struct indicies_info
  {
    GLuint ebo;
    GLenum type;

    /* Attention: Offset already in bytes! */
    void * offset;
  };

  struct vertices_info
  {
    GLuint             vbo, vao;
    GLenum             type;
    GLsizei            count;
    yc::graphics::aabb aabb;
  };

private:
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics>       ctx;
  std::shared_ptr<yc::graphics::material> material;

  vertices_info vertices;
  indicies_info indicies;

public:
  mesh_buffer(
        const std::shared_ptr<yc::gl::graphics> &       graphics,
        vertices_info                                   vertices,
        indicies_info                                   indicies,
        const std::shared_ptr<yc::graphics::material> & material);
  void unload() override;
  yc::graphics::draw_result
  draw(const std::shared_ptr<yc::graphics::render_pass> & pass) override;

  [[nodiscard]] yc::graphics::aabb get_aabb() const override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}