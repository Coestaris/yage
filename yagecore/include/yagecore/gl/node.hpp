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
#include "yagecore/gl/mesh_buffer.hpp"

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
class node final : public yc::graphics::node
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
    attr_layout_item(type type, GLint location, bool required = true);

    [[nodiscard]] type   get_type() const;
    [[nodiscard]] GLint  get_location() const;
    [[nodiscard]] bool   get_required() const;
    [[nodiscard]] size_t get_size() const;
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
    std::vector<attr_layout_item> items;

    [[nodiscard]] std::optional<attr_layout_item>
    get(attr_layout_item::type type_) const;

  public:
    attr_layout() = default;
    attr_layout(const std::vector<attr_layout_item> & layout);

    [[nodiscard]] bool  need_type(attr_layout_item::type type) const;
    [[nodiscard]] bool  required(attr_layout_item::type type) const;
    [[nodiscard]] GLint location(attr_layout_item::type type) const;

    /* Implement iterator */
    [[nodiscard]] std::vector<attr_layout_item>::const_iterator begin() const;
    [[nodiscard]] std::vector<attr_layout_item>::const_iterator end() const;
  };

  static attr_layout layout;

public:
  static void set_layout(const std::vector<attr_layout_item> & layout);

private:
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;
  /* map to translate gltf attribute names to GL attribute locations
   * (that used in users' shaders) */
  std::unordered_map<std::string, GLint> attribute_map;

  yc::graphics::aabb aabb {};

  bool load_gltf(const yc::data_source & source);
  bool parse_gltf(const fastgltf::Asset & asset);

  bool process_position(
        const fastgltf::Asset &     asset,
        const fastgltf::Primitive & primitive,
        std::vector<vertex> &       verticies,
        size_t &                    initial_vtx,
        yc::graphics::aabb &        aabb);
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
        const std::vector<GLuint> & indices,
        const yc::graphics::aabb &  aabb);

public:
  ~node() override = default;
  explicit
  node(std::string                                     name,
       std::shared_ptr<yc::app::asset_cacher>          cacher,
       const std::shared_ptr<yc::graphics::graphics> & graphics,
       type                                            type = type::TOPLEVEL);

  yc::error load() override;
  void      unload() override;

  [[nodiscard]] yc::graphics::aabb get_aabb() const override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}