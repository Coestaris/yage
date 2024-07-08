/** ========================================================================
 *
 * @file platform.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "yagecore/graphics/window.hpp"
#include "yagecore/graphics/shader.hpp"
#include "yagecore/graphics/render_pass.hpp"
#include "yagecore/app/asset_factory.hpp"

namespace yc
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
yc::graphics::window * create_window(yc::graphics::settings & settings);
void                   destroy_window(yc::graphics::window * window);

std::vector<yc::graphics::render_pass *> trivial_render_chain(
      yc::graphics::graphics * graphics,
      yc::app::asset_factory * asset_factory);

}
