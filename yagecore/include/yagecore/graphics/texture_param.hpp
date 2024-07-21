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
#include "glm/vec4.hpp"

namespace yc::graphics
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

class texture_param
{
private:
  tex_type         type;
  tex_pixel_format format;
  glm::vec4        border_color;
  tex_filter       min_filter;
  tex_filter       mag_filter;
  tex_wrap         wrap_s;
  tex_wrap         wrap_t;
  tex_wrap         wrap_r;

public:
  texture_param() = default;

  void set_type(std::string type);
  void set_type(tex_type type);
  void set_format(std::string format);
  void set_format(tex_pixel_format format);
  void set_border_color(const glm::vec4 & color);
  void set_min_filter(std::string filter);
  void set_min_filter(tex_filter filter);
  void set_mag_filter(std::string filter);
  void set_mag_filter(tex_filter filter);
  void set_wrap_s(std::string wrap);
  void set_wrap_s(tex_wrap wrap);
  void set_wrap_t(std::string wrap);
  void set_wrap_t(tex_wrap wrap);
  void set_wrap_r(std::string wrap);
  void set_wrap_r(tex_wrap wrap);

  [[nodiscard]] tex_type         get_type() const;
  [[nodiscard]] tex_pixel_format get_format() const;
  [[nodiscard]] glm::vec4        get_border_color() const;
  [[nodiscard]] tex_filter       get_min_filter() const;
  [[nodiscard]] tex_filter       get_mag_filter() const;
  [[nodiscard]] tex_wrap         get_wrap_s() const;
  [[nodiscard]] tex_wrap         get_wrap_t() const;
  [[nodiscard]] tex_wrap         get_wrap_r() const;

  bool operator==(const texture_param & other) const;
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