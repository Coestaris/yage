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
#include "yagecore/graphics/render_pass.hpp"
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

    GLuint  vao;
    GLenum  indicies_type;
    GLsizei vertex_count;

    std::shared_ptr<yc::graphics::material> material;

    /* Attention: Offset already in bytes! */
    void * index_offset;

    drawable_mesh(
          GLuint                                          _vao,
          size_t                                          _vertex_count,
          size_t                                          _index_offset,
          GLenum                                          _indicies_type,
          const std::shared_ptr<yc::graphics::material> & _material)
    : vao(_vao)
    , indicies_type(_indicies_type)
    , vertex_count(_vertex_count)
    , material(_material)
    , index_offset(reinterpret_cast<void *>(_index_offset * sizeof(GLuint))) {};
  } drawable;

  mesh_buffer(
        const std::shared_ptr<yc::gl::graphics> &       graphics,
        GLuint                                          vao,
        GLuint                                          _vbo,
        GLuint                                          _ebo,
        size_t                                          vertex_count,
        size_t                                          index_offset,
        GLenum                                          indicies_type,
        const std::shared_ptr<yc::graphics::material> & material)
  : ctx(graphics)
  , vbo(_vbo)
  , ebo(_ebo)
  , drawable(vao, vertex_count, index_offset, indicies_type, material)
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

  static inline void
  draw(const std::shared_ptr<yc::graphics::render_pass> & pass,
       const drawable_mesh &                              info)
  {
    pass->on_mesh(info.material);

    yc::gl::glBindVertexArray(info.vao);
    yc::gl::glDrawElements(
          GL_TRIANGLES, info.vertex_count, info.indicies_type,
          info.index_offset);
    yc::gl::glBindVertexArray(0);
  }

  inline void
  draw(const std::shared_ptr<yc::graphics::render_pass> & pass) override
  {
    draw(pass, drawable);
  }
};

class node : public yc::graphics::node
{
public:
  class attr_layout_item
  {
  public:
    enum type
    {
      POSITION, /* Always vec3 */
      NORMAL,   /* Always vec3 */
      TANGENT,  /* Always vec3 */
      TEXCOORD, /* Always vec2 */
      COLOR,    /* Always vec4 */
      JOINTS,   /* Always ivec4 */
      WEIGHTS,  /* Always vec4 */
    };

  private:
    type  type_;
    GLint location;
    bool  required;

  public:
    attr_layout_item(type _type, GLint _location, bool _required = true)
    : type_(_type)
    , location(_location)
    , required(_required)
    {
    }

    inline type get_type() const
    {
      return type_;
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
      switch (type_)
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

  class attr_layout
  {
  private:
    std::vector<attr_layout_item> items;

    [[nodiscard]] std::optional<attr_layout_item>
    get(attr_layout_item::type type_) const
    {
      for (auto & item : items)
      {
        if (item.get_type() == type_)
        {
          return item;
        }
      }
      return std::nullopt;
    }

  public:
    attr_layout() = default;
    attr_layout(const std::vector<attr_layout_item> & layout_)
    : items(layout_) {};

    [[nodiscard]] bool need_type(attr_layout_item::type type_) const
    {
      return get(type_).has_value();
    }

    [[nodiscard]] bool required(attr_layout_item::type type_) const
    {
      if (auto item = get(type_))
      {
        return item->get_required();
      }
      return false;
    }

    [[nodiscard]] GLint location(attr_layout_item::type type_) const
    {
      if (auto item = get(type_))
      {
        return item->get_location();
      }
      return -1;
    }

    /* Implement iterator */
    [[nodiscard]] std::vector<attr_layout_item>::const_iterator begin() const
    {
      return items.begin();
    }

    [[nodiscard]] std::vector<attr_layout_item>::const_iterator end() const
    {
      return items.end();
    }
  };

  static attr_layout layout;

public:
  static void set_layout(const std::vector<attr_layout_item> & layout_)
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
        const fastgltf::Asset &     asset,
        const fastgltf::Primitive & primitive,
        std::vector<vertex> &       verticies,
        size_t &                    initial_vtx);
  bool process_normal(
        const fastgltf::Asset &     asset,
        const fastgltf::Primitive & primitive,
        std::vector<vertex> &       verticies,
        size_t &                    initial_vtx);
  bool process_tangent(
        const fastgltf::Asset &     asset,
        const fastgltf::Primitive & primitive,
        std::vector<vertex> &       verticies,
        size_t &                    initial_vtx);
  bool process_texcoord(
        const fastgltf::Asset &     asset,
        const fastgltf::Primitive & primitive,
        std::vector<vertex> &       verticies,
        size_t &                    initial_vtx);
  bool process_color(
        const fastgltf::Asset &     asset,
        const fastgltf::Primitive & primitive,
        std::vector<vertex> &       verticies,
        size_t &                    initial_vtx);
  bool process_joints(
        const fastgltf::Asset &     asset,
        const fastgltf::Primitive & primitive,
        std::vector<vertex> &       verticies,
        size_t &                    initial_vtx);
  bool process_weights(
        const fastgltf::Asset &     asset,
        const fastgltf::Primitive & primitive,
        std::vector<vertex> &       verticies,
        size_t &                    initial_vtx);
  std::shared_ptr<yc::gl::mesh_buffer> make_buffer(
        const std::vector<vertex> & verticies,
        const std::vector<GLuint> & indices);

public:
  ~node() override = default;
  explicit
  node(std::string                                     name,
       std::shared_ptr<yc::app::asset_cacher>          cacher,
       const std::shared_ptr<yc::graphics::graphics> & graphics,
       type                                            type = type::TOPLEVEL)
  : yc::graphics::node(std::move(name), std::move(cacher), graphics, type)
  , ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics))
  {
  }

  yc::error load() override;
  void      unload() override;

  inline void
  draw(const std::shared_ptr<yc::graphics::render_pass> & pass) override
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
      for (auto & drawable : flat_drawables)
      {
        mesh_buffer::draw(pass, drawable);
      }
    }
    else
    {
      yc::graphics::node::draw(pass);
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}