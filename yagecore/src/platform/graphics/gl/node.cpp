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
  /* Unload all buffers */
  for (auto & vbo : vbos)
  {
    yc::gl::glDeleteBuffers(1, &vbo.second);
  }

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
        data.get(), yc::basedir(uri.get_uri()), fastgltf::Options::None);
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

struct vertex_attribute
{
  size_t                 buffer_index;
  fastgltf::AccessorType type;
  size_t                 count;
  void *                 byte_offset;
  size_t                 byte_stride;
  fastgltf::BufferTarget target;
};

vertex_attribute get_vertex_attribute(
      const fastgltf::Asset &     asset,
      const fastgltf::Primitive & primitive,
      const std::string &         attribute_name)
{
  vertex_attribute attr;

  /* Fail if no attribute */
  for (size_t i = 0; i < primitive.attributes.size(); i++)
  {
    if (std::strcmp(
              primitive.attributes[i].name.c_str(), attribute_name.c_str())
        != 0)
      continue;

    auto & gltf_accessor
          = asset.accessors[primitive.attributes[i].accessorIndex];
    auto & gltf_buffer_view
          = asset.bufferViews[gltf_accessor.bufferViewIndex.value()];

    attr.buffer_index = gltf_buffer_view.bufferIndex;
    attr.type         = gltf_accessor.type;
    attr.count        = gltf_accessor.count;
    attr.byte_offset  = reinterpret_cast<void *>(gltf_buffer_view.byteOffset);
    if (gltf_buffer_view.byteStride)
    {
      attr.byte_stride = gltf_buffer_view.byteStride.value();
    }
    else
    {
      attr.byte_stride = gltf_buffer_view.byteLength / gltf_accessor.count;
    }

    attr.target = gltf_buffer_view.target.value();

    return attr;
  }

  throw std::runtime_error(attribute_name + " attribute not found");
}

vertex_attribute get_indices(
      const fastgltf::Asset & asset, const fastgltf::Primitive & primitive)
{
  vertex_attribute attr;

  if (!primitive.indicesAccessor)
  {
    throw std::runtime_error("No indices accessor");
  }

  /* Fail if no attribute */
  auto & gltf_accessor = asset.accessors[primitive.indicesAccessor.value()];
  auto & gltf_buffer_view
        = asset.bufferViews[gltf_accessor.bufferViewIndex.value()];

  attr.buffer_index = gltf_buffer_view.bufferIndex;
  attr.type         = gltf_accessor.type;
  attr.count        = gltf_accessor.count;
  attr.byte_offset  = reinterpret_cast<void *>(gltf_buffer_view.byteOffset);
  attr.byte_stride  = gltf_buffer_view.byteLength / gltf_accessor.count;

  attr.target = gltf_buffer_view.target.value();

  return attr;
}

bool yc::gl::node::parse_gltf(const fastgltf::Asset & asset)
{
  /* Load all buffer to the GPU */
  for (size_t i = 0; i < asset.buffers.size(); i++)
  {
    auto & gltf_buffer = asset.buffers[i];
    auto   buffer_name = gltf_buffer.name.c_str();

    /* If stores URI - load it */
    std::vector<uint8_t> data;
    if (std::holds_alternative<fastgltf::sources::URI>(gltf_buffer.data))
    {
      /* TODO: This is not valid! */
      auto uri = yc::util::uri(yc::join_path(
            sources.get_work_dir(),
            std::get<fastgltf::sources::URI>(gltf_buffer.data).uri.c_str()));

      uri.read(data);
      if (data.empty())
      {
        logging(
              LOG_ERROR, "Failed to read buffer data from URI: %s",
              uri.get_uri().c_str());
        return false;
      }
    }
    else
    {
      throw std::runtime_error("Unsupported buffer data type");
    }

    /* Load buffer data to the GPU */
    GLuint buffer;

    yc::gl::glGenBuffers(1, &buffer);
    yc::gl::glBindBuffer(GL_ARRAY_BUFFER, buffer);
    yc::gl::glBufferData(
          GL_ARRAY_BUFFER, data.size(), data.data(), GL_STATIC_DRAW);

    vbos[i] = buffer;
  }

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

      /* Load meshes */
      if (gltf_node.meshIndex)
      {
        auto & gltf_mesh = asset.meshes[gltf_node.meshIndex.value()];
        auto   mesh      = std::make_shared<yc::gl::node>(
              gltf_mesh.name.c_str(), cacher, graphics, type::MESH);

        node->children.push_back(mesh);

        /* Load vertices */
        for (auto & gltf_primitive : gltf_mesh.primitives)
        {
          auto pos = get_vertex_attribute(asset, gltf_primitive, "POSITION");
          auto nor = get_vertex_attribute(asset, gltf_primitive, "NORMAL");
          auto tex = get_vertex_attribute(asset, gltf_primitive, "TEXCOORD_0");

          /* Setup Vertex Array Object */
          GLuint vao;
          yc::gl::glGenVertexArrays(1, &vao);
          yc::gl::glBindVertexArray(vao);
          yc::gl::glBindBuffer(GL_ARRAY_BUFFER, vbos[pos.buffer_index]);
          yc::gl::glEnableVertexAttribArray(0);
          yc::gl::glVertexAttribPointer(
                0, 3, GL_FLOAT, GL_FALSE, pos.byte_stride, pos.byte_offset);
          yc::gl::glEnableVertexAttribArray(1);
          yc::gl::glVertexAttribPointer(
                1, 3, GL_FLOAT, GL_FALSE, nor.byte_stride, nor.byte_offset);
          yc::gl::glEnableVertexAttribArray(2);
          yc::gl::glVertexAttribPointer(
                2, 2, GL_FLOAT, GL_FALSE, tex.byte_stride, tex.byte_offset);

          /* Load indices */
          auto   indices = get_indices(asset, gltf_primitive);
          GLuint ebo;
          /* EBO data is in the same buffer as VBO */
          yc::gl::glGenBuffers(1, &ebo);
          yc::gl::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
          yc::gl::glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, indices.count * 2, nullptr,
                GL_STATIC_DRAW);

          auto mesh_buffer = std::make_shared<yc::gl::mesh_buffer>(
                ctx, vao, vbos[pos.buffer_index], ebo);

          mesh->meshes.push_back(mesh_buffer);
        }
      }
    }
  }

  return true;
}
