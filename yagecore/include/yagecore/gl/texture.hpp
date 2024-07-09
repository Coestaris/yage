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
#include "glplatform.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class texture : public yc::graphics::texture
{
private:
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;

  GLuint id {};

public:
  ~texture() override = default;
  explicit texture(
        std::string                                     name,
        std::shared_ptr<yc::app::asset_cacher>          cacher,
        const std::shared_ptr<yc::graphics::graphics> & graphics)
  : yc::graphics::texture(std::move(name), std::move(cacher), graphics)
  , ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics))
  {
  }

  yc::error load() override;
  void      unload() override;

  inline void use(GLenum texture_unit)
  {
    yc::gl::glActiveTexture(texture_unit);
    yc::gl::glBindTexture(GL_TEXTURE_2D, id);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}