/** ========================================================================
 *
 * @file texture_param.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include "core/float4.hpp"
namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
enum class tex_type
{
  UNKNOWN,

  TEXTURE_1D,
  TEXTURE_2D,
  TEXTURE_3D,
  TEXTURE_CUBE_MAP,

  MAX_TEXTURE_TYPES
};

enum class tex_filter
{
  UNKNOWN,

  NEAREST,
  LINEAR,

  NEAREST_MIPMAP_NEAREST,
  LINEAR_MIPMAP_NEAREST,
  NEAREST_MIPMAP_LINEAR,
  LINEAR_MIPMAP_LINEAR,

  MAX_FILTER_TYPES
};

enum class tex_pixel_format
{
  UNKNOWN,

  RED,
  RG,
  RGB,
  RGBA,
  DEPTH,
  STENCIL,
  DEPTH_STENCIL,

  MAX_FORMAT_TYPES
};

enum class tex_wrap
{
  UNKNOWN,

  REPEAT,
  CLAMP_TO_EDGE,
  CLAMP_TO_BORDER,
  MIRRORED_REPEAT,

  MAX_WRAP_TYPES
};

struct texture_param
{
  tex_type           type;
  tex_pixel_format   format;
  yage::core::float4 border_color;
  tex_filter         min_filter;
  tex_filter         mag_filter;
  tex_wrap           wrap_s;
  tex_wrap           wrap_t;
  tex_wrap           wrap_r;

  bool operator==(const texture_param & other) const
  {
    return type == other.type && format == other.format
           && border_color == other.border_color
           && min_filter == other.min_filter && mag_filter == other.mag_filter
           && wrap_s == other.wrap_s && wrap_t == other.wrap_t
           && wrap_r == other.wrap_r;
  }

  bool operator!=(const texture_param & other) const
  {
    return !(*this == other);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
std::string to_string(tex_type type);
std::string to_string(tex_filter filter);
std::string to_string(tex_wrap wrap);
std::string to_string(tex_pixel_format format);

tex_type         tex_type_from_string(const std::string & type);
tex_filter       tex_filter_from_string(const std::string & filter);
tex_pixel_format tex_pixel_format_from_string(const std::string & format);
tex_wrap         tex_wrap_from_string(const std::string & wrap);

}