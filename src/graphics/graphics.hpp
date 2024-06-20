/** ========================================================================
*
* @file graphics.hpp
* @date 06-06-2024
* @author Maxim Kurylko <vk_vm@ukr.net>
*
*  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include "core/error.hpp"
#include "core/float2.hpp"

#include "graphics/shader.hpp"
#include "graphics/shader_compiler.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader;
class shader_compiler;

class graphics
{
public:
  virtual yage::core::error clear() = 0;
  virtual yage::core::error flush() = 0;

  virtual yage::graphics::shader_compiler * create_shader_compiler() = 0;
  virtual yage::core::error destroy_shader_compiler(yage::graphics::shader_compiler * compiler) = 0;
  virtual yage::graphics::shader * create_shader() = 0;
  virtual yage::core::error destroy_shader(yage::graphics::shader * shader) = 0;

  virtual yage::core::error draw_line(yage::core::float2 a, yage::core::float2 b) = 0;
  virtual yage::core::error draw_rect(yage::core::float2 position, yage::core::float2 dim) = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}


