*
/** ========================================================================
* @file graphics.hpp
* @date 06-06-2024
* @author Maxim Kurylko <vk_vm@ukr.net>
*
*  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include "core/error.hpp"

#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "graphics/shader_compiler.hpp"
#include "graphics/texture_param.hpp"
#include "glm/vec2.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader;
class shader_compiler;
class texture;
class texture_compiler;

class graphics
{
public:
  virtual yage::core::error clear() = 0;
  virtual yage::core::error flush() = 0;

  virtual yage::graphics::shader_compiler * create_shader_compiler() = 0;
  virtual yage::core::error destroy_shader_compiler(yage::graphics::shader_compiler * compiler) = 0;

  virtual yage::graphics::shader * create_shader() = 0;
  virtual yage::core::error destroy_shader(yage::graphics::shader * shader) = 0;

  virtual yage::graphics::texture_compiler * create_texture_compiler() = 0;
  virtual yage::core::error destroy_texture_compiler(yage::graphics::texture_compiler * compiler) = 0;

  virtual yage::core::error destroy_texture(yage::graphics::texture * texture) = 0;

  virtual yage::core::error draw_line(glm::vec2 a, glm::vec2 b) = 0;
  virtual yage::core::error draw_rect(glm::vec2 position, glm::vec2 dim) = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}


