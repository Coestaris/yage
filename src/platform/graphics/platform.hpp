/** ========================================================================
 *
 * @file platform.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics/window.hpp"
#include "graphics/shader_compiler.hpp"
#include "graphics/shader.hpp"
#include "graphics/render_pass.hpp"
#include "app/asset_factory.hpp"

namespace yage::platform::graphics
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
yage::graphics::window * create_window(yage::graphics::settings & settings);
void                     destroy_window(yage::graphics::window * window);

std::vector<yage::graphics::render_pass *> trivial_render_chain(
      yage::graphics::graphics * graphics,
      yage::app::asset_factory * asset_factory);

}
