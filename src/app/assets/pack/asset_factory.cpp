/** ========================================================================
 *
 * @file asset_factory.cpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <fstream>
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
  std::vector<std::string> files
        = yage::core::list_files(extract_path, "*.meta");
  for (const auto & file : files)
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
      return yage::core::INVALID_STATE;
    }

    auto t = root["type"];
    if (t.isNull())
    {
      log(LOG_ERROR, "Type is not specified in meta file: %s", file.c_str());
      return yage::core::INVALID_STATE;
    }

    std::string basename = yage::core::basename(file, false);

    if (t.asString() == "shader")
    {
      if (!parse_shader(graphics, basename, root))
      {
        log(LOG_ERROR, "Failed to parse shader: %s", basename.c_str());
        return yage::core::INVALID_STATE;
      }
    }
    else
    {
      log(LOG_ERROR, "Unknown type in meta file: %s", file.c_str());
      return yage::core::INVALID_STATE;
    }
  }

  return yage::core::error::OK;
}

yage::core::error yage::app::assets::pack::asset_factory::get_shader(
      const std::string & name, yage::app::assets::shader ** shader)
{
  if (shaders.find(name) == shaders.end())
  {
    return yage::core::FILE_NOT_FOUND;
  }

  *shader = shaders[name];
  return yage::core::error::OK;
}

yage::app::assets::shader *
yage::app::assets::pack::asset_factory::parse_shader(
      yage::graphics::graphics * graphics,
      const std::string &        basename,
      const Json::Value &        root)
{
  std::vector<std::pair<yage::graphics::shader_type, std::string>> sources;
  Json::Value source = root["fragment"];
  if (!source.isNull())
  {
    sources.emplace_back(
          yage::graphics::shader_type::FRAGMENT_SHADER,
          extract_path + "/" + source.asString());
  }

  source = root["vertex"];
  if (!source.isNull())
  {
    sources.emplace_back(
          yage::graphics::shader_type::VERTEX_SHADER,
          extract_path + "/" + source.asString());
  }

  if (sources.empty())
  {
    log(LOG_ERROR, "No sources found for shader: %s", basename.c_str());
    return nullptr;
  }

  auto * ptr
        = new yage::app::assets::shader(basename, &cacher, graphics, sources);
  shaders[basename] = ptr;

  return ptr;
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
