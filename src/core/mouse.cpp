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

#include "mouse.hpp"

/* Variables ================================================================ */
struct mouse_descriptor
{
  std::string name;
};

/* Static functions ========================================================= */
static const std::unordered_map<yage::core::mouse, mouse_descriptor>
      mouse_descriptors = {
        { yage::core::mouse::UNKNOWN, { "UNKNOWN" } },

        { yage::core::mouse::LEFT, { "LEFT" } },
        { yage::core::mouse::MIDDLE, { "MIDDLE" } },
        { yage::core::mouse::RIGHT, { "RIGHT" } },

        { yage::core::mouse::X1, { "X1" } },
        { yage::core::mouse::X2, { "X2" } },
        { yage::core::mouse::X3, { "X3" } },
        { yage::core::mouse::X4, { "X4" } },
        { yage::core::mouse::X5, { "X5" } },
        { yage::core::mouse::X6, { "X6" } },
      };

/* Shared functions ========================================================= */
std::string yage::core::mouse_to_string(yage::core::mouse b)
{
  auto it = mouse_descriptors.find(b);
  if (it == mouse_descriptors.end())
  {
    return "UNKNOWN";
  }

  return it->second.name;
}

yage::core::mouse yage::core::mouse_from_string(const std::string & str)
{
  for (const auto & [button, descriptor] : mouse_descriptors)
  {
    if (descriptor.name == str)
    {
      return button;
    }
  }

  return yage::core::mouse::UNKNOWN;
}
