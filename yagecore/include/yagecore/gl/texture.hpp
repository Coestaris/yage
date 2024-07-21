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
#include "yagecore/gl/util.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class texture final : public yc::graphics::texture
{
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;

  GLuint id {};

public:
  ~        texture() override = default;
  explicit texture(
        std::string                                     name,
        std::shared_ptr<yc::app::asset_cacher>          cacher,
        const std::shared_ptr<yc::graphics::graphics> & graphics);

  yc::error load() override;
  void      unload() override;

  inline void use(GLenum texture_unit);
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}