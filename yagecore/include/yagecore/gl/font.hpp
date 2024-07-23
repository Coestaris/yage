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
#include "yagecore/gl/texture.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class char_renderer
{
public:
  virtual ~    char_renderer()                                     = default;
  virtual void begin(GLuint vao, glm::vec4 color, GLuint atlas_id) = 0;
  virtual void end()                                               = 0;
  virtual void
  on_char(const yc::graphics::font::char_cache & c, glm::mat4 model)
        = 0;
};

class font final : public yc::graphics::font
{
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;

  /* Attention: this asset is not handled by factory,
   * we need to unload it manually */
  std::shared_ptr<yc::gl::texture> atlas {};
  GLuint                           id;
  GLuint                           vao, vbo, ebo;

  std::shared_ptr<yc::gl::char_renderer> char_renderer;

  void draw_begin(glm::vec4 color) override;
  void draw_end() override;
  void draw(const char_cache & c, glm::vec2 pos) override;

public:
  ~font() override = default;
  explicit
  font(std::string                                     name,
       std::shared_ptr<yc::app::asset_cacher>          cacher,
       const std::shared_ptr<yc::graphics::graphics> & graphics);

  void set_char_renderer(std::shared_ptr<yc::gl::char_renderer> renderer);

  yc::error load() override;
  void      unload() override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
