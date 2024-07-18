/** ========================================================================
 *
 * @file font_param.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class font_param
{
  int size       = 12;
  int start_char = 32;
  int end_char   = 127;

public:
  font_param() = default;
  font_param(int size, int start_char, int end_char)
  : size(size)
  , start_char(start_char)
  , end_char(end_char)
  {
  }

  void set_size(int _size)
  {
    size = _size;
  }

  void set_start_char(int _start_char)
  {
    start_char = _start_char;
  }

  void set_end_char(int _end_char)
  {
    end_char = _end_char;
  }

  int get_size() const
  {
    return size;
  }

  int get_start_char() const
  {
    return start_char;
  }

  int get_end_char() const
  {
    return end_char;
  }

  bool operator==(const font_param & other) const
  {
    return size == other.size && start_char == other.start_char
           && end_char == other.end_char;
  }
};
/* Variables ================================================================ */
/* Prototypes =============================================================== */
}