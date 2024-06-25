/** ========================================================================
 *
 * @file shader_compiler.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <string>

#include "graphics/shader.hpp"

namespace yage::platform::graphics::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader : public yage::graphics::shader
{
public:
  explicit shader(yage::graphics::graphics * graphics)
  : yage::graphics::shader(graphics)
  {
  }
  ~shader() override = default;

  yage::core::error        set_binary(std::vector<uint8_t> binary) override;
  yage::core::error        destroy() override;
  std::vector<std::string> get_uniforms() override;
  std::vector<std::string> get_attributes() override;
  yage::core::error        bind() override;
  yage::core::error        unbind() override;

  int               get_uniform_location_direct(std::string name) override;
  yage::core::error set_uniform_1i(int location, int value) override;
  yage::core::error set_uniform_1f(int location, float value) override;
  yage::core::error
  set_uniform_2f(int location, yage::core::float2 value) override;
  yage::core::error
  set_uniform_3f(int location, yage::core::float3 value) override;
  yage::core::error set_uniform_mat4f(int location, float * value) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
