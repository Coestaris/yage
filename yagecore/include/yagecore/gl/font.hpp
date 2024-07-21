/** ========================================================================
 *
 * @file font.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>

#include "GL/gl.h"
#include "yagecore/graphics/font.hpp"
#include "yagecore/gl/graphics.hpp"
#include "yagecore/gl/util.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class font final : public yc::graphics::font
{
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;

  GLuint id {};

  void
  draw(const char_cache & c,
       glm::vec2          pos,
       glm::vec4          color,
       glm::vec2          scale) override;

public:
  ~font() override = default;
  explicit
  font(std::string                                     name,
       std::shared_ptr<yc::app::asset_cacher>          cacher,
       const std::shared_ptr<yc::graphics::graphics> & graphics);

  yc::error load() override;
  void      unload() override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
