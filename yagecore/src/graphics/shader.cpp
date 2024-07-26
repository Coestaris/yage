/** ========================================================================
 *
 * @file shader.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/shader.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::graphics::shader::data::data(std::string language)
: language(std::move(language))
{
}

std::string yc::graphics::shader::data::get_hash(const std::string & name)
{
  yc::fast_hasher hasher;
  for (const auto & [type, uri] : data_sources)
  {
    hasher.add(type);
    hasher.add(get_source(type));
  }

  return "_shader" + name + "_" + hasher.as_string();
}

void yc::graphics::shader::data::set_work_dir(const std::string & dir)
{
  work_dir = dir;
}

void yc::graphics::shader::data::add_source(
      const std::string & type, const yc::data_source & src)
{
  data_sources[type] = src;
}

bool yc::graphics::shader::data::operator==(data & other)
{
  /* Compare URI by content, not as a string */
  if (data_sources.size() != other.data_sources.size())
  {
    return false;
  }

  for (const auto & [type, src] : data_sources)
  {
    if (other.data_sources.find(type) == other.data_sources.end())
    {
      return false;
    }

    if (src != other.data_sources.at(type))
    {
      return false;
    }

    std::string old_source = get_source(type);
    std::string new_source = other.get_source(type);

    if (old_source != new_source)
    {
      return false;
    }
  }

  return true;
}

std::vector<std::string> yc::graphics::shader::data::get_types() const
{
  std::vector<std::string> types;
  for (const auto & [type, uri] : data_sources)
  {
    types.push_back(type);
  }

  return types;
}

std::string yc::graphics::shader::data::get_work_dir() const
{
  return work_dir;
}

std::string yc::graphics::shader::data::get_source(const std::string & type)
{
  if (content_cache.find(type) == content_cache.end())
  {
    std::vector<std::byte> data;
    data_sources.at(type).read(data);
    content_cache[type] = std::string(data.begin(), data.end());
  }

  return content_cache.at(type);
}

bool yc::graphics::shader::data::empty() const
{
  return data_sources.empty();
}

std::string yc::graphics::shader::data::get_language() const
{
  return language;
}

yc::graphics::shader::shader(
      std::string                             name,
      std::shared_ptr<yc::app::asset_cacher>  cacher,
      std::shared_ptr<yc::graphics::graphics> graphics)
: yc::app::asset(std::move(name), std::move(cacher))
, graphics(std::move(graphics))
{
}

void yc::graphics::shader::set_sources(data & _sources)
{
  if (!(sources == _sources))
  {
    logging(LOG_INFO, "Shader '%s' marked to reload", name.c_str());
    sources = _sources;
    live    = false;
  }
}