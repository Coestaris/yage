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
};

class node : public yc::app::asset
{
public:
  class data
  {
  private:
    yc::util::uri uri {};

  public:
    void set_uri(yc::util::uri _uri)
    {
      uri = _uri;
    }

    yc::util::uri get_uri() const
    {
      return uri;
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
  static bool load_gltf(const yc::util::uri & uri);

protected:
  std::shared_ptr<yc::graphics::graphics> graphics;
  data                     sources;

  // storage for all the data on a given glTF file
  std::unordered_map<std::string, std::shared_ptr<mesh_buffer>> meshes;
  std::unordered_map<std::string, std::shared_ptr<node>>        nodes;
  std::unordered_map<std::string, image>                        images;
  std::unordered_map<std::string, std::shared_ptr<material>>    materials;

  // nodes that dont have a parent, for iterating through the file in tree order
  std::vector<std::shared_ptr<node>> topNodes;

  //  std::vector<VkSampler> samplers;

  //  DescriptorAllocatorGrowable descriptorPool;
  //
  //  AllocatedBuffer materialDataBuffer;

  glm::mat4  transform;
  // material * mat;

protected:
  node(std::string              name,
       std::shared_ptr<yc::app::asset_cacher>  cacher,
       std::shared_ptr<yc::graphics::graphics> graphics)
  : asset(std::move(name), std::move(cacher))
  , graphics(std::move(graphics))
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
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}