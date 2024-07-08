/** ========================================================================
 *
 * @file shader_compiler.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>
#include <vector>
#include <string>

#include "yagecore/graphics/shader.hpp"
#include "yagecore/gl/graphics.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader : public yc::graphics::shader
{
protected:
  GLint              id;
  yc::gl::graphics * ctx;

  yc::error load_from_binary(const std::vector<uint8_t> & binary);
  yc::error compile_binary(std::vector<uint8_t> & binary);

public:
  ~shader() override = default;
  shader(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics)
  : yc::graphics::shader(std::move(name), cacher, graphics)
  , ctx(dynamic_cast<yc::gl::graphics *>(graphics))
  {
  }

  yc::error load() override;
  void      unload() override;

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
