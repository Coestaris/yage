/** ========================================================================
 *
 * @file mesh_buffer.hpp
 * @date 11-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <memory>
#include "yagecore/graphics/draw_result.hpp"
#include "yagecore/graphics/aabb.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class render_pass;

class mesh_buffer
{
public:
  virtual ~mesh_buffer() = default;

  virtual yc::graphics::aabb get_aabb() const = 0;

  virtual void unload() = 0;
  virtual draw_result
  draw(const std::shared_ptr<yc::graphics::render_pass> & pass)
        = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}