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
  std::unordered_map<std::string, int> uniform_cache;

  virtual int get_uniform_location_direct(std::string name) = 0;

public:
  shader(yage::graphics::graphics * graphics) : graphics(graphics) {}
  virtual ~shader() = default;

  /** Uploads binary representation of shader to video memory */
  virtual yage::core::error set_binary(std::vector<uint8_t> binary);

  /** Deletes shader from video memory */
  virtual yage::core::error destroy() = 0;

  /* Reflection */
  virtual std::vector<std::string> get_uniforms()   = 0;
  virtual std::vector<std::string> get_attributes() = 0;

  virtual yage::core::error bind();
  virtual yage::core::error unbind();

  int get_uniform_location(std::string name);

  virtual yage::core::error set_uniform_1i(int location, int value);
  virtual yage::core::error set_uniform_1f(int location, float value);
  virtual yage::core::error set_uniform_2f(int location, glm::vec2 value);
  virtual yage::core::error set_uniform_3f(int location, glm::vec3 value);
  virtual yage::core::error set_uniform_mat4f(int location, float * value);

  yage::core::error set_uniform_1i(std::string name, int value);
  yage::core::error set_uniform_1f(std::string name, float value);
  yage::core::error set_uniform_2f(std::string name, glm::vec2 value);
  yage::core::error set_uniform_3f(std::string name, glm::vec3 value);
  yage::core::error set_uniform_mat4f(std::string name, float * value);
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}