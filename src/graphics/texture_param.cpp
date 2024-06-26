/** ========================================================================
 *
 * @file texture_param.cpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <unordered_map>
#include "graphics/texture_param.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
// clang-format off
static const std::unordered_map<std::string, yage::graphics::tex_type>
      tex_type_map
      = { { "TEXTURE_1D", yage::graphics::tex_type::TEXTURE_1D },
          { "TEXTURE_2D", yage::graphics::tex_type::TEXTURE_2D },
          { "TEXTURE_3D", yage::graphics::tex_type::TEXTURE_3D },
          { "TEXTURE_CUBE_MAP", yage::graphics::tex_type::TEXTURE_CUBE_MAP } };

static const std::unordered_map<std::string, yage::graphics::tex_filter>
      tex_filter_map
      = { { "NEAREST", yage::graphics::tex_filter::NEAREST },
          { "LINEAR", yage::graphics::tex_filter::LINEAR },
          { "NEAREST_MIPMAP_NEAREST", yage::graphics::tex_filter::NEAREST_MIPMAP_NEAREST },
          { "LINEAR_MIPMAP_NEAREST", yage::graphics::tex_filter::LINEAR_MIPMAP_NEAREST },
          { "NEAREST_MIPMAP_LINEAR", yage::graphics::tex_filter::NEAREST_MIPMAP_LINEAR },
          { "LINEAR_MIPMAP_LINEAR", yage::graphics::tex_filter::LINEAR_MIPMAP_LINEAR } };

static const std::unordered_map<std::string, yage::graphics::tex_pixel_format>
      tex_pixel_format_map
      = { { "RED", yage::graphics::tex_pixel_format::RED },
          { "RG", yage::graphics::tex_pixel_format::RG },
          { "RGB", yage::graphics::tex_pixel_format::RGB },
          { "RGBA", yage::graphics::tex_pixel_format::RGBA },
          { "DEPTH", yage::graphics::tex_pixel_format::DEPTH },
          { "STENCIL", yage::graphics::tex_pixel_format::STENCIL },
          { "DEPTH_STENCIL", yage::graphics::tex_pixel_format::DEPTH_STENCIL } };

static const std::unordered_map<std::string, yage::graphics::tex_wrap>
      tex_wrap_map
      = { { "REPEAT", yage::graphics::tex_wrap::REPEAT },
          { "MIRRORED_REPEAT", yage::graphics::tex_wrap::MIRRORED_REPEAT },
          { "CLAMP_TO_EDGE", yage::graphics::tex_wrap::CLAMP_TO_EDGE },
          { "CLAMP_TO_BORDER", yage::graphics::tex_wrap::CLAMP_TO_BORDER } };
// clang-format on
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::string yage::graphics::to_string(yage::graphics::tex_type type)
{
  for (const auto & [key, value] : tex_type_map)
  {
    if (value == type)
    {
      return key;
    }
  }

  return "UNKNOWN";
}

yage::graphics::tex_type
yage::graphics::tex_type_from_string(const std::string & type)
{
  if (tex_type_map.find(type) != tex_type_map.end())
  {
    return tex_type_map.at(type);
  }

  return yage::graphics::tex_type::UNKNOWN;
}

std::string
yage::graphics::to_string(yage::graphics::tex_filter filter)
{
  for (const auto & [key, value] : tex_filter_map)
  {
    if (value == filter)
    {
      return key;
    }
  }

  return "UNKNOWN";
}

yage::graphics::tex_filter
yage::graphics::tex_filter_from_string(const std::string & filter)
{
  if (tex_filter_map.find(filter) != tex_filter_map.end())
  {
    return tex_filter_map.at(filter);
  }

  return yage::graphics::tex_filter::UNKNOWN;
}

std::string yage::graphics::to_string(
      yage::graphics::tex_pixel_format format)
{
  for (const auto & [key, value] : tex_pixel_format_map)
  {
    if (value == format)
    {
      return key;
    }
  }

  return "UNKNOWN";
}

yage::graphics::tex_pixel_format
yage::graphics::tex_pixel_format_from_string(const std::string & format)
{

  if (tex_pixel_format_map.find(format) != tex_pixel_format_map.end())
  {
    return tex_pixel_format_map.at(format);
  }

  return yage::graphics::tex_pixel_format::UNKNOWN;
}

std::string yage::graphics::to_string(yage::graphics::tex_wrap wrap)
{
  for (const auto & [key, value] : tex_wrap_map)
  {
    if (value == wrap)
    {
      return key;
    }
  }

  return "UNKNOWN";
}

yage::graphics::tex_wrap
yage::graphics::tex_wrap_from_string(const std::string & wrap)
{
  if (tex_wrap_map.find(wrap) != tex_wrap_map.end())
  {
    return tex_wrap_map.at(wrap);
  }

  return yage::graphics::tex_wrap::UNKNOWN;
}
