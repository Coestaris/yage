/** ========================================================================
 *
 * @file shader.hpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include "graphics/graphics.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;

class shader
{
protected:
  yage::graphics::graphics * graphics;

public:
  shader(yage::graphics::graphics * graphics) : graphics(graphics) {}
  virtual ~shader() = default;

  /** Uploads binary representation of shader to video memory */
  virtual yage::core::error set_binary(std::vector<uint8_t> binary) = 0;

  /** Deletes shader from video memory */
  virtual yage::core::error destroy() = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}