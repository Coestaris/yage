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
  flat_drawables.clear();

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
      std::vector<vertex>   vertices;

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

          if (!process_position(gltf_primitive, vertices))
            return false;
          if (!process_normal(gltf_primitive, vertices))
            return false;
          if (!process_tangent(gltf_primitive, vertices))
            return false;
          if (!process_texcoord(gltf_primitive, vertices))
            return false;
          if (!process_color(gltf_primitive, vertices))
            return false;
          if (!process_joints(gltf_primitive, vertices))
            return false;
          if (!process_weights(gltf_primitive, vertices))
            return false;
        }
      }

      auto mesh_buffer = make_buffer(vertices, indices);
      if (!mesh_buffer)
      {
        logging(
              LOG_ERROR, "Failed to create mesh buffer for GLTF node: %s",
              get_name().c_str());
        return false;
      }

      node->meshes.push_back(mesh_buffer);
    }
  }

  flat_drawable(flat_drawables);

  return true;
}
void yc::gl::node::flat_drawable(
      std::vector<mesh_buffer::drawable_mesh> & drawables)
{
  for (auto & mesh : meshes)
  {
    drawables.push_back(
          dynamic_cast<yc::gl::mesh_buffer *>(mesh.get())->drawable);
  }

  for (auto & child : children)
  {
    dynamic_cast<yc::gl::node *>(child.get())->flat_drawable(drawables);
  }
}

bool yc::gl::node::process_position(
      const fastgltf::Primitive & primitive, std::vector<vertex> & verticies)
{
  if (!layout.need_type(POSITION))
  {
    return true;
  }

  if (primitive.findAttribute("POSITION") == primitive.attributes.end())
  {
    if (layout.required(POSITION))
    {
      logging(
            LOG_ERROR, "No positions found in GLTF node: %s",
            get_name().c_str());
      return false;
    }
    else
    {
      logging(
            LOG_ERROR,
            "No positions found in GLTF node: %s. All vectors will "
            "be zeroed",
            get_name().c_str());
    }
  }

  auto & gltf_pos_accessor
        = asset.accessors[primitive.findAttribute("POSITION")->accessorIndex];

  size_t initial_vtx = verticies.size();
  verticies.resize(verticies.size() + gltf_pos_accessor.count);

  fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
        asset, gltf_pos_accessor,
        [&](fastgltf::math::fvec3 pos, std::size_t idx) {
          vertex v                       = {};
          v.position                     = glm::vec3(pos[0], pos[1], pos[2]);
          verticies[initial_vtx + index] = v;
        });
}

bool yc::gl::node::process_normal(
      const fastgltf::Primitive & primitive, std::vector<vertex> & verticies)
{
  if (!layout.need_type(NORMAL))
  {
    return true;
  }

  if (primitive.findAttribute("NORMAL") == primitive.attributes.end())
  {
    if (layout.required(NORMAL))
    {
      logging(
            LOG_ERROR, "No ccc found in GLTF node: %s",
            get_name().c_str());
      return false;
    }
    else
    {
      logging(
            LOG_ERROR,
            "No normals found in GLTF node: %s. All vectors will "
            "be zeroed",
            get_name().c_str());
    }
  }

  auto & gltf_normal_accessor
        = asset.accessors[primitive.findAttribute("NORMAL")->accessorIndex];
  fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
        asset, gltf_normal_accessor,
        [&](fastgltf::math::fvec3 normal, std::size_t idx) {
          verticies[initial_vtx + idx].normal
                = glm::vec3(normal[0], normal[1], normal[2]);
        });
}

bool yc::gl::node::process_tangent(
      const fastgltf::Primitive & primitive, std::vector<vertex> & verticies)
{
  if (layout.need_type(TANGENT))
  {
    logging(LOG_ERROR, "Tangent is not currently supported");
    return false;
  }

  return true;
}

bool yc::gl::node::process_texcoord(
      const fastgltf::Primitive & primitive, std::vector<vertex> & verticies)
{

  if (!layout.need_type(TEXCOORD))
  {
    return true;
  }

  if (primitive.findAttribute("TEXCOORD_0") == primitive.attributes.end())
  {
    if (layout.required(TEXCOORD))
    {
      logging(
            LOG_ERROR, "No texture coordinates found in GLTF node: %s",
            get_name().c_str());
      return false;
    }
    else
    {
      logging(
            LOG_ERROR,
            "No texture coordinates found in GLTF node: %s. All vectors will "
            "be zeroed",
            get_name().c_str());
    }
  }

  auto & gltf_uv_accessor
        = asset.accessors[primitive.findAttribute("TEXCOORD_0")->accessorIndex];
  fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
        asset, gltf_uv_accessor,
        [&](fastgltf::math::fvec2 uv, std::size_t idx) {
          verticies[initial_vtx + idx].uvs = glm::vec2(uv[0], uv[1]);
        });
}

bool yc::gl::node::process_color(
      const fastgltf::Primitive & primitive, std::vector<vertex> & verticies)
{

  if (!layout.need_type(COLOR))
  {
    return true;
  }

  /* TODO: Try to find a better way to handle color */
  if (primitive.findAttribute("COLOR_0") == primitive.attributes.end())
  {
    if (layout.required(COLOR))
    {
      logging(
            LOG_ERROR, "No colors found in GLTF node: %s",
            get_name().c_str());
      return false;
    }
    else
    {
      logging(
            LOG_ERROR,
            "No colors found in GLTF node: %s. All vectors will "
            "be zeroed",
            get_name().c_str());
    }
  }

  auto & gltf_color_accessor
        = asset.accessors[primitive.findAttribute("COLOR_0")->accessorIndex];

  fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
        asset, gltf_color_accessor,
        [&](fastgltf::math::fvec3 color, std::size_t idx) {
          verticies[initial_vtx + idx].color
                = glm::vec3(color[0], color[1], color[2]);
        });
}

bool yc::gl::node::process_joints(
      const fastgltf::Primitive & primitive, std::vector<vertex> & verticies)
{
  if (layout.need_type(JOINTS))
  {
    logging(LOG_ERROR, "Joints is not currently supported");
    return false;
  }

  return true;
}

bool yc::gl::node::process_weights(
      const fastgltf::Primitive & primitive, std::vector<vertex> & verticies)
{
  if (layout.need_type(WEIGHTS))
  {
    logging(LOG_ERROR, "Weights is not currently supported");
    return false;
  }

  return true;
}

std::shared_ptr<yc::gl::mesh_buffer> yc::gl::node::make_buffer(
      const std::vector<vertex> & verticies,
      const std::vector<GLuint> & indices)
{
  /* Create Vertex Array Object */
  GLuint vao, vbo, ebo;

  yc::gl::glGenVertexArrays(1, &vao);
  yc::gl::glGenBuffers(1, &vbo);
  yc::gl::glGenBuffers(1, &ebo);

  std::vector<float> data;
  for (auto & vertex : verticies)
  {


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

  auto mesh_buffer = std::make_shared<yc::gl::mesh_buffer>(
        ctx, vao, vbo, ebo, indices.size(), 0);

  return std::shared_ptr<yc::gl::mesh_buffer>();
}
