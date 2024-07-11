/** ========================================================================
 *
 * @file node.hpp
 * @date 07-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <cinttypes>
#include "yagecore/log.hpp"
#include "yagecore/error.hpp"
#include "yagecore/gl/node.hpp"
#include "yagecore/fs.hpp"

#include "glm/gtc/type_ptr.hpp"

/* Suspend -Werror=redundant-move */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-move"
#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"
#pragma GCC diagnostic pop

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::error yc::gl::node::load()
{
  live = true;

  logging(LOG_DEBUG, "Loading GLTF node: %s", get_name().c_str());

  if (!load_gltf(sources.get_uri()))
  {
    logging(LOG_ERROR, "Failed to load GLTF node: %s", get_name().c_str());
    return yc::error::EXTERNAL_API_ERROR;
  }

  live   = true;
  loaded = true;

  return yc::error::OK;
}

void yc::gl::node::unload()
{
  flat_vaos_to_draw.clear();

  /* Unload all meshes */
  for (auto & mesh : meshes)
  {
    mesh->unload();
  }

  /* Unload all children */
  for (auto & child : children)
  {
    child->unload();
  }

  loaded = false;
}

bool yc::gl::node::load_gltf(const yc::util::uri & uri)
{
  fastgltf::Parser parser;
  auto             data    = fastgltf::MappedGltfFile::FromPath(uri.get_uri());
  bool             success = true;
  if (auto error = data.error(); error != fastgltf::Error::None)
  {
    logging(
          LOG_ERROR, "Failed to read GLTF data from URI: %s",
          uri.get_uri().c_str());
    success = false;
  }
  if (!success)
  {
    return false;
  }

  success          = true;
  auto asset_error = parser.loadGltf(
        data.get(), yc::basedir(uri.get_uri()),
        fastgltf::Options::LoadExternalBuffers);
  if (auto error = asset_error.error(); error != fastgltf::Error::None)
  {
    logging(
          LOG_ERROR, "Failed to parse GLTF data from URI: %s",
          uri.get_uri().c_str());
    success = false;
  }
  if (!success)
  {
    return false;
  }

  return parse_gltf(asset_error.get());
}

bool yc::gl::node::parse_gltf(const fastgltf::Asset & asset)
{
  for (auto & gltf_scene : asset.scenes)
  {
    auto scene = std::make_shared<yc::gl::node>(
          gltf_scene.name.c_str(), cacher, graphics, type::SCENE);
    children.push_back(scene);

    for (auto index : gltf_scene.nodeIndices)
    {
      auto & gltf_node = asset.nodes[index];
      auto   node      = std::make_shared<yc::gl::node>(
            gltf_node.name.c_str(), cacher, graphics, type::NODE);

      /* Transform matrix to glm::mat4 */
      glm::mat4 transform;
      if (std::holds_alternative<fastgltf::math::fmat4x4>(gltf_node.transform))
      {
        auto transform_matrix
              = std::get<fastgltf::math::fmat4x4>(gltf_node.transform);
        transform = glm::make_mat4(&transform_matrix[0][0]);
      }
      else if (std::holds_alternative<fastgltf::TRS>(gltf_node.transform))
      {
        auto trs = std::get<fastgltf::TRS>(gltf_node.transform);
        transform
              = glm::translate(
                      glm::mat4(1.0f),
                      glm::vec3(
                            trs.translation[0], trs.translation[1],
                            trs.translation[2]))
                * glm::rotate(
                      glm::mat4(1.0f), trs.rotation[3],
                      glm::vec3(
                            trs.rotation[0], trs.rotation[1], trs.rotation[2]))
                * glm::scale(
                      glm::mat4(1.0f),
                      glm::vec3(trs.scale[0], trs.scale[1], trs.scale[2]));
      }

      node->transform = transform;
      scene->children.push_back(node);

      std::vector<uint32_t> indices;

      struct vertex
      {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 normal;
        glm::vec2 uvs;
      };

      std::vector<vertex> vertices;

      /* Load meshes */
      if (gltf_node.meshIndex)
      {
        auto & gltf_mesh = asset.meshes[gltf_node.meshIndex.value()];

        /* Load vertices */
        for (auto & gltf_primitive : gltf_mesh.primitives)
        {
          /* Process indices */
          if (!gltf_primitive.indicesAccessor.has_value())
          {
            logging(
                  LOG_ERROR, "No indices found in GLTF node: %s",
                  get_name().c_str());
            return false;
          }
          auto & gltf_index_accessor
                = asset.accessors[gltf_primitive.indicesAccessor.value()];

          size_t start_index = indices.size();
          indices.reserve(indices.size() + gltf_index_accessor.count);

          fastgltf::iterateAccessor<std::uint32_t>(
                asset, gltf_index_accessor, [&](std::uint32_t idx) {
                  indices.push_back(idx + start_index);
                });

          /* Process positions */
          if (gltf_primitive.findAttribute("POSITION")
              == gltf_primitive.attributes.end())
          {
            logging(
                  LOG_ERROR, "No positions found in GLTF node: %s",
                  get_name().c_str());
            return false;
          }
          auto & gltf_pos_accessor
                = asset.accessors[gltf_primitive.findAttribute("POSITION")
                                        ->accessorIndex];

          size_t initial_vtx = vertices.size();
          vertices.resize(vertices.size() + gltf_pos_accessor.count);

          fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                asset, gltf_pos_accessor,
                [&](fastgltf::math::fvec3 pos, std::size_t idx) {
                  vertex v   = {};
                  v.position = glm::vec3(pos[0], pos[1], pos[2]);
                  vertices[initial_vtx + index] = v;
                });

          /* Process normals */
          if (gltf_primitive.findAttribute("NORMAL")
              != gltf_primitive.attributes.end())
          {
            auto & gltf_normal_accessor
                  = asset.accessors[gltf_primitive.findAttribute("NORMAL")
                                          ->accessorIndex];
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                  asset, gltf_normal_accessor,
                  [&](fastgltf::math::fvec3 normal, std::size_t idx) {
                    vertices[initial_vtx + idx].normal
                          = glm::vec3(normal[0], normal[1], normal[2]);
                  });
          }
          else
          {
            logging(
                  LOG_WARN, "No normals found in GLTF node: %s",
                  get_name().c_str());
          }

          /* Process colors */
          if (gltf_primitive.findAttribute("COLOR_0")
              != gltf_primitive.attributes.end())
          {
            auto & gltf_color_accessor
                  = asset.accessors[gltf_primitive.findAttribute("COLOR_0")
                                          ->accessorIndex];

            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                  asset, gltf_color_accessor,
                  [&](fastgltf::math::fvec3 color, std::size_t idx) {
                    vertices[initial_vtx + idx].color
                          = glm::vec3(color[0], color[1], color[2]);
                  });
          }
          else
          {
            logging(
                  LOG_WARN, "No colors found in GLTF node: %s",
                  get_name().c_str());
          }

          /* Process UVs */
          if (gltf_primitive.findAttribute("TEXCOORD_0")
              != gltf_primitive.attributes.end())
          {
            auto & gltf_uv_accessor
                  = asset.accessors[gltf_primitive.findAttribute("TEXCOORD_0")
                                          ->accessorIndex];
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                  asset, gltf_uv_accessor,
                  [&](fastgltf::math::fvec2 uv, std::size_t idx) {
                    vertices[initial_vtx + idx].uvs = glm::vec2(uv[0], uv[1]);
                  });
          }
          else
          {
            logging(
                  LOG_WARN, "No UVs found in GLTF node: %s",
                  get_name().c_str());
          }
        }
      }

      /* Create Vertex Array Object */
      GLuint vao, vbo, ebo;

      yc::gl::glGenVertexArrays(1, &vao);
      yc::gl::glGenBuffers(1, &vbo);
      yc::gl::glGenBuffers(1, &ebo);

      /* TODO: Find a better way to calculate size */
      std::vector<float> positions;
      for (auto & vertex : vertices)
      {
        positions.push_back(vertex.position.x);
        positions.push_back(vertex.position.y);
        positions.push_back(vertex.position.z);
        positions.push_back(vertex.normal.x);
        positions.push_back(vertex.normal.y);
        positions.push_back(vertex.normal.z);
        positions.push_back(vertex.uvs.x);
        positions.push_back(vertex.uvs.y);
      }

      yc::gl::glBindVertexArray(vao);
      yc::gl::glBindBuffer(GL_ARRAY_BUFFER, vbo);
      yc::gl::glBufferData(
            GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(),
            GL_STATIC_DRAW);
      yc::gl::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      yc::gl::glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
            indices.data(), GL_STATIC_DRAW);

      yc::gl::glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            reinterpret_cast<void *>(0));
      yc::gl::glEnableVertexAttribArray(0);
      yc::gl::glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            reinterpret_cast<void *>(3 * sizeof(float)));
      yc::gl::glEnableVertexAttribArray(1);
      yc::gl::glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
            reinterpret_cast<void *>(6 * sizeof(float)));
      yc::gl::glEnableVertexAttribArray(2);

      auto mesh_buffer
            = std::make_shared<yc::gl::mesh_buffer>(ctx, vao, vbo, ebo);
      node->meshes.push_back(mesh_buffer);
    }
  }

  flat_vaos(flat_vaos_to_draw);

  return true;
}
void yc::gl::node::flat_vaos(std::vector<GLuint> & vaos)
{
  for (auto & mesh : meshes)
  {
    vaos.push_back(dynamic_cast<yc::gl::mesh_buffer *>(mesh.get())->vao);
  }

  for (auto & child : children)
  {
    dynamic_cast<yc::gl::node *>(child.get())->flat_vaos(vaos);
  }
}
