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
class mesh_buffer : public yc::graphics::mesh_buffer
{
private:
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;

  /* ATTENTION: VAO is handled by parent Node class */
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
    if (vbo)
      yc::gl::glDeleteBuffers(1, &vbo);
    if (ebo)
      yc::gl::glDeleteBuffers(1, &ebo);
  }

  inline void draw() override
  {
    yc::gl::glBindVertexArray(vao);
    yc::gl::glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    // ctx->glBindVertexArray(0);
  }
};

class node : public yc::graphics::node
{
private:
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;
  /* gltf index -> GL buffer */
  std::unordered_map<size_t, GLuint> vbos;
  /* map to translate gltf attribute names to GL attribute locations
   * (that used in users' shaders) */
  std::unordered_map<std::string, GLint> attribute_map;

  bool load_gltf(const yc::util::uri & uri);
  bool parse_gltf(const fastgltf::Asset & asset);

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
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}