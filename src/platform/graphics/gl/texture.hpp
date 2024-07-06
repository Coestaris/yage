/** ========================================================================
 *
 * @file image.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics/texture.hpp"
#include "GL/gl.h"
#include "graphics.hpp"

namespace yage::platform::graphics::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class texture : public yage::graphics::texture
{
private:
  GLuint                                   id = 0;
  yage::platform::graphics::gl::graphics * ctx;

public:
  ~texture() override = default;
  explicit texture(
        std::string                    name,
        yage::graphics::asset_cacher * cacher,
        yage::graphics::graphics *     graphics)
  : yage::graphics::texture(name, cacher, graphics)
  , ctx(dynamic_cast<yage::platform::graphics::gl::graphics *>(graphics))
  {
  }

  yage::core::error load() override;
  void              unload() override;

  inline void use(GLenum texture_unit)
  {
    ctx->glActiveTexture(texture_unit);
    ctx->glBindTexture(GL_TEXTURE_2D, id);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}