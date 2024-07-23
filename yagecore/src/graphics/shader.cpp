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
  for (const auto & [type, uri] : uris)
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
      const std::string & type, const yc::util::uri & url)
{
  uris[type] = url;
}

bool yc::graphics::shader::data::operator==(data & other)
{
  /* Compare URI by content, not as a string */
  if (uris.size() != other.uris.size())
  {
    return false;
  }

  for (const auto & [type, uri] : uris)
  {
    if (other.uris.find(type) == other.uris.end())
    {
      return false;
    }

    if (uri != other.uris.at(type))
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
  for (const auto & [type, uri] : uris)
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
  if (sources.find(type) == sources.end())
  {
    std::vector<uint8_t> data;
    uris.at(type).read(data);
    sources[type] = std::string(data.begin(), data.end());
  }

  return sources.at(type);
}

bool yc::graphics::shader::data::empty() const
{
  return uris.empty();
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