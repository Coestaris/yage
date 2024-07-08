/** ========================================================================
 *
 * @file image.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>

#include "GL/gl.h"
#include "yagecore/graphics/texture.hpp"
#include "yagecore/gl/graphics.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class texture : public yc::graphics::texture
{
private:
  GLuint             id = 0;
  yc::gl::graphics * ctx;

public:
  ~texture() override = default;
  explicit texture(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics)
  : yc::graphics::texture(std::move(name), cacher, graphics)
  , ctx(dynamic_cast<yc::gl::graphics *>(graphics))
  {
  }

  yc::error load() override;
  void      unload() override;

  inline void use(GLenum texture_unit)
  {
    ctx->glActiveTexture(texture_unit);
    ctx->glBindTexture(GL_TEXTURE_2D, id);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}