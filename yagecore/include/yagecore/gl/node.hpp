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
private:
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;

  GLuint  vbo, ebo, vao;
  GLenum  indicies_type;
  GLsizei vertex_count;

  friend node;

  std::shared_ptr<yc::graphics::material> material;

  /* Attention: Offset already in bytes! */
  void * index_offset;

public:
  mesh_buffer(
        const std::shared_ptr<yc::gl::graphics> &       graphics,
        GLuint                                          vao,
        GLuint                                          vbo,
        GLuint                                          ebo,
        size_t                                          vertex_count,
        size_t                                          index_offset,
        GLenum                                          indicies_type,
        const std::shared_ptr<yc::graphics::material> & material)
  : ctx(graphics)
  , vbo(vbo)
  , ebo(ebo)
  , vao(vao)
  , indicies_type(indicies_type)
  , vertex_count(vertex_count)
  , material(material)
  , index_offset(reinterpret_cast<void *>(index_offset * sizeof(GLuint))) {};

  void unload() override
  {
    if (vao)
      yc::gl::glDeleteVertexArrays(1, &vao);
    if (vbo)
      yc::gl::glDeleteBuffers(1, &vbo);
    if (ebo)
      yc::gl::glDeleteBuffers(1, &ebo);
  }

  inline void
  draw(const std::shared_ptr<yc::graphics::render_pass> & pass) override
  {
    if (!pass->on_mesh(*this, material))
    {
      return;
    }

    yc::gl::glBindVertexArray(vao);
    yc::gl::glDrawElements(
          GL_TRIANGLES, vertex_count, indicies_type,
          index_offset);
    yc::gl::glBindVertexArray(0);
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

  bool load_gltf(const yc::util::uri & uri);
  bool parse_gltf(const fastgltf::Asset & asset);

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
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}