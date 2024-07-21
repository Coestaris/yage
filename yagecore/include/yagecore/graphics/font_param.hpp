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
  font_param(int size, int start_char, int end_char);

  void set_size(int size);
  void set_start_char(int start_char);
  void set_end_char(int end_char);

  [[nodiscard]] int get_size() const;
  [[nodiscard]] int get_start_char() const;
  [[nodiscard]] int get_end_char() const;

  bool operator==(const font_param & other) const;
};
/* Variables ================================================================ */
/* Prototypes =============================================================== */
}