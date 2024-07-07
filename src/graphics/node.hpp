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
#include "asset.hpp"
#include "core/util/uri.hpp"
#include "graphics.hpp"

namespace yage::graphics
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

class node : public yage::graphics::asset
{
public:
  class data
  {
  private:
    yage::core::util::uri uri {};

  public:
    void set_uri(yage::core::util::uri _uri)
    {
      uri = _uri;
    }

    yage::core::util::uri get_uri() const
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
  static bool load_gltf(
        const yage::core::util::uri & uri);

protected:
  yage::graphics::graphics * graphics;
  data                       sources;

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
  material * mat;

protected:
  node(std::string                    name,
       yage::graphics::asset_cacher * cacher,
       yage::graphics::graphics *     graphics)
  : asset(std::move(name), cacher)
  , graphics(graphics)
  {
  }

public:
  void set_sources(const data & _sources)
  {
    if (!(sources == _sources))
    {
      sources = _sources;
      live    = false;
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}