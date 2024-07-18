/** ========================================================================
 *
 * @file graphics.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <string>

#include "GL/gl.h"
#include "GL/glext.h"
#include "yagecore/gl/mesh_buffer.hpp"
#include <yagecore/graphics/graphics.hpp>

namespace yc::gl
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics final : public yc::graphics::graphics
{
private:
  std::vector<std::string> extensions;

  void cache_extensions();
  void print_extensions();
  bool has_extension(const std::string & extension);

public:
  explicit graphics(const std::shared_ptr<yc::graphics::window> & window)
  : yc::graphics::graphics(window) {};
  ~graphics() override = default;

  yc::error init(int bits_per_pixel, int width, int height);

  std::shared_ptr<yc::graphics::shader> create_shader(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics) override;
  std::shared_ptr<yc::graphics::node> create_node(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics) override;
  std::shared_ptr<yc::graphics::texture> create_texture(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics) override;
  std::shared_ptr<yc::graphics::font> create_font(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics) override;

  bool supports_shader_language(const std::string & language) override
  {
    return language == "GLSL";
  }

  yc::error clear() override;
  yc::error flush() override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
