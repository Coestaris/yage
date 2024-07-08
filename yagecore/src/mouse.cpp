/** ========================================================================
 *
 * @file mouse.cpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
#include <string>
#include <unordered_map>

#include "yagecore/mouse.hpp"

/* Variables ================================================================ */
struct mouse_descriptor
{
  std::string name;
};

/* Static functions ========================================================= */
static const std::unordered_map<yc::mouse, mouse_descriptor> mouse_descriptors
      = {
          { yc::mouse::UNKNOWN, { "UNKNOWN" } },

          { yc::mouse::LEFT, { "LEFT" } },
          { yc::mouse::MIDDLE, { "MIDDLE" } },
          { yc::mouse::RIGHT, { "RIGHT" } },

          { yc::mouse::X1, { "X1" } },
          { yc::mouse::X2, { "X2" } },
          { yc::mouse::X3, { "X3" } },
          { yc::mouse::X4, { "X4" } },
          { yc::mouse::X5, { "X5" } },
          { yc::mouse::X6, { "X6" } },
        };

/* Shared functions ========================================================= */
std::string yc::mouse_to_string(yc::mouse b)
{
  auto it = mouse_descriptors.find(b);
  if (it == mouse_descriptors.end())
  {
    return "UNKNOWN";
  }

  return it->second.name;
}

yc::mouse yc::mouse_from_string(const std::string & str)
{
  for (const auto & [button, descriptor] : mouse_descriptors)
  {
    if (descriptor.name == str)
    {
      return button;
    }
  }

  return yc::mouse::UNKNOWN;
}
