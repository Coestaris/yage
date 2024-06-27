/** ========================================================================
 *
 * @file window.cpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <unordered_map>
#include "graphics/window.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
// clang-format off
static const std::unordered_map<std::string, yage::graphics::settings::window_mode>
win_type_map = {
  {"FULLSCREEN", yage::graphics::settings::window_mode::FULLSCREEN},
  {"WINDOWED_RESIZABLE", yage::graphics::settings::window_mode::WINDOWED_RESIZABLE},
  {"WINDOWED_FIXED", yage::graphics::settings::window_mode::WINDOWED_FIXED},
};

// clang-format on
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::string
yage::graphics::to_string(yage::graphics::settings::window_mode type)
{
  for (const auto & [key, value] : win_type_map)
  {
    if (value == type)
    {
      return key;
    }
  }

  return "UNKNOWN";
}
