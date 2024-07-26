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

#include <numeric>
#include <yagecore/keyboard.hpp>
#include <yagecore/gl/texture.hpp>
#pragma GCC diagnostic pop

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
yc::gl::node::attr_layout yc::gl::node::layout;

/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::gl::node::attr_layout_item::attr_layout_item(
      type _type, GLint _location, bool _required)
: type_(_type)
, location(_location)
, required(_required)
{
}

yc::gl::node::attr_layout_item::type
yc::gl::node::attr_layout_item::get_type() const
{
  return type_;
}

GLint yc::gl::node::attr_layout_item::get_location() const
{
  return location;
}

bool yc::gl::node::attr_layout_item::get_required() const
{
  return required;
}

size_t yc::gl::node::attr_layout_item::get_size() const
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

std::optional<yc::gl::node::attr_layout_item>
yc::gl::node::attr_layout::get(attr_layout_item::type type_) const
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

yc::gl::node::attr_layout::attr_layout(
      const std::vector<attr_layout_item> & layout_)
: items(layout_) {};

bool yc::gl::node::attr_layout::need_type(
      yc::gl::node::attr_layout_item::type type_) const
{
  return get(type_).has_value();
}

bool yc::gl::node::attr_layout::required(
      yc::gl::node::attr_layout_item::type type_) const
{
  if (auto item = get(type_))
  {
    return item->get_required();
  }
  return false;
}

GLint yc::gl::node::attr_layout::location(
      yc::gl::node::attr_layout_item::type type_) const
{
  if (auto item = get(type_))
  {
    return item->get_location();
  }
  return -1;
}

std::vector<yc::gl::node::attr_layout_item>::const_iterator
yc::gl::node::attr_layout::begin() const
{
  return items.begin();
}

std::vector<yc::gl::node::attr_layout_item>::const_iterator
yc::gl::node::attr_layout::end() const
{
  return items.end();
}

void yc::gl::node::set_layout(const std::vector<attr_layout_item> & layout_)
{
  layout = layout_;
}

yc::gl::node::node(
      std::string                                     name,
      std::shared_ptr<yc::app::asset_cacher>          cacher,
      const std::shared_ptr<yc::graphics::graphics> & graphics,
      type                                            type)
: yc::graphics::node(std::move(name), std::move(cacher), graphics, type)
, ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics))
{
}

yc::graphics::aabb yc::gl::node::get_aabb() const
{
  return aabb;
}

yc::error yc::gl::node::load()
{
  if (loaded)
  {
    return OK;
  }

  live = true;

  logging(LOG_DEBUG, "Loading GLTF node: '%s'", get_name().c_str());

  if (!load_gltf(sources.get_uri()))
  {
    logging(LOG_ERROR, "Failed to load GLTF node: '%s'", get_name().c_str());
    return yc::error::EXTERNAL_API_ERROR;
  }

  live   = true;
  loaded = true;

  return yc::error::OK;
}

void yc::gl::node::unload()
{
  if (!loaded)
  {
    return;
  }

  for (auto & texture : textures)
  {
    texture->unload();
  }

  for (auto & material : materials)
  {
    material->unload();
  }

  /* Unload all meshes */
  if (mesh)
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

bool yc::gl::node::load_gltf(const yc::data_source & source)
{
  fastgltf::Parser parser;

  std::vector<uint8_t> data;
  source.read(data);
  if (!source.empty())
  {
    logging(LOG_ERROR, "Failed to read GLTF data from URI: '%s'", source.string().c_str());
    return false;
  }

  auto data    = fastgltf::MappedGltfFile::FromPath(uri.get_uri());
  bool success = true;
  if (auto error = data.error(); error != fastgltf::Error::None)
  {
    logging(
          LOG_ERROR, "Failed to read GLTF data from URI: '%s'",
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
          LOG_ERROR, "Failed to parse GLTF data from URI: '%s'",
          uri.get_uri().c_str());
    success = false;
  }
  if (!success)
  {
    return false;
  }

  return parse_gltf(asset_error.get());
}

void gltf_datasource(
      const fastgltf::Asset &      asset,
      const fastgltf::DataSource & source,
      std::vector<uint8_t> &       out,
      fastgltf::MimeType &         out_type)
{
  std::visit(
        [&asset, &out, &out_type](auto && arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, fastgltf::sources::BufferView>)
          {
            auto & buffer_view = asset.bufferViews[arg.bufferViewIndex];
            auto & buffer      = asset.buffers[buffer_view.bufferIndex];

            std::vector<uint8_t> buffer_data;
            fastgltf::MimeType   buffer_mime;
            gltf_datasource(asset, buffer.data, buffer_data, buffer_mime);

            out.resize(buffer_view.byteLength);
            std::memcpy(
                  out.data(), buffer_data.data() + buffer_view.byteOffset,
                  buffer_view.byteLength);

            out_type = arg.mimeType;
          }
          else if constexpr (std::is_same_v<T, fastgltf::sources::URI>)
          {
            logging(LOG_DEBUG, "URI: %s", arg.uri.c_str());
          }
          else if constexpr (std::is_same_v<T, fastgltf::sources::Array>)
          {
            out.resize(arg.bytes.size());
            std::memcpy(out.data(), arg.bytes.data(), arg.bytes.size());
            out_type = arg.mimeType;
          }
          else if constexpr (std::is_same_v<T, fastgltf::sources::Vector>)
          {
            out.resize(arg.bytes.size());
            std::memcpy(out.data(), arg.bytes.data(), arg.bytes.size());
            out_type = arg.mimeType;
          }
          else if constexpr (std::is_same_v<T, fastgltf::sources::CustomBuffer>)
          {
            logging(LOG_DEBUG, "CustomBuffer");
          }
          else if constexpr (std::is_same_v<T, fastgltf::sources::ByteView>)
          {
            logging(LOG_DEBUG, "ByteView");
          }
          else if constexpr (std::is_same_v<T, fastgltf::sources::Fallback>)
          {
            logging(LOG_DEBUG, "Fallback");
            return;
          }
          else
          {
            logging(LOG_DEBUG, "Unknown");
            return;
          }
        },
        source);
}

bool yc::gl::node::parse_gltf(const fastgltf::Asset & asset)
{
  /* Load textures */
  for (auto & gltf_texture : asset.textures)
  {
    if (gltf_texture.imageIndex)
    {
      auto & gltf_image = asset.images[gltf_texture.imageIndex.value()];
      std::vector<uint8_t> image_data;
      fastgltf::MimeType   image_type;
      gltf_datasource(asset, gltf_image.data, image_data, image_type);

      std::string mime;
      if (image_type == fastgltf::MimeType::None)
      {
        logging(
              LOG_WARN, "Texture '%s' has no mime type",
              gltf_texture.name.c_str());
        mime = "image/png";
      }
      else
      {
        mime = fastgltf::getMimeTypeString(image_type);
      }

      std::string name;
      if (!gltf_texture.name.empty())
      {
        name = get_name() + "_" + std::string(gltf_texture.name);
      }
      else if (!gltf_image.name.empty())
      {
        name = get_name() + "_" + std::string(gltf_image.name);
      }
      else
      {
        name = get_name() + "_texture_" + std::to_string(textures.size());
      }

      yc::data_source uri(image_data, mime);
      auto texture = std::make_shared<yc::gl::texture>(name, cacher, graphics);
      yc::gl::texture::data data;
      data.add_uri(uri);
      yc::graphics::texture_param params;
      params.set_type(yc::graphics::tex_type::TEXTURE_2D);
      params.set_min_filter(yc::graphics::tex_filter::LINEAR);
      params.set_mag_filter(yc::graphics::tex_filter::LINEAR);
      params.set_wrap_s(yc::graphics::tex_wrap::REPEAT);
      params.set_wrap_t(yc::graphics::tex_wrap::REPEAT);
      params.set_format(yc::graphics::tex_pixel_format::RGB);
      data.set_parameters(params);
      texture->set_sources(data);
      textures.push_back(texture);
    }
    else
    {
      logging(
            LOG_WARN, "Texture '%s' has no image index",
            gltf_texture.name.c_str());
    }
  }

  for (auto & texture : textures)
  {
    texture->load();
  }

  for (auto & material : asset.materials)
  {
    auto mat = std::make_shared<yc::graphics::material>(
          material.name.c_str(), cacher, graphics);
    materials.push_back(mat);

    mat->pbr.base_color_factor = glm::vec3(
          material.pbrData.baseColorFactor[0],
          material.pbrData.baseColorFactor[1],
          material.pbrData.baseColorFactor[2]);
    if (material.pbrData.baseColorTexture)
    {
      mat->pbr.base_color_texture
            = textures[material.pbrData.baseColorTexture->textureIndex];
    }

    mat->pbr.metallic_factor  = material.pbrData.metallicFactor;
    mat->pbr.roughness_factor = material.pbrData.roughnessFactor;
    if (material.pbrData.metallicRoughnessTexture)
    {
      mat->pbr.metallic_roughness_texture
            = textures[material.pbrData.metallicRoughnessTexture->textureIndex];
    }

    if (material.normalTexture)
    {
      mat->normal_scale   = material.normalTexture->scale;
      mat->normal_texture = textures[material.normalTexture->textureIndex];
    }

    if (material.occlusionTexture)
    {
      mat->occlusion_strength = material.occlusionTexture->strength;
      mat->occlusion_texture
            = textures[material.occlusionTexture->textureIndex];
    }

    if (material.emissiveTexture)
    {
      mat->emissive_factor = glm::vec3(
            material.emissiveFactor[0], material.emissiveFactor[1],
            material.emissiveFactor[2]);
      mat->emissive_texture = textures[material.emissiveTexture->textureIndex];
    }
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

      std::vector<uint32_t> indices;
      std::vector<vertex>   vertices;

      /* Load mesh */
      aabb = {};
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
                  LOG_ERROR, "No indices found in GLTF node: '%s'",
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

          size_t initial_vtx = 0;
          if (!process_position(
                    asset, gltf_primitive, vertices, initial_vtx, aabb))
            return false;
          if (!process_normal(asset, gltf_primitive, vertices, initial_vtx))
            return false;
          if (!process_tangent(asset, gltf_primitive, vertices, initial_vtx))
            return false;
          if (!process_texcoord(asset, gltf_primitive, vertices, initial_vtx))
            return false;
          if (!process_color(asset, gltf_primitive, vertices, initial_vtx))
            return false;
          if (!process_joints(asset, gltf_primitive, vertices, initial_vtx))
            return false;
          if (!process_weights(asset, gltf_primitive, vertices, initial_vtx))
            return false;
        }
      }

      /* Curently, mesh always have the same bounding box with the node */
      node->mesh = make_buffer(vertices, indices, aabb);
      if (!node->mesh)
      {
        logging(
              LOG_ERROR, "Failed to create mesh buffer for GLTF node: '%s'",
              get_name().c_str());
        return false;
      }
    }
  }

  return true;
}

bool yc::gl::node::process_position(
      const fastgltf::Asset &     asset,
      const fastgltf::Primitive & primitive,
      std::vector<vertex> &       verticies,
      size_t &                    initial_vtx,
      yc::graphics::aabb &        aabb)
{
  if (!layout.need_type(attr_layout_item::type::POSITION))
  {
    return true;
  }

  if (primitive.findAttribute("POSITION") == primitive.attributes.end())
  {
    if (layout.required(attr_layout_item::type::POSITION))
    {
      logging(
            LOG_ERROR, "No positions found in GLTF node: '%s'",
            get_name().c_str());
      return false;
    }

    logging(
          LOG_WARN,
          "No positions found in GLTF node: '%s'. All vectors will "
          "be zeroed",
          get_name().c_str());
    return true;
  }

  auto & gltf_pos_accessor
        = asset.accessors[primitive.findAttribute("POSITION")->accessorIndex];

  initial_vtx = verticies.size();
  verticies.resize(verticies.size() + gltf_pos_accessor.count);
  fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
        asset, gltf_pos_accessor,
        [&](fastgltf::math::fvec3 pos, std::size_t idx) {
          vertex v                     = {};
          v.position                   = glm::vec3(pos[0], pos[1], pos[2]);
          verticies[initial_vtx + idx] = v;

          aabb.extend(v.position);
        });

  return true;
}

bool yc::gl::node::process_normal(
      const fastgltf::Asset &     asset,
      const fastgltf::Primitive & primitive,
      std::vector<vertex> &       verticies,
      size_t &                    initial_vtx)
{
  if (!layout.need_type(attr_layout_item::type::NORMAL))
  {
    return true;
  }

  if (primitive.findAttribute("NORMAL") == primitive.attributes.end())
  {
    if (layout.required(attr_layout_item::type::NORMAL))
    {
      logging(
            LOG_ERROR, "No normals found in GLTF node: '%s'",
            get_name().c_str());
      return false;
    }

    logging(
          LOG_WARN,
          "No normals found in GLTF node: '%s'. All vectors will "
          "be zeroed",
          get_name().c_str());
    return true;
  }

  auto & gltf_normal_accessor
        = asset.accessors[primitive.findAttribute("NORMAL")->accessorIndex];
  fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
        asset, gltf_normal_accessor,
        [&](fastgltf::math::fvec3 normal, std::size_t idx) {
          verticies[initial_vtx + idx].normal
                = glm::vec3(normal[0], normal[1], normal[2]);
        });

  return true;
}

bool yc::gl::node::process_tangent(
      const fastgltf::Asset &     asset,
      const fastgltf::Primitive & primitive,
      std::vector<vertex> &       verticies,
      size_t &                    initial_vtx)
{
  if (layout.need_type(attr_layout_item::type::TANGENT))
  {
    logging(LOG_ERROR, "Tangent is not currently supported");
    return false;
  }

  return true;
}

bool yc::gl::node::process_texcoord(
      const fastgltf::Asset &     asset,
      const fastgltf::Primitive & primitive,
      std::vector<vertex> &       verticies,
      size_t &                    initial_vtx)
{

  if (!layout.need_type(attr_layout_item::type::TEXCOORD))
  {
    return true;
  }

  if (primitive.findAttribute("TEXCOORD_0") == primitive.attributes.end())
  {
    if (layout.required(attr_layout_item::type::TEXCOORD))
    {
      logging(
            LOG_ERROR, "No texture coordinates found in GLTF node: %s",
            get_name().c_str());
      return false;
    }

    logging(
          LOG_WARN,
          "No texture coordinates found in GLTF node: '%s'. All vectors will "
          "be zeroed",
          get_name().c_str());
    return true;
  }

  auto & gltf_uv_accessor
        = asset.accessors[primitive.findAttribute("TEXCOORD_0")->accessorIndex];
  fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
        asset, gltf_uv_accessor,
        [&](fastgltf::math::fvec2 uv, std::size_t idx) {
          verticies[initial_vtx + idx].uvs = glm::vec2(uv[0], uv[1]);
        });

  return true;
}

bool yc::gl::node::process_color(
      const fastgltf::Asset &     asset,
      const fastgltf::Primitive & primitive,
      std::vector<vertex> &       verticies,
      size_t &                    initial_vtx)
{

  if (!layout.need_type(attr_layout_item::type::COLOR))
  {
    return true;
  }

  /* TODO: Try to find a better way to handle color */
  if (primitive.findAttribute("COLOR_0") == primitive.attributes.end())
  {
    if (layout.required(attr_layout_item::type::COLOR))
    {
      logging(
            LOG_ERROR, "No colors found in GLTF node: '%s'",
            get_name().c_str());
      return false;
    }

    logging(
          LOG_WARN,
          "No colors found in GLTF node: '%s'. All vectors will "
          "be zeroed",
          get_name().c_str());
    return true;
  }

  auto & gltf_color_accessor
        = asset.accessors[primitive.findAttribute("COLOR_0")->accessorIndex];

  fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
        asset, gltf_color_accessor,
        [&](fastgltf::math::fvec3 color, std::size_t idx) {
          verticies[initial_vtx + idx].color
                = glm::vec4(color[0], color[1], color[2], 1.0f);
        });

  return true;
}

bool yc::gl::node::process_joints(
      const fastgltf::Asset &     asset,
      const fastgltf::Primitive & primitive,
      std::vector<vertex> &       verticies,
      size_t &                    initial_vtx)
{
  if (layout.need_type(attr_layout_item::type::JOINTS))
  {
    logging(LOG_ERROR, "Joints is not currently supported");
    return false;
  }

  return true;
}

bool yc::gl::node::process_weights(
      const fastgltf::Asset &     asset,
      const fastgltf::Primitive & primitive,
      std::vector<vertex> &       verticies,
      size_t &                    initial_vtx)
{
  if (layout.need_type(attr_layout_item::type::WEIGHTS))
  {
    logging(LOG_ERROR, "Weights is not currently supported");
    return false;
  }

  return true;
}

std::shared_ptr<yc::gl::mesh_buffer> yc::gl::node::make_buffer(
      const std::vector<vertex> & verticies,
      const std::vector<GLuint> & indices,
      const yc::graphics::aabb &  aabb)
{
  size_t stride = std::accumulate(
        layout.begin(), layout.end(), 0,
        [](size_t acc, const attr_layout_item & elem) {
          return acc + elem.get_size();
        });

  class attr_descr
  {
  public:
    size_t offset; /* In bytes */
    size_t stride; /* In bytes */
    size_t size;   /* Components */
    GLenum type;   /* Type */
    GLint  location;

    attr_descr(
          size_t _offset,
          size_t _stride,
          size_t _size,
          GLenum _type,
          GLint  _loc)
    : offset(_offset)
    , stride(_stride)
    , size(_size)
    , type(_type)
    , location(_loc) {};
  };
  std::vector<attr_descr> attrs;

  std::vector<uint8_t> data;
  data.resize(verticies.size() * stride);

  size_t offset = 0; /* In bytes */
  for (auto & item : layout)
  {
    switch (item.get_type())
    {
      case attr_layout_item::type::POSITION:
      {
        static_assert(sizeof(verticies[0].position) == 3 * sizeof(float));
        static_assert(sizeof(float) == sizeof(GLfloat));

        attrs.emplace_back(
              offset, stride, 3, GL_FLOAT,
              layout.location(attr_layout_item::type::POSITION));
        for (size_t i = 0; i < verticies.size(); i++)
        {
          auto & vertex = verticies[i];
          std::memcpy(
                data.data() + i * stride + offset, &vertex.position,
                sizeof(vertex.position));
        }

        offset += sizeof(glm::vec3);
        break;
      }

      case attr_layout_item::type::NORMAL:
      {
        attrs.emplace_back(
              offset, stride, 3, GL_FLOAT,
              layout.location(attr_layout_item::type::NORMAL));
        static_assert(sizeof(verticies[0].normal) == 3 * sizeof(float));
        static_assert(sizeof(float) == sizeof(GLfloat));

        for (size_t i = 0; i < verticies.size(); i++)
        {
          auto & vertex = verticies[i];
          std::memcpy(
                data.data() + i * stride + offset, &vertex.normal,
                sizeof(vertex.normal));
        }

        offset += sizeof(verticies[0].normal);
        break;
      }

      case attr_layout_item::type::TEXCOORD:
      {
        static_assert(sizeof(verticies[0].uvs) == 2 * sizeof(float));
        static_assert(sizeof(float) == sizeof(GLfloat));

        attrs.emplace_back(
              offset, stride, 2, GL_FLOAT,
              layout.location(attr_layout_item::type::TEXCOORD));

        for (size_t i = 0; i < verticies.size(); i++)
        {
          auto & vertex = verticies[i];
          std::memcpy(
                data.data() + i * stride + offset, &vertex.uvs,
                sizeof(vertex.uvs));
        }

        offset += sizeof(verticies[0].uvs);
        break;
      }

      default:
      {
        logging(
              LOG_ERROR, "Unsupported attribute type: %d",
              static_cast<int>(item.get_type()));
        return nullptr;
      }
    }
  }

  /* Create Vertex Array Object */
  GLuint vao, vbo, ebo;
  yc::gl::glGenVertexArrays(1, &vao);
  yc::gl::glGenBuffers(1, &vbo);
  yc::gl::glGenBuffers(1, &ebo);

  /* Upload data to GPU */
  yc::gl::glBindVertexArray(vao);
  yc::gl::glBindBuffer(GL_ARRAY_BUFFER, vbo);
  yc::gl::glBufferData(
        GL_ARRAY_BUFFER, data.size(), data.data(), GL_STATIC_DRAW);

  /* Upload indices to GPU */
  yc::gl::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  yc::gl::glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
        indices.data(), GL_STATIC_DRAW);

  for (auto & attr : attrs)
  {
    yc::gl::glEnableVertexAttribArray(attr.location);
    yc::gl::glVertexAttribPointer(
          attr.location, attr.size, attr.type, GL_FALSE, stride,
          reinterpret_cast<void *>(attr.offset));
  }

  yc::gl::glBindVertexArray(0);

  logging(
        LOG_DEBUG, "Loaded %" PRIuPTR " vertices/%" PRIuPTR " indices",
        verticies.size(), indices.size());

  /* Stub for now */
  std::shared_ptr<yc::graphics::material> material
        = yc::graphics::material::basic_material();

  yc::gl::mesh_buffer::indicies_info indicies = {
    .ebo    = ebo,
    .type   = GL_UNSIGNED_INT,
    .offset = reinterpret_cast<void *>(0 * sizeof(GLuint)),
  };
  yc::gl::mesh_buffer::vertices_info vertices = {
    .vbo   = vbo,
    .vao   = vao,
    .type  = GL_TRIANGLES,
    .count = static_cast<GLsizei>(indices.size()),
    .aabb  = aabb,
  };

  return std::make_shared<yc::gl::mesh_buffer>(
        ctx, vertices, indicies, material);
}
