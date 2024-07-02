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

namespace yage::platform::graphics::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class texture : public yage::graphics::texture
{
private:
  GLuint id {};

public:
  ~texture() override = default;
  explicit texture(
        yage::graphics::graphics *            graphics,
        const yage::graphics::texture_param & parameters,
        GLuint                                id)
  : yage::graphics::texture(graphics, parameters)
  , id(id)
  {
  }

  yage::core::error destroy() override;

  inline void use(GLenum texture_unit)
  {
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, id);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}