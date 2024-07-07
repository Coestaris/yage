/** ========================================================================
 *
 * @file node.hpp
 * @date 07-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics/node.hpp"
#include "GL/gl.h"
#include "graphics.hpp"

namespace yage::platform::graphics::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class node : public yage::graphics::node
{
private:
  yage::platform::graphics::gl::graphics * ctx;

public:
  ~node() override = default;
  explicit node(
        std::string                    name,
        yage::graphics::asset_cacher * cacher,
        yage::graphics::graphics *     graphics)
  : yage::graphics::node(std::move(name), cacher, graphics)
  , ctx(dynamic_cast<yage::platform::graphics::gl::graphics *>(graphics))
  {
  }

  yage::core::error load() override;
  void              unload() override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}