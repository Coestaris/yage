/** ========================================================================
 *
 * @file node.hpp
 * @date 07-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "GL/gl.h"
#include "yagecore/graphics/node.hpp"
#include "yagecore/gl/graphics.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class node : public yc::graphics::node
{
private:
  yc::gl::graphics * ctx;

public:
  ~node() override = default;
  explicit node(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics)
  : yc::graphics::node(std::move(name), cacher, graphics)
  , ctx(dynamic_cast<yc::gl::graphics *>(graphics))
  {
  }

  yc::error load() override;
  void      unload() override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}