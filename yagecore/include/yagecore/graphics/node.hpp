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

#include "yagecore/app/asset.hpp"
#include "yagecore/util/uri.hpp"
#include "yagecore/graphics/graphics.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class material
{
public:
  material()          = default;
  virtual ~material() = default;
};

class image
{
public:
  image()          = default;
  virtual ~image() = default;
};

class mesh_buffer
{
public:
  mesh_buffer()          = default;
  virtual ~mesh_buffer() = default;

  virtual void unload() = 0;
  virtual void draw()   = 0;
};

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

  std::vector<std::shared_ptr<mesh_buffer>> meshes;
  std::vector<std::shared_ptr<node>>        children;
  // std::unordered_map<std::string, image>                        images;
  // std::unordered_map<std::string, std::shared_ptr<material>>    materials;

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

  virtual void draw()
  {
    if (!live)
    {
      return;
    }

    for (auto & mesh : meshes)
    {
      mesh->draw();
    }

    for (auto & child : children)
    {
      child->draw();
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}