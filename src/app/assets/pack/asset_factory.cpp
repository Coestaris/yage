/** ========================================================================
 *
 * @file asset_factory.cpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <fstream>
#include <utility>
#include "elzip/elzip.hpp"

#include "app/assets/pack/asset_factory.hpp"
#include "app/assets/asset_factory.hpp"
#include "app/assets/shader.hpp"
#include "graphics/shader_type.hpp"
#include "core/fs.hpp"
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error yage::app::assets::pack::asset_factory::pack_parser::parse(
      const std::string &      dir,
      std::vector<std::string> meta_files,
      bool                     fail_on_error)
{
  for (const auto & file : meta_files)
  {
    log(LOG_INFO, "Reading meta file: %s", file.c_str());
    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
      log(LOG_ERROR, "Failed to open meta file: %s", file.c_str());
      return yage::core::FILE_NOT_FOUND;
    }

    Json::Value  root;
    Json::Reader reader;
    if (!reader.parse(ifs, root))
    {
      log(LOG_ERROR, "Failed to parse meta file: %s", file.c_str());
      log(LOG_ERROR, "%s", reader.getFormattedErrorMessages().c_str());
      if (fail_on_error)
      {
        return yage::core::INVALID_STATE;
      }
      continue;
    }

    auto t = root["type"];
    if (t.isNull())
    {
      log(LOG_ERROR, "Type is not specified in meta file: %s", file.c_str());
      if (fail_on_error)
      {
        return yage::core::INVALID_STATE;
      }
      continue;
    }

    std::string basename = yage::core::basename(file, false);

    if (t.asString() == "shader")
    {
      auto sources = parse_shader_sources(dir, root);
      if (sources.empty())
      {
        log(LOG_ERROR, "Failed to parse shader sources from meta file: %s",
            file.c_str());
        if (fail_on_error)
        {
          return yage::core::INVALID_STATE;
        }
        continue;
      }

      if (on_shader_cb)
        on_shader_cb(basename, sources);
    }
    else if (t.asString() == "texture")
    {
      auto [param, sources] = parse_texture_sources(dir, root);
      if (sources.empty())
      {
        log(LOG_ERROR, "Failed to parse texture sources from meta file: %s",
            file.c_str());
        if (fail_on_error)
        {
          return yage::core::INVALID_STATE;
        }
        continue;
      }

      if (on_texture_cb)
        on_texture_cb(basename, param, sources);
    }
    else
    {
      log(LOG_ERROR, "Unknown type '%s' in meta file: %s", t.asString().c_str(),
          file.c_str());
      if (fail_on_error)
      {
        return yage::core::INVALID_STATE;
      }
      continue;
    }
  }

  return yage::core::error::OK;
}
std::pair<yage::graphics::texture_param, std::vector<std::string>>
yage::app::assets::pack::asset_factory::pack_parser::parse_texture_sources(
      const std::string & dir, const Json::Value & root)
{
  std::vector<std::string>      sources;
  yage::graphics::texture_param param;

  /*
   * {"type": "texture", "tex_type": "TEXTURE_2D", "format": "RGBA",
   * "border_color": [0, 0, 0, 0], "min_filter": "LINEAR", "mag_filter":
   * "LINEAR", "wrap_s": "CLAMP_TO_EDGE", "wrap_t": "CLAMP_TO_EDGE", "wrap_r":
   * "CLAMP_TO_EDGE", "textures": ["image.png"]}
   */

  param.type
        = yage::graphics::tex_type_from_string(root["tex_type"].asString());
  param.format = yage::graphics::tex_pixel_format_from_string(
        root["format"].asString());
  param.min_filter
        = yage::graphics::tex_filter_from_string(root["min_filter"].asString());
  param.mag_filter
        = yage::graphics::tex_filter_from_string(root["mag_filter"].asString());
  param.wrap_s
        = yage::graphics::tex_wrap_from_string(root["wrap_s"].asString());
  param.wrap_t
        = yage::graphics::tex_wrap_from_string(root["wrap_t"].asString());
  param.wrap_r
        = yage::graphics::tex_wrap_from_string(root["wrap_r"].asString());
  param.border_color = yage::core::float4(
        root["border_color"][0].asFloat(), root["border_color"][1].asFloat(),
        root["border_color"][2].asFloat(), root["border_color"][3].asFloat());

  for (const auto & texture : root["textures"])
  {
    sources.push_back(dir + "/" + texture.asString());
  }

  return { param, sources };
}

yage::app::assets::shader::sources
yage::app::assets::pack::asset_factory::pack_parser::parse_shader_sources(
      const std::string & dir, const Json::Value & root)
{
  yage::app::assets::shader::sources sources;

  const std::map<std::string, yage::graphics::shader_type> types
        = { { "fragment", yage::graphics::shader_type::FRAGMENT_SHADER },
            { "vertex", yage::graphics::shader_type::VERTEX_SHADER } };

  for (const auto & [key, type] : types)
  {
    Json::Value source = root[key];
    if (!source.isNull())
    {
      std::string   s = dir + "/" + source.asString();
      std::ifstream sourcef(s);
      if (!sourcef.is_open())
      {
        log(LOG_ERROR, "Failed to open shader source file: %s", s.c_str());
        continue;
      }

      std::string source_code(
            (std::istreambuf_iterator<char>(sourcef)),
            std::istreambuf_iterator<char>());

      sources.emplace_back(type, source_code);
    }
  }

  return sources;
}

yage::core::error yage::app::assets::pack::asset_factory::read_assets(
      yage::graphics::graphics * graphics)
{
  /* 1. Unzip the archive */
  extract_path = yage::core::get_temp_dir();
  if (extract_path.empty())
  {
    log(LOG_ERROR, "Failed to get temporary directory");
    return yage::core::INVALID_STATE;
  }
  extract_path += "/assets";

  log(LOG_INFO, "Extracting assets to: %s", extract_path.c_str());
  elz::extractZip(path, extract_path);

  /* 2. Read .meta files */
  std::vector<std::string> meta_files
        = yage::core::list_files(extract_path, "*.meta");

  pack_parser parser;
  parser.on_shader([this, graphics](
                         const std::string &                basename,
                         yage::app::assets::shader::sources sources) {
    auto * shader = new yage::app::assets::shader(basename, &cacher, graphics);
    shader->update(std::move(sources));
    shaders[basename] = shader;
  });
  parser.on_texture([this, graphics](
                          const std::string &                   basename,
                          const yage::graphics::texture_param & param,
                          const std::vector<std::string> &      sources) {
    auto * texture
          = new yage::app::assets::texture(basename, &cacher, graphics);
    texture->update(param, sources);
    textures[basename] = texture;
  });

  if (parser.parse(extract_path, meta_files) != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to parse pack file");
    return yage::core::INVALID_STATE;
  }

  watcher.start();

  return yage::core::error::OK;
}

yage::app::assets::shader *
yage::app::assets::pack::asset_factory::get_shader(const std::string & name)
{
  return shaders[name];
}

yage::app::assets::texture *
yage::app::assets::pack::asset_factory::get_texture(const std::string & name)
{
  return textures[name];
}

yage::core::error yage::app::assets::pack::asset_factory::destroy()
{
  watcher.stop();
  watcher.join();

  for (auto & shader : shaders)
  {
    delete shader.second;
  }
  shaders.clear();

  return yage::core::OK;
}

yage::core::error
yage::app::assets::pack::asset_factory::asset_cacher::cache_asset(
      const std::string & key, const std::vector<uint8_t> & data)
{
  /* save keys as files */
  std::string   dst = cache_path + "/" + key;
  std::ofstream ofs(dst, std::ios::binary | std::ios::trunc | std::ios::out);

  if (!ofs.is_open())
  {
    log(LOG_ERROR, "Failed to open file for writing: %s", dst.c_str());
    return yage::core::FILE_NOT_FOUND;
  }

  ofs.write(reinterpret_cast<const char *>(data.data()), data.size());
  ofs.close();

  return yage::core::error::OK;
}

yage::core::error
yage::app::assets::pack::asset_factory::asset_cacher::get_cached_asset(
      const std::string & key, std::vector<uint8_t> & data)
{
  std::string   src = cache_path + "/" + key;
  std::ifstream ifs(src, std::ios::binary);

  if (!ifs.is_open())
  {
    return yage::core::FILE_NOT_FOUND;
  }

  ifs.seekg(0, std::ios::end);
  size_t size = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  data.resize(size);
  ifs.read(reinterpret_cast<char *>(data.data()), size);

  return yage::core::error::OK;
}

void yage::app::assets::pack::asset_factory::pack_watcher::run()
{
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (should_stop)
    {
      break;
    }

    time_t mtime = yage::core::file_mtime(factory->path);

    if (last_mtime == 0)
    {
      last_mtime = mtime;
      continue;
    }

    if (last_mtime != mtime)
    {
      log(LOG_INFO, "Pack file has been modified, reloading assets");
      last_mtime = mtime;

      std::string p = factory->extract_path + "_new";
      log(LOG_DEBUG, "Extracting assets to: %s", p.c_str());
      elz::extractZip(factory->path, p);

      std::vector<std::string> meta_files = yage::core::list_files(p, "*.meta");

      pack_parser parser;
      parser.on_shader([this,
                        p](const std::string &                basename,
                           yage::app::assets::shader::sources sources) {
        if (factory->shaders.find(basename) == factory->shaders.end())
        {
          log(LOG_WARN, "New shader: %s. Please rerun the application",
              basename.c_str());
        }

        auto * shader = factory->shaders[basename];
        if (shader->src != sources)
        {
          log(LOG_DEBUG, "Marked shader for update: %s", basename.c_str());
          shader->update(sources);
        }
        else
        {
          log(LOG_DEBUG, "Shader is up to date: %s", basename.c_str());
        }
      });

      if (parser.parse(p, meta_files) != yage::core::error::OK)
      {
        log(LOG_ERROR, "Failed to parse pack file");
      }
    }
  }
}
