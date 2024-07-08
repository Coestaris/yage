/** ========================================================================
 * @file graphics.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include "glm/vec2.hpp"

#include "yagecore/error.hpp"
#include "yagecore/graphics/shader.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader;
class texture;
class node;
class asset_cacher;

class graphics
{
public:
  virtual yc::error clear() = 0;
  virtual yc::error flush() = 0;

  virtual yc::graphics::shader * create_shader(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics)
        = 0;
  virtual yc::graphics::node * create_node(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics)
        = 0;
  virtual yc::graphics::texture * create_texture(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics)
        = 0;

  virtual yc::error destroy_shader(yc::graphics::shader * shader)    = 0;
  virtual yc::error destroy_node(yc::graphics::node * node)          = 0;
  virtual yc::error destroy_texture(yc::graphics::texture * texture) = 0;

  virtual bool supports_shader_language(std::string language) = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}
