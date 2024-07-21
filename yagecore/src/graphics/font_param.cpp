/** ========================================================================
 *
 * @file font_param.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/font_param.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::graphics::font_param::font_param(int size, int start_char, int end_char)
: size(size)
, start_char(start_char)
, end_char(end_char)
{
}

void yc::graphics::font_param::set_size(int _size)
{
  size = _size;
}

void yc::graphics::font_param::set_start_char(int _start_char)
{
  start_char = _start_char;
}

void yc::graphics::font_param::set_end_char(int _end_char)
{
  end_char = _end_char;
}

int yc::graphics::font_param::get_size() const
{
  return size;
}

int yc::graphics::font_param::get_start_char() const
{
  return start_char;
}

int yc::graphics::font_param::get_end_char() const
{
  return end_char;
}

bool yc::graphics::font_param::operator==(const font_param & other) const
{
  return size == other.size && start_char == other.start_char
         && end_char == other.end_char;
}
