/** ========================================================================
 *
 * @file mesh_buffer.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/mesh_buffer.hpp"
#include "yagecore/gl/mesh_buffer.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::gl::mesh_buffer::mesh_buffer(
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

void yc::gl::mesh_buffer::unload()
{
  if (vertices.vao)
    yc::gl::glDeleteVertexArrays(1, &vertices.vao);
  if (vertices.vbo)
    yc::gl::glDeleteBuffers(1, &vertices.vbo);
  if (indicies.ebo)
    yc::gl::glDeleteBuffers(1, &indicies.ebo);
}

inline yc::graphics::draw_result yc::gl::mesh_buffer::draw(
      const std::shared_ptr<yc::graphics::render_pass> & pass)
{
  if (pass && !pass->on_mesh(*this, material))
  {
    return yc::graphics::draw_result();
  }

  yc::gl::glBindVertexArray(vertices.vao);
  yc::gl::glDrawElements(
        vertices.type, vertices.count, indicies.type, indicies.offset);
  yc::gl::glBindVertexArray(0);

  return { 1, static_cast<size_t>(vertices.count) };
}

yc::graphics::aabb yc::gl::mesh_buffer::get_aabb() const
{
  return vertices.aabb;
}
