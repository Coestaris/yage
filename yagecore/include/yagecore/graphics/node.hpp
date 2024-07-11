/** ========================================================================
 *
 * @file render_primitive.hpp
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

#include "yagecore/graphics/material.hpp"
#include "yagecore/graphics/mesh_buffer.hpp"
#include "yagecore/app/asset.hpp"
#include "yagecore/util/uri.hpp"
#include "yagecore/graphics/graphics.hpp"
#include "yagecore/graphics/texture.hpp"

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
  private:
    yc::util::uri uri {};
    std::string   work_dir {};

  public:
    void set_work_dir(const std::string & _work_dir)
    {
      work_dir = _work_dir;
    }

    void set_uri(yc::util::uri _uri)
    {
      uri = _uri;
    }

    yc::util::uri get_uri() const
    {
      return uri;
    }

    std::string get_work_dir() const
    {
      return work_dir;
    }

    bool operator==(const data & other) const
    {
      return uri == other.uri;
    }

    bool empty() const
    {
      return uri.empty();
    }
  };

protected:
  std::shared_ptr<yc::graphics::graphics> graphics;

  data sources;
  type type_;

  /* These texture are handled by the node itself.
   * Any asset factory has no idea about them. */
  std::vector<std::shared_ptr<yc::graphics::texture>>     textures;
  std::vector<std::shared_ptr<yc::graphics::material>>    materials;
  std::vector<std::shared_ptr<yc::graphics::mesh_buffer>> meshes;
  std::vector<std::shared_ptr<node>>                      children;

  /* Transformation matrix for the meshes.
   * Note, that this matrix IS used to render meshes directly.
   * It means, that it should store all the transformations from the
   * root node to the current one. */
  glm::mat4 transform;

protected:
  node(std::string                             name,
       std::shared_ptr<yc::app::asset_cacher>  cacher,
       std::shared_ptr<yc::graphics::graphics> graphics,
       type                                    type = type::TOPLEVEL)
  : asset(std::move(name), std::move(cacher))
  , graphics(std::move(graphics))
  , type_(type)
  {
    if (type != type::TOPLEVEL)
    {
      live   = true;
      loaded = true;
    }
  }

public:
  void set_sources(const data & _sources)
  {
    if (!(sources == _sources))
    {
      logging(LOG_INFO, "Node %s marked to reload", name.c_str());
      sources = _sources;
      live    = false;
    }
  }

  type get_type() const
  {
    return type_;
  }

  virtual void draw(const std::shared_ptr<yc::graphics::render_pass> & pass)
  {
#if USE_DEBUG
    if (!live)
    {
      return;
    }
#endif

    for (auto & mesh : meshes)
    {
      mesh->draw(pass);
    }

    for (auto & child : children)
    {
      child->draw(pass);
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}