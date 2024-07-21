/** ========================================================================
 *
 * @file node.hpp
 * @date 28-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <memory>
#include <utility>
#include "glm/mat4x4.hpp"

#include "yagecore/graphics/draw_result.hpp"
#include "yagecore/graphics/material.hpp"
#include "yagecore/graphics/mesh_buffer.hpp"
#include "yagecore/app/asset.hpp"
#include "yagecore/util/uri.hpp"
#include "yagecore/graphics/graphics.hpp"
#include "yagecore/graphics/texture.hpp"
#include "yagecore/graphics/aabb.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class render_pass;

class node : public yc::app::asset
{
public:
  enum class type
  {
    TOPLEVEL,
    SCENE,
    NODE,
    MESH,
  };

  class data
  {
    yc::util::uri uri {};
    std::string   work_dir {};

  public:
    void set_work_dir(const std::string & work_dir);
    void set_uri(yc::util::uri uri);

    [[nodiscard]] yc::util::uri get_uri() const;
    [[nodiscard]] std::string get_work_dir() const;

    bool operator==(const data & other) const;
    bool empty() const;
  };

protected:
  std::shared_ptr<yc::graphics::graphics> graphics;

  data sources;
  type type_;

  /* These texture are handled by the node itself.
   * Any asset factory has no idea about them. */
  std::vector<std::shared_ptr<yc::graphics::texture>>     textures;
  std::vector<std::shared_ptr<yc::graphics::material>>    materials;
  std::shared_ptr<yc::graphics::mesh_buffer> mesh;
  // std::shared_ptr<yc::graphics::mesh_buffer> light;
  // std::shared_ptr<yc::graphics::mesh_buffer> camera;
  std::vector<std::shared_ptr<node>>                      children;

  /* Transformation matrix for the meshes.
   * Note, that this matrix IS used to render meshes directly.
   * It means, that it should store all the transformations from the
   * root node to the current one. */
  glm::mat4 transform;

  node(std::string                             name,
       std::shared_ptr<yc::app::asset_cacher>  cacher,
       std::shared_ptr<yc::graphics::graphics> graphics,
       type                                    type = type::TOPLEVEL);

public:
  void set_sources(const data & _sources);

  type get_type() const;
  draw_result draw(const std::shared_ptr<yc::graphics::render_pass> & pass);

  virtual yc::graphics::aabb get_aabb() const = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}