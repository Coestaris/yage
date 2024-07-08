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
  virtual ~graphics() = default;

  virtual yc::error clear() = 0;
  virtual yc::error flush() = 0;

  virtual std::shared_ptr<yc::graphics::shader> create_shader(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics)
        = 0;
  virtual std::shared_ptr<yc::graphics::node> create_node(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics)
        = 0;
  virtual std::shared_ptr<yc::graphics::texture> create_texture(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics)
        = 0;

  virtual bool supports_shader_language(const std::string & language) = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}
