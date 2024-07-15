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
#include "yagecore/gl/glplatform.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class node;

class mesh_buffer : public yc::graphics::mesh_buffer
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
  friend node;

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
        const std::shared_ptr<yc::graphics::material> & material)
  : ctx(graphics)
  , material(material)
  , vertices(std::move(vertices))
  , indicies(std::move(indicies))
  {
  }

  void unload() override
  {
    if (vertices.vao)
      yc::gl::glDeleteVertexArrays(1, &vertices.vao);
    if (vertices.vbo)
      yc::gl::glDeleteBuffers(1, &vertices.vbo);
    if (indicies.ebo)
      yc::gl::glDeleteBuffers(1, &indicies.ebo);
  }

  inline yc::graphics::draw_result
  draw(const std::shared_ptr<yc::graphics::render_pass> & pass) override
  {
    // if (pass && !pass->on_mesh(*this, material))
    // {
    //   return yc::graphics::draw_result();
    // }

    yc::gl::glBindVertexArray(vertices.vao);
    yc::gl::glDrawElements(
          vertices.type, vertices.count, indicies.type, indicies.offset);
    yc::gl::glBindVertexArray(0);

    return yc::graphics::draw_result(1, vertices.count);
  }

  yc::graphics::aabb get_aabb() const override
  {
    return vertices.aabb;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}