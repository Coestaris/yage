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
#include <map>

#include <GL/gl.h>

#include "graphics/shader_compiler.hpp"
#include "graphics.hpp"

namespace yage::platform::graphics::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader_compiler : public yage::graphics::shader_compiler
{
private:
  /* ordered because we don't want to have
   * different hashes for the same shaders */
  std::map<yage::graphics::shader_type, std::string> sources;
  std::vector<uint8_t>                               binary;
  yage::platform::graphics::gl::graphics *           ctx;

public:
  shader_compiler(yage::graphics::graphics * graphics)
  : yage::graphics::shader_compiler(graphics)
  , ctx(dynamic_cast<yage::platform::graphics::gl::graphics *>(graphics))
  {
  }
  ~shader_compiler() override = default;

  void clear() override;
  yage::core::error add_shader(std::string source, yage::graphics::shader_type type) override;
  yage::core::error compile(std::vector<compile_message> & out_messages) override;
  yage::core::error get_binary(std::vector<uint8_t> & out_data) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
