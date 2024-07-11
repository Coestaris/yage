/** ========================================================================
 *
 * @file node.hpp
 * @date 07-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <optional>
#include "GL/gl.h"
#include "yagecore/graphics/node.hpp"
#include "yagecore/gl/graphics.hpp"
#include "glplatform.hpp"

/* Hack to not include all the GLTF parser.
 * This will require making all GLTF includes public. */
namespace fastgltf
{
class Asset;
class Primitive;
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

  GLuint vbo, ebo;

public:
  class drawable_mesh
  {
    friend node;
    friend mesh_buffer;

  private:
    GLuint  vao;
    GLsizei vertex_count;

    /* Attention: Offset already in bytes! */
    void * index_offset;

  public:
    drawable_mesh(GLuint _vao, size_t _vertex_count, size_t _index_offset)
    : vao(_vao)
    , vertex_count(_vertex_count)
    , index_offset(reinterpret_cast<void *>(_index_offset * sizeof(GLuint)))
    {
    }
  } drawable;

  mesh_buffer(
        const std::shared_ptr<yc::gl::graphics> & graphics,
        GLuint                                    _vao,
        GLuint                                    _vbo,
        GLuint                                    _ebo,
        size_t                                    _vertex_count,
        size_t                                    _index_offset)
  : ctx(graphics)
  , vbo(_vbo)
  , ebo(_ebo)
  , drawable(_vao, _vertex_count, _index_offset)
  {
  }

  void unload() override
  {
    if (drawable.vao)
      yc::gl::glDeleteVertexArrays(1, &drawable.vao);
    if (vbo)
      yc::gl::glDeleteBuffers(1, &vbo);
    if (ebo)
      yc::gl::glDeleteBuffers(1, &ebo);
  }

  static inline void draw(const drawable_mesh & info)
  {
    yc::gl::glBindVertexArray(info.vao);
    yc::gl::glDrawElements(
          GL_TRIANGLES, info.vertex_count, GL_UNSIGNED_INT, info.index_offset);
    yc::gl::glBindVertexArray(0);
  }

  inline void draw() override
  {
    draw(drawable);
  }
};

class node : public yc::graphics::node
{
public:
  enum node_layout_type
  {
    POSITION, /* Always vec3 */
    NORMAL,   /* Always vec3 */
    TANGENT,  /* Always vec3 */
    TEXCOORD, /* Always vec2 */
    COLOR,    /* Always vec4 */
    JOINTS,   /* Always ivec4 */
    WEIGHTS,  /* Always vec4 */
  };

  class node_layout
  {
  private:
    node_layout_type type;
    GLint            location;
    bool             required;

  public:
    node_layout(node_layout_type _type, GLint _location, bool _required = true)
    : type(_type)
    , location(_location)
    , required(_required)
    {
    }

    inline node_layout_type get_type() const
    {
      return type;
    }

    inline GLint get_location() const
    {
      return location;
    }

    inline bool get_required() const
    {
      return required;
    }

    size_t get_size() const
    {
      switch (type)
      {
        case POSITION:
        case NORMAL:
        case TANGENT:
          return 3 * sizeof(GLfloat);
        case TEXCOORD:
          return 2 * sizeof(GLfloat);
        case COLOR:
          return 4 * sizeof(GLfloat);
        case JOINTS:
          return 4 * sizeof(GLint);
        case WEIGHTS:
          return 4 * sizeof(GLfloat);
      }
      return 0;
    }
  };

private:
  struct vertex
  {
    glm::vec3  position;
    glm::vec3  normal;
    glm::vec3  tangent;
    glm::vec2  uvs;
    glm::vec4  color;
    glm::ivec4 joints;
    glm::vec4  weights;
  };

  class layout
  {
  private:
    std::vector<node_layout> items;

    std::optional<node_layout> get(node_layout_type type) const
    {
      for (auto & item : items)
      {
        if (item.get_type() == type)
        {
          return item;
        }
      }
      return std::nullopt;
    }

  public:
    layout(const std::vector<node_layout> & layout_)
    : items(layout_) {};

    bool need_type(node_layout_type type) const
    {
      return get(type).has_value();
    }

    bool required(node_layout_type type) const
    {
      if (auto item = get(type))
      {
        return item->get_required();
      }
      return false;
    }

    /* Implement iterator */
    std::vector<node_layout>::const_iterator begin() const
    {
      return items.begin();
    }

    std::vector<node_layout>::const_iterator end() const
    {
      return items.end();
    }
  };

  static layout layout;

public:
  static void set_layout(const std::vector<node_layout> & layout_)
  {
    layout = layout_;
  }

private:
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;
  /* map to translate gltf attribute names to GL attribute locations
   * (that used in users' shaders) */
  std::unordered_map<std::string, GLint> attribute_map;
  /* Little optimization to avoid recursive tree traversal,
   * we just store all the flat VAOs to draw them in one go. */
  std::vector<mesh_buffer::drawable_mesh> flat_drawables;

  bool load_gltf(const yc::util::uri & uri);
  bool parse_gltf(const fastgltf::Asset & asset);
  void flat_drawable(std::vector<mesh_buffer::drawable_mesh> & drawables);

  bool process_position(
        const fastgltf::Primitive & primitive, std::vector<vertex> & verticies);
  bool process_normal(
        const fastgltf::Primitive & primitive, std::vector<vertex> & verticies);
  bool process_tangent(
        const fastgltf::Primitive & primitive, std::vector<vertex> & verticies);
  bool process_texcoord(
        const fastgltf::Primitive & primitive, std::vector<vertex> & verticies);
  bool process_color(
        const fastgltf::Primitive & primitive, std::vector<vertex> & verticies);
  bool process_joints(
        const fastgltf::Primitive & primitive, std::vector<vertex> & verticies);
  bool process_weights(
        const fastgltf::Primitive & primitive, std::vector<vertex> & verticies);
  std::shared_ptr<yc::gl::mesh_buffer> make_buffer(
        const std::vector<vertex> & verticies,
        const std::vector<GLuint> & indices);

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
      for (auto drawable : flat_drawables)
      {
        mesh_buffer::draw(drawable);
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