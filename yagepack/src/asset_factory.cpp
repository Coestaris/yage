/** ========================================================================
 *
 * @file asset_factory.cpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagepack/asset_factory.hpp"

#include <fstream>
#include <utility>
#include "elzip/elzip.hpp"

#include "yagecore/fs.hpp"
#include "yagecore/log.hpp"
#include "yagecore/error.hpp"
#include "yagecore/app/asset_factory.hpp"
#include "yagecore/graphics/shader.hpp"
#include "yagecore/graphics/texture.hpp"

#include "glm/vec4.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::error yagepack::asset_factory::pack_parser::parse(
      const std::string &      dir,
      std::vector<std::string> meta_files,
      bool                     fail_on_error)
{
  for (const auto & file : meta_files)
  {
    //    logging(LOG_INFO, "Reading meta file: %s", file.c_str());
    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
      logging(LOG_ERROR, "Failed to open meta file: %s", file.c_str());
      return yc::FILE_NOT_FOUND;
    }

    Json::Value  root;
    Json::Reader reader;
    if (!reader.parse(ifs, root))
    {
      logging(LOG_ERROR, "Failed to parse meta file: %s", file.c_str());
      logging(LOG_ERROR, "%s", reader.getFormattedErrorMessages().c_str());
      if (fail_on_error)
      {
        return yc::INVALID_STATE;
      }
      continue;
    }

    auto t = root["type"];
    if (t.isNull())
    {
      logging(
            LOG_ERROR, "Type is not specified in meta file: %s", file.c_str());
      if (fail_on_error)
      {
        return yc::INVALID_STATE;
      }
      continue;
    }

    std::string basename = yc::basename(file, false);

    if (t.asString() == "shader")
    {
      auto sources = parse_shader_sources(dir, root);
      if (sources.empty())
      {
        logging(
              LOG_ERROR, "Failed to parse shader sources from meta file: %s",
              file.c_str());
        if (fail_on_error)
        {
          return yc::INVALID_STATE;
        }
        continue;
      }

      if (on_shader_cb)
        on_shader_cb(basename, sources);
    }
    else if (t.asString() == "texture")
    {
      auto sources = parse_texture_sources(dir, root);
      if (sources.empty())
      {
        logging(
              LOG_ERROR, "Failed to parse texture sources from meta file: %s",
              file.c_str());
        if (fail_on_error)
        {
          return yc::INVALID_STATE;
        }
        continue;
      }

      if (on_texture_cb)
        on_texture_cb(basename, sources);
    }
    else if (t.asString() == "model")
    {
      auto sources = parse_gltf_sources(dir, root);
      if (sources.empty())
      {
        logging(
              LOG_ERROR, "Failed to parse glTF sources from meta file: %s",
              file.c_str());
        if (fail_on_error)
        {
          return yc::INVALID_STATE;
        }
        continue;
      }

      if (on_gltf_cb)
        on_gltf_cb(basename, sources);
    }
    else
    {
      logging(
            LOG_ERROR, "Unknown type '%s' in meta file: %s",
            t.asString().c_str(), file.c_str());
      if (fail_on_error)
      {
        return yc::INVALID_STATE;
      }
      continue;
    }
  }

  return yc::error::OK;
}

yc::util::uri resolve_uri(const std::string & dir, const std::string & uri)
{
  if (uri.empty())
  {
    return "";
  }

  if (uri[0] == '/')
  {
    return uri;
  }

  return yc::join_path(dir, uri);
}

yc::graphics::texture::data
yagepack::asset_factory::pack_parser::parse_texture_sources(
      const std::string & dir, const Json::Value & root)
{
  yc::graphics::texture::data sources;
  yc::graphics::texture_param param {};

  param.type = yc::graphics::tex_type_from_string(root["tex_type"].asString());
  param.format
        = yc::graphics::tex_pixel_format_from_string(root["format"].asString());
  param.min_filter
        = yc::graphics::tex_filter_from_string(root["min_filter"].asString());
  param.mag_filter
        = yc::graphics::tex_filter_from_string(root["mag_filter"].asString());
  param.wrap_s = yc::graphics::tex_wrap_from_string(root["wrap_s"].asString());
  param.wrap_t = yc::graphics::tex_wrap_from_string(root["wrap_t"].asString());
  param.wrap_r = yc::graphics::tex_wrap_from_string(root["wrap_r"].asString());
  param.border_color = glm::vec4(
        root["border_color"][0].asFloat(), root["border_color"][1].asFloat(),
        root["border_color"][2].asFloat(), root["border_color"][3].asFloat());
  sources.set_parameters(param);

  for (const auto & uri : root["uris"])
  {
    sources.add_uri(resolve_uri(dir, uri.asString()));
  }

  return sources;
}

yc::graphics::shader::data
yagepack::asset_factory::pack_parser::parse_shader_sources(
      const std::string & dir, const Json::Value & root)
{
  auto lang = root["language"];
  if (lang.isNull())
  {
    logging(LOG_ERROR, "Language is not specified in meta file");
    return { {} };
  }

  yc::graphics::shader::data sources(lang.asString());

  auto shaders = root["shaders"];
  if (shaders.isNull())
  {
    logging(LOG_ERROR, "Shaders are not specified in meta file");
    return { {} };
  }

  for (const auto & shader : shaders)
  {
    auto type = shader["type"];
    if (type.isNull())
    {
      logging(LOG_ERROR, "Type is not specified in shader");
      continue;
    }

    auto uri = shader["uri"];
    if (uri.isNull())
    {
      logging(LOG_ERROR, "Source is not specified in shader");
      continue;
    }

    sources.add_source(type.asString(), resolve_uri(dir, uri.asString()));
  }

  return sources;
}

yc::graphics::node::data
yagepack::asset_factory::pack_parser::parse_gltf_sources(
      const std::string & dir, const Json::Value & root)
{
  auto model_type = root["model_type"];
  if (model_type.isNull())
  {
    logging(LOG_ERROR, "Model type is not specified in meta file");
    return {};
  }

  if (model_type.asString() != "GLTF")
  {
    logging(
          LOG_ERROR, "Unsupported model type: %s",
          model_type.asString().c_str());
    return {};
  }

  yc::graphics::node::data sources;
  auto                     uri = root["uri"];
  if (uri.isNull())
  {
    logging(LOG_ERROR, "URI is not specified in meta file");
    return {};
  }

  sources.set_uri(resolve_uri(dir, uri.asString()));
  return sources;
}

yc::error
yagepack::asset_factory::read_assets(yc::graphics::graphics * graphics)
{
  /* 1. Unzip the archive */
  extract_path = yc::get_temp_dir();
  if (extract_path.empty())
  {
    logging(LOG_ERROR, "Failed to get temporary directory");
    return yc::INVALID_STATE;
  }
  extract_path = yc::join_path(extract_path, "assets");

  logging(LOG_INFO, "Extracting assets to: %s", extract_path.c_str());
  elz::extractZip(path, extract_path);

  /* 2. Read .meta files */
  std::vector<std::string> meta_files = yc::list_files(extract_path, "*.meta");

  pack_parser parser;
  parser.on_shader([this, graphics](
                         const std::string &                basename,
                         const yc::graphics::shader::data & sources) {
    if (!graphics->supports_shader_language(sources.get_language()))
    {
      logging(
            LOG_DEBUG, "Skipping shader: %s, unsupported language: %s",
            basename.c_str(), sources.get_language().c_str());
      return;
    }

    auto * shader = graphics->create_shader(basename, &cacher, graphics);
    shader->set_sources(const_cast<yc::graphics::shader::data &>(sources));
    assets[basename] = shader;
  });
  parser.on_texture([this, graphics](
                          const std::string &                 basename,
                          const yc::graphics::texture::data & sources) {
    auto * texture = graphics->create_texture(basename, &cacher, graphics);
    texture->set_sources(sources);
    assets[basename] = texture;
  });
  parser.on_gltf([this, graphics](
                       const std::string &              basename,
                       const yc::graphics::node::data & sources) {
    auto * node = graphics->create_node(basename, &cacher, graphics);
    node->set_sources(sources);
    assets[basename] = node;
  });

  if (parser.parse(extract_path, meta_files) != yc::error::OK)
  {
    logging(LOG_ERROR, "Failed to parse pack file");
    return yc::INVALID_STATE;
  }

#if USE_DEBUG
  watchdog.start();
#endif

  return yc::error::OK;
}

yc::app::asset * yagepack::asset_factory::get_asset(const std::string & name)
{
  return assets[name];
}

yc::error yagepack::asset_factory::destroy()
{
#if USE_DEBUG
  watchdog.stop();
  watchdog.join();
#endif

  for (auto & asset : assets)
  {
    asset.second->unload();
    delete asset.second;
  }
  assets.clear();

  return yc::OK;
}

yc::error yagepack::asset_factory::asset_cacher::cache_asset(
      const std::string & key, const std::vector<uint8_t> & data)
{
  /* save keys as files */
  std::string   dst = yc::join_path(cache_path, key);
  std::ofstream ofs(dst, std::ios::binary | std::ios::trunc | std::ios::out);

  if (!ofs.is_open())
  {
    logging(LOG_ERROR, "Failed to open file for writing: %s", dst.c_str());
    return yc::FILE_NOT_FOUND;
  }

  ofs.write(reinterpret_cast<const char *>(data.data()), data.size());
  ofs.close();

  return yc::error::OK;
}

yc::error yagepack::asset_factory::asset_cacher::get_cached_asset(
      const std::string & key, std::vector<uint8_t> & data)
{
  std::string   src = yc::join_path(cache_path, key);
  std::ifstream ifs(src, std::ios::binary);

  if (!ifs.is_open())
  {
    return yc::FILE_NOT_FOUND;
  }

  ifs.seekg(0, std::ios::end);
  size_t size = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  data.resize(size);
  ifs.read(reinterpret_cast<char *>(data.data()), size);

  return yc::error::OK;
}

#if USE_DEBUG
void yagepack::asset_factory::pack_watchdog::run()
{
  while (true)
  {
    if (should_stop)
    {
      break;
    }

    /* Wait for changes around 1000ms, then exit */
    if (!file_watcher.wait_for_changes(1000))
      continue;

    std::string p = factory->extract_path + "_new";
    logging(LOG_DEBUG, "Extracting assets to: %s", p.c_str());
    elz::extractZip(factory->path, p);

    std::vector<std::string> meta_files = yc::list_files(p, "*.meta");

    pack_parser parser;
    parser.on_shader([this,
                      p](const std::string &                basename,
                         const yc::graphics::shader::data & sources) {
      /* TODO: Skip unsupported languages */
      //      if
      //      (!factory->graphics->supports_shader_language(sources.get_language()))
      //      {
      //        logging(
      //              LOG_DEBUG, "Skipping shader: %s, unsupported language:
      //              %s", basename.c_str(), sources.get_language().c_str());
      //        return;
      //      }

      if (factory->assets.find(basename) == factory->assets.end())
      {
        logging(
              LOG_WARN, "New shader: %s. Please rerun the application",
              basename.c_str());
      }

      auto * shader
            = dynamic_cast<yc::graphics::shader *>(factory->assets[basename]);
      if (!shader)
      {
        logging(
              LOG_ERROR, "Failed to cast asset to shader: %s",
              basename.c_str());
        return;
      }

      shader->set_sources(const_cast<yc::graphics::shader::data &>(sources));
    });
    parser.on_texture([this,
                       p](const std::string &                 basename,
                          const yc::graphics::texture::data & sources) {
      if (factory->assets.find(basename) == factory->assets.end())
      {
        logging(
              LOG_WARN, "New texture: %s. Please rerun the application",
              basename.c_str());
      }

      auto * texture
            = dynamic_cast<yc::graphics::texture *>(factory->assets[basename]);
      if (!texture)
      {
        logging(
              LOG_ERROR, "Failed to cast asset to texture: %s",
              basename.c_str());
        return;
      }

      texture->set_sources(sources);
    });

    if (parser.parse(p, meta_files, false) != yc::error::OK)
    {
      logging(LOG_ERROR, "Failed to parse pack file");
    }
  }
}
#endif