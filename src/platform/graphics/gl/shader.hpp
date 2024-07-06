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
#include "graphics.hpp"

namespace yage::platform::graphics::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader : public yage::graphics::shader
{
protected:
  GLint                                    id;
  yage::platform::graphics::gl::graphics * ctx;

  yage::core::error load_from_binary(const std::vector<uint8_t> & binary);
  yage::core::error compile_binary(std::vector<uint8_t> & binary);

public:
  ~shader() override = default;
  shader(
        std::string                    name,
        yage::graphics::asset_cacher * cacher,
        yage::graphics::graphics *     graphics)
  : yage::graphics::shader(name, cacher, graphics)
  , ctx(dynamic_cast<platform::graphics::gl::graphics *>(graphics))
  {
  }

  yage::core::error load() override;
  void              unload() override;

  /* GL specific sfuff */
  GLint get_uniform_location(const std::string & name, bool required = true);
  GLint get_attribute_location(const std::string & name, bool required = true);

  inline void use()
  {
    ctx->glUseProgram(id);
  }

  inline void set_uniform(GLint location, int value)
  {
    ctx->glUniform1i(location, value);
  }

  inline void set_uniform(GLint location, float value)
  {
    ctx->glUniform1f(location, value);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
