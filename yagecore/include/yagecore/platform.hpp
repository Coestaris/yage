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
std::shared_ptr<yc::graphics::window>
create_window(yc::graphics::settings & settings);

std::vector<std::shared_ptr<yc::graphics::render_pass>> trivial_render_chain(
      const std::shared_ptr<yc::graphics::graphics>& graphics,
      const std::shared_ptr<yc::app::asset_factory>& asset_factory);

}
