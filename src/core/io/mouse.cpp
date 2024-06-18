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

#include "core/io/mouse.hpp"

/* Variables ================================================================ */
struct mouse_descriptor
{
  std::string name;
};

/* Static functions ========================================================= */
static const std::unordered_map<yage::core::io::mouse, mouse_descriptor>
      mouse_descriptors = {
        { yage::core::io::mouse::UNKNOWN, { "UNKNOWN" } },

        { yage::core::io::mouse::LEFT, { "LEFT" } },
        { yage::core::io::mouse::MIDDLE, { "MIDDLE" } },
        { yage::core::io::mouse::RIGHT, { "RIGHT" } },

        { yage::core::io::mouse::X1, { "X1" } },
        { yage::core::io::mouse::X2, { "X2" } },
        { yage::core::io::mouse::X3, { "X3" } },
        { yage::core::io::mouse::X4, { "X4" } },
        { yage::core::io::mouse::X5, { "X5" } },
        { yage::core::io::mouse::X6, { "X6" } },
      };

/* Shared functions ========================================================= */
std::string yage::core::io::mouse_to_string(yage::core::io::mouse b)
{
  auto it = mouse_descriptors.find(b);
  if (it == mouse_descriptors.end())
  {
    return "UNKNOWN";
  }

  return it->second.name;
}

yage::core::io::mouse yage::core::io::mouse_from_string(const std::string & str)
{
  for (const auto & [button, descriptor] : mouse_descriptors)
  {
    if (descriptor.name == str)
    {
      return button;
    }
  }

  return yage::core::io::mouse::UNKNOWN;
}
