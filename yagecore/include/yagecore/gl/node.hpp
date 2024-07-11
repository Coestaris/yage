/** ========================================================================
 *
 * @file node.hpp
 * @date 07-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "GL/gl.h"
#include "yagecore/graphics/node.hpp"
#include "yagecore/gl/graphics.hpp"
#include "glplatform.hpp"

/* Hack to not include all the GLTF parser.
 * This will require making all GLTF includes public. */
namespace fastgltf
{
class Asset;
}

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class node;

class mesh_buffer : public yc::graphics::mesh_buffer
{
protected:
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;

  friend node;
  GLuint vao, vbo, ebo;

public:
  mesh_buffer(
        const std::shared_ptr<yc::gl::graphics> & graphics,
        GLuint                                    _vao,
        GLuint                                    _vbo,
        GLuint                                    _ebo)
  : ctx(graphics)
  , vao(_vao)
  , vbo(_vbo)
  , ebo(_ebo)
  {
  }

  void unload() override
  {
    if (vao)
      yc::gl::glDeleteVertexArrays(1, &vao);
    if (vbo)
      yc::gl::glDeleteBuffers(1, &vbo);
    if (ebo)
      yc::gl::glDeleteBuffers(1, &ebo);
  }

  static inline void draw(GLuint vao_)
  {
    yc::gl::glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  inline void draw() override
  {
    draw(vao);
  }
};

class node : public yc::graphics::node
{
private:
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;
  /* map to translate gltf attribute names to GL attribute locations
   * (that used in users' shaders) */
  std::unordered_map<std::string, GLint> attribute_map;
  /* Little optimization to avoid recursive tree traversal,
   * we just store all the flat VAOs to draw them in one go. */
  std::vector<GLuint> flat_vaos_to_draw;

  bool load_gltf(const yc::util::uri & uri);
  bool parse_gltf(const fastgltf::Asset & asset);
  void flat_vaos(std::vector<GLuint> & vaos);

public:
  ~node() override = default;
  explicit node(
        std::string                                     name,
        std::shared_ptr<yc::app::asset_cacher>          cacher,
        const std::shared_ptr<yc::graphics::graphics> & graphics,
        type                                            type = type::TOPLEVEL)
  : yc::graphics::node(std::move(name), std::move(cacher), graphics, type)
  , ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics))
  {
  }

  yc::error load() override;
  void      unload() override;

  inline void draw() override
  {
#if USE_DEBUG
    if (!live)
    {
      return;
    }
#endif

    /* Little optimization */
    if (type_ == yc::graphics::node::type::TOPLEVEL)
    {
      for (auto vao : flat_vaos_to_draw)
      {
        mesh_buffer::draw(vao);
      }
    }
    else
    {
      yc::graphics::node::draw();
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}